#include <assert.h>
#include <fcntl.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

struct AST {
    char *ast_type;      // 生成規則を区別
    struct AST *parent;  // 親へのバックポインタ
    int nth;             // 自分が何番目の兄弟か
    int num_child;       // 子ノードの数
    struct AST **child;  // 子ノードポインタの配列
    char *lexeme;        // 葉ノード用 (整数，文字，文字列の定数，識別子）
};

/* ------------------------------------------------------- */
static void print_nspace(int n);
static struct AST *create_AST(char *ast_type, int num_child, ...);
static struct AST *create_leaf(char *ast_type, char *lexeme);
static struct AST *add_AST(struct AST *ast, int num_child, ...);
static void show_AST(struct AST *ast, int depth);
static void unparse_AST(struct AST *ast, int depth);

static struct AST *parse_translation_unit(void);
static struct AST *parse_type_specifier(void);
static struct AST *parse_declarator(void);
static struct AST *parse_primary(void);
static struct AST *parse_expression(void);
static struct AST *parse_statement(void);
static struct AST *parse_compound_statement(void);

static char *map_file(char *filename);
static void my_regcomp(regex_t *regex_p, char *pattern);
static int my_regexec(regex_t *regex_p, char *str, regmatch_t *regmatch_p);
static void *copy_string_region_int(char *s, int start, int end);
static void *copy_string_region_ptr(char *start, char *end);
static void *copy_string_region_regmatch(char *s, regmatch_t *regmatch_p);
static int skip_block_comment(char *ptr, int off);
static int skip_whitespaces(char *ptr, int off);
static int strncmp_r(char *s1, char *s2);
static int check_kind_id(char *lexeme);
static int set_token_int(char *ptr, int begin, int end, int kind, int off);
static int set_token_regmatch(char *ptr, regmatch_t *regmatch_p, int kind, int off);
static void create_tokens(char *ptr);
static void dump_tokens();
/* ------------------------------------------------------- */
// データ構造と変数

#define MAX_TOKENS 10000
struct token {
    int kind;
    int offset_begin;
    int offset_end;
    char *lexeme;
};
enum token_kind {
    TK_UNUSED = 0,
    TK_ID = 1,
    TK_INT = 2,
    TK_CHAR = 3,
    TK_STRING = 4,
    TK_KW_CHAR = 5,      // char
    TK_KW_ELSE = 6,      // else
    TK_KW_GOTO = 7,      // goto
    TK_KW_IF = 8,        // if
    TK_KW_INT = 9,       // int
    TK_KW_RETURN = 10,   // return
    TK_KW_VOID = 11,     // void
    TK_KW_WHILE = 12,    // while
    TK_OP_EQ = 13,       // ==
    TK_OP_AND = 14,      // &&
    TK_OP_OR = 15,       // ||
    TK_KW_LONG = 16,     // long
    TK_COMMENT = 17,     // デバッグ用
    TK_WHITESPACE = 18,  // デバッグ用
    // 以下は名前を付けずにそのまま使う
    // ';' ':' '{' '}' ',' '=' '(' ')' '&' '!' '-' '+' '*' '/' '<'
};

char *token_kind_string[] = {
    "UNUSED",
    "ID",
    "INT",
    "CHAR",
    "STRING",
    "char",
    "else",
    "goto",
    "if",
    "int",
    "return",
    "void",
    "while",
    "==",
    "&&",
    "||",
    "long",
    [';'] = ";",
    [':'] = ":",
    ['{'] = "{",
    ['}'] = "}",
    [','] = ",",
    ['='] = "=",
    ['('] = "(",
    [')'] = ")",
    ['&'] = "&",
    ['!'] = "!",
    ['-'] = "-",
    ['+'] = "+",
    ['*'] = "*",
    ['/'] = "/",
    ['<'] = "<",
};

static struct token tokens[MAX_TOKENS];
static int tokens_index = 0;
static struct token *token_p;  // for parsing

/* ------------------------------------------------------- */
static void print_nspace(int n) {
    printf("%*s", n, "");
}

static void printf_ns(int n, const char *format, ...) {
    va_list arg;
    print_nspace(n * 4);  // 空文字列をn*4幅で印字
    va_start(arg, format);
    vprintf(format, arg);
    va_end(arg);
}

static struct AST *create_AST(char *ast_type, int num_child, ...) {
    va_list ap;
    struct AST *ast;
    ast = malloc(sizeof(struct AST));
    ast->parent = NULL;
    ast->nth = -1;
    ast->ast_type = ast_type;
    ast->num_child = num_child;
    ast->lexeme = NULL;
    va_start(ap, num_child);
    if (num_child == 0) {
        ast->child = NULL;
    } else {
        int i;
        ast->child = malloc(sizeof(struct AST *) * num_child);
        for (i = 0; i < num_child; i++) {
            struct AST *child = va_arg(ap, struct AST *);
            ast->child[i] = child;
            if (child != NULL) {
                child->parent = ast;
                child->nth = i;
            }
        }
    }
    va_end(ap);
    return ast;
}

static struct AST *create_leaf(char *ast_type, char *lexeme) {
    struct AST *ast;
    ast = malloc(sizeof(struct AST));
    ast->parent = NULL;
    ast->nth = -1;
    ast->ast_type = ast_type;
    ast->num_child = 0;
    ast->child = NULL;
    ast->lexeme = lexeme;
    return ast;
}

static struct AST *add_AST(struct AST *ast, int num_child, ...) {
    va_list ap;
    int i, start = ast->num_child;
    ast->num_child += num_child;
    assert(num_child > 0);
    ast->child = realloc(ast->child, sizeof(struct AST *) * ast->num_child);
    va_start(ap, num_child);
    for (i = start; i < ast->num_child; i++) {
        struct AST *child = va_arg(ap, struct AST *);
        ast->child[i] = child;
        if (child != NULL) {
            child->parent = ast;
            child->nth = i;
        }
    }
    va_end(ap);
    return ast;
}

static void show_AST(struct AST *ast, int depth) {
    int i;
    print_nspace(depth);
    if (!strcmp(ast->ast_type, "TK_ID") || !strcmp(ast->ast_type, "TK_INT") || !strcmp(ast->ast_type, "TK_CHAR") || !strcmp(ast->ast_type, "TK_STRING")) {
        printf("%s (%s)\n", ast->ast_type, ast->lexeme);
    } else {
        printf("%s\n", ast->ast_type);
    }

    for (i = 0; i < ast->num_child; i++) {
        if (ast->child[i] != NULL) {
            show_AST(ast->child[i], depth + 1);
        }
    }
}
/* ------------------------------------------------------- */
// 構文解析

static void parse_error(void) {
    fprintf(stderr, "parse error (%d-%d): %s (%s)\n",
            token_p->offset_begin, token_p->offset_end,
            token_kind_string[token_p->kind], token_p->lexeme);
    exit(1);
}

static int lookahead(int i) {
    return tokens[tokens_index + i - 1].kind;
}

static struct token *next_token(void) {
    token_p = &tokens[++tokens_index];
    assert(tokens_index < MAX_TOKENS);
    return token_p;
}

static struct token *reset_tokens(void) {
    tokens_index = 0;
    token_p = &tokens[tokens_index];
    return token_p;
}

static void consume_token(enum token_kind kind) {
    if (lookahead(1) == kind) {
        next_token();
    } else {
        parse_error();
    }
}

// type_specifier: "void" | "char" | "int" | "long"
static struct AST *parse_type_specifier() {
    struct AST *ast, *ast1;
    ast = create_AST("type_specifier", 0);

    if (lookahead(1) == TK_KW_VOID) {
        consume_token(TK_KW_VOID);
        ast1 = create_AST("TK_KW_VOID", 0);
        ast = add_AST(ast, 1, ast1);
    } else if (lookahead(1) == TK_KW_CHAR) {
        consume_token(TK_KW_CHAR);
        ast1 = create_AST("TK_KW_CHAR", 0);
        ast = add_AST(ast, 1, ast1);
    } else if (lookahead(1) == TK_KW_INT) {
        consume_token(TK_KW_INT);
        ast1 = create_AST("TK_KW_INT", 0);
        ast = add_AST(ast, 1, ast1);
    } else if (lookahead(1) == TK_KW_LONG) {
        consume_token(TK_KW_LONG);
        ast1 = create_AST("TK_KW_LONG", 0);
        ast = add_AST(ast, 1, ast1);
    } else {
        parse_error();
    }
    return ast;
}

// declarator: IDENTIFIER | IDENTIFIER "(" ")"
static struct AST *parse_declarator() {
    struct AST *ast, *ast1, *ast2, *ast3;
    ast = create_AST("declarator", 0);

    if (lookahead(1) == TK_ID) {
        ast1 = create_AST("TK_ID", 0);
        ast1->lexeme = token_p->lexeme;
        consume_token(TK_ID);

        if (lookahead(1) == '(') {
            ast2 = create_AST("(", 0);
            consume_token('(');
            assert(lookahead(1) == ')');
            ast3 = create_AST(")", 0);
            consume_token(')');

            ast = add_AST(ast, 3, ast1, ast2, ast3);

        } else {
            ast = add_AST(ast, 1, ast1);
        }
    } else {
        parse_error();
    }
    return ast;
}

// primary: INTEGER | CHARACTER | STRING | IDENTIFIER | "(" expression ")"
static struct AST *parse_primary() {
    struct AST *ast, *ast1, *ast2, *ast3;
    ast = create_AST("primary", 0);

    if (lookahead(1) == TK_INT) {  // primary : INTEGER

        ast1 = create_AST("TK_INT", 0);
        ast1->lexeme = token_p->lexeme;
        consume_token(TK_INT);

        ast = add_AST(ast, 1, ast1);
    } else if (lookahead(1) == TK_CHAR) {  // primary : CHARACTER

        ast1 = create_AST("TK_CHAR", 0);
        ast1->lexeme = token_p->lexeme;
        consume_token(TK_CHAR);

        ast = add_AST(ast, 1, ast1);
    } else if (lookahead(1) == TK_STRING) {  // primary : STRING

        ast1 = create_AST("TK_STRING", 0);
        ast1->lexeme = token_p->lexeme;
        consume_token(TK_STRING);

        ast = add_AST(ast, 1, ast1);
    } else if (lookahead(1) == TK_ID) {  // primary : IDENTIFIER

        ast1 = create_AST("TK_ID", 0);
        ast1->lexeme = token_p->lexeme;
        consume_token(TK_ID);

        ast = add_AST(ast, 1, ast1);
    } else if (lookahead(1) == '(') {  // primary : "(" expression ")"

        ast1 = create_AST("(", 0);
        consume_token('(');
        ast2 = parse_expression();
        assert(lookahead(1) == ')');
        ast3 = create_AST(")", 0);
        consume_token(')');

        ast = add_AST(ast, 3, ast1, ast2, ast3);
    } else {
        parse_error();
    }
    return ast;
}

// expression: primary ( "(" ")" )?
static struct AST *parse_expression() {
    struct AST *ast, *ast1, *ast2, *ast3;
    ast = create_AST("expression", 0);

    if (lookahead(1) == TK_INT || lookahead(1) == TK_CHAR || lookahead(1) == TK_STRING || lookahead(1) == TK_ID || lookahead(1) == '(') {
        ast1 = parse_primary();

        if (lookahead(1) == '(') {  // exp : primary "("  ")"
            ast2 = create_AST("(", 0);
            consume_token('(');
            assert(lookahead(1) == ')');
            ast3 = create_AST(")", 0);
            consume_token(')');

            ast = add_AST(ast, 3, ast1, ast2, ast3);
        } else {
            // exp : primary
            ast = add_AST(ast, 1, ast1);
        }
    } else {
        parse_error();
    }
    return ast;
}

// statement
/*
    statement:
        IDENTIFIER ":" |
        "if" "(" expression ")" statement ("else" statement)? |
        "while" "(" expression ")" statement |
        "goto IDENTIFIER ";" |
        "return" (expression)? ";" |
        ( expression )? ";"
*/
static struct AST *parse_statement() {
    struct AST *ast;
    ast = create_AST("statement", 0);

    if (lookahead(1) == TK_ID && lookahead(2) == ':') {  // IDENTIFIER ":"
        struct AST *ast1, *ast2;

        ast1 = create_AST("TK_ID", 0);
        ast1->lexeme = token_p->lexeme;
        consume_token(TK_ID);
        assert(lookahead(1) == ':');
        ast2 = create_AST(":", 0);
        consume_token(':');

        ast = add_AST(ast, 2, ast1, ast2);
    } else if (lookahead(1) == '{') {  // compound_statement
        struct AST *ast1;
        ast1 = parse_compound_statement();
        ast = add_AST(ast, 1, ast1);
    } else if (lookahead(1) == TK_KW_IF) {  // "if" "(" expression ")" statement ("else" statement)?
        struct AST *ast1, *ast2, *ast3, *ast4, *ast5;

        ast1 = create_AST("TK_KW_IF", 0);
        consume_token(TK_KW_IF);
        assert(lookahead(1) == '(');
        ast2 = create_AST("(", 0);
        consume_token('(');
        ast3 = parse_expression();
        assert(lookahead(1) == ')');
        ast4 = create_AST(")", 0);
        consume_token(')');
        ast5 = parse_statement();

        // 1個先のtokenが else なら if else 文と解釈
        if (lookahead(1) == TK_KW_ELSE) {
            // statement : "if" "(" expression ")" statement "else" statement
            struct AST *ast6, *ast7;

            ast6 = create_AST("TK_KW_ELSE", 0);
            consume_token(TK_KW_ELSE);
            ast7 = parse_statement();

            ast = add_AST(ast, 7, ast1, ast2, ast3, ast4, ast5, ast6, ast7);
        } else {
            // statement : if "(" expression ")" statement
            ast = add_AST(ast, 5, ast1, ast2, ast3, ast4, ast5);
        }
    } else if (lookahead(1) == TK_KW_WHILE) {  // "while" "(" expression ")" statement
        struct AST *ast1, *ast2, *ast3, *ast4, *ast5;

        ast1 = create_AST("TK_KW_WHILE", 0);
        consume_token(TK_KW_WHILE);
        assert(lookahead(1) == '(');
        ast2 = create_AST("(", 0);
        consume_token('(');
        ast3 = parse_expression();
        assert(lookahead(1) == ')');
        ast4 = create_AST(")", 0);
        consume_token(')');
        ast5 = parse_statement();

        ast = add_AST(ast, 5, ast1, ast2, ast3, ast4, ast5);
    } else if (lookahead(1) == TK_KW_GOTO) {  // "goto IDENTIFIER ";"
        struct AST *ast1, *ast2, *ast3;

        ast1 = create_AST("TK_KW_GOTO", 0);
        consume_token(TK_KW_GOTO);
        assert(lookahead(1) == TK_ID);
        ast2 = create_AST("TK_ID", 0);
        ast2->lexeme = token_p->lexeme;
        consume_token(TK_ID);
        assert(lookahead(1) == ';');
        ast3 = create_AST(";", 0);
        consume_token(';');

        ast = add_AST(ast, 3, ast1, ast2, ast3);
    } else if (lookahead(1) == TK_KW_RETURN) {  // "return" (expression)? ";"
        struct AST *ast1, *ast2, *ast3;

        ast1 = create_AST("TK_KW_RETURN", 0);
        consume_token(TK_KW_RETURN);
        if (lookahead(1) != ';') {
            ast2 = parse_expression();
            assert(lookahead(1) == ';');
            ast3 = create_AST(";", 0);
            consume_token(';');

            ast = add_AST(ast, 3, ast1, ast2, ast3);
        } else {
            ast2 = create_AST(";", 0);
            consume_token(';');

            ast = add_AST(ast, 2, ast1, ast2);
        }
    } else if (lookahead(1) == ';') {  // ";"
        struct AST *ast1;

        ast1 = create_AST(";", 0);
        consume_token(';');

        ast = add_AST(ast, 1, ast1);
    } else if (lookahead(1) == TK_INT || lookahead(1) == TK_CHAR || lookahead(1) == TK_STRING || lookahead(1) == TK_ID || lookahead(1) == '(') {  //  expression  ";"
        // expression := primary ( "(" ")" )? ";"
        // primary := INTEGER | CHARACTER | STRING | IDENTIFIER | "(" expression ")"
        struct AST *ast1, *ast2;

        ast1 = parse_expression();
        assert(lookahead(1) == ';');
        ast2 = create_AST(";", 0);
        consume_token(';');

        ast = add_AST(ast, 2, ast1, ast2);
    } else {
        parse_error();
    }

    return ast;
}

// compound_statement: "{" (type_specifier declarator ";")*  ( statement )*  "}"
static struct AST *parse_compound_statement() {
    struct AST *ast, *ast1, *ast2, *ast3, *ast4, *ast5, *ast6;
    ast = create_AST("compound_statement", 0);

    if (lookahead(1) == '{') {
        consume_token('{');
        ast1 = create_AST("{", 0);

        ast = add_AST(ast, 1, ast1);
        // ( type_specifier declarator ";" ) *
        while (lookahead(1) == TK_KW_VOID || lookahead(1) == TK_KW_CHAR || lookahead(1) == TK_KW_INT || lookahead(1) == TK_KW_LONG) {  // type_specifier があるかどうか

            ast2 = parse_type_specifier();
            ast3 = parse_declarator();
            consume_token(';');
            ast4 = create_AST(";", 0);

            ast = add_AST(ast, 3, ast2, ast3, ast4);
            // ( type_specifier declarator ";" ) * より、その都度追加する
        }

        // ( statement ) *
        while (1) {
            if (lookahead(1) == '}') {
                break;
            } else {
                ast5 = parse_statement();

                ast = add_AST(ast, 1, ast5);
                // ( statement ) * より、その都度追加する
            }
        }

        // "}"
        if (lookahead(1) == '}') {
            consume_token('}');
            ast6 = create_AST("}", 0);

            ast = add_AST(ast, 1, ast6);
        } else {
            parse_error();
        }
    } else {
        parse_error();
    }
    return ast;
}

// translation_unit: ( type_specifier declarator ( ";" | compound_statement ))*
static struct AST *parse_translation_unit(void) {
    struct AST *ast, *ast1, *ast2, *ast3;

    ast = create_AST("translation_unit", 0);
    while (1) {
        switch (lookahead(1)) {
            case TK_KW_INT:
            case TK_KW_LONG:
            case TK_KW_CHAR:
            case TK_KW_VOID:
                // parse_type_specifier などのコードを書く
                ast1 = parse_type_specifier();
                ast2 = parse_declarator();
                switch (lookahead(1)) {
                    case ';':
                        consume_token(';');
                        ast3 = create_AST(";", 0);
                        break;
                    case '{':
                        ast3 = parse_compound_statement();
                        break;
                    default:
                        parse_error();
                        break;
                }
                ast = add_AST(ast, 3, ast1, ast2, ast3);
                break;
            default:
                goto loop_exit;
        }
    }
loop_exit:
    return ast;
}

/* ------------------------------------------------------- */
// 字句解析

static char *map_file(char *filename) {
    struct stat sbuf;
    char *ptr;

    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    fstat(fd, &sbuf);
#if 0
    printf ("file size = %lld\n", sbuf.st_size);
#endif

    ptr = mmap(NULL, sbuf.st_size + 1,  // +1 for '\0'
               PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    ptr[sbuf.st_size] = '\0';
    return ptr;
}

static void my_regcomp(regex_t *regex_p, char *pattern) {
    static char buf[BUFSIZ];
    int ret = regcomp(regex_p, pattern, REG_EXTENDED);

    if (ret != 0) {
        regerror(ret, regex_p, buf, sizeof(buf));
        fprintf(stderr, "regcomp: %s\n", buf);
        exit(1);
    }
}

static int my_regexec(regex_t *regex_p, char *str, regmatch_t *regmatch_p) {
    int ret = regexec(regex_p, str, 1, regmatch_p, 0);
#if 0
    if (ret == 0) {
        fprintf (stderr, "matched: |%lld, %lld: %.40s|\n",
                 regmatch_p->rm_so, regmatch_p->rm_eo, str+regmatch_p->rm_so);
    } else {
        fprintf (stderr, "unmatched\n");
    }
#endif
    return ret;
}

static void *copy_string_region_int(char *s, int start, int end) {
    int size = end - start;
    char *buf = malloc(size + 1);  // +1 for '\0'
    memcpy(buf, s + start, size);
    buf[size] = '\0';
    return buf;
}

static void *copy_string_region_ptr(char *start, char *end) {
    int size = end - start;
    char *buf = malloc(size + 1);  // +1 for '\0'
    memcpy(buf, start, size);
    buf[size] = '\0';
    return buf;
}

static void *copy_string_region_regmatch(char *s, regmatch_t *regmatch_p) {
    return copy_string_region_int(s, regmatch_p->rm_so, regmatch_p->rm_eo);
}

static int skip_block_comment(char *ptr, int off) {
    int i = off;

    assert((ptr[i] == '/') && (ptr[i + 1] == '*'));
    i += 2;
    while ((ptr[i] != '\0') && !(ptr[i] == '*' && ptr[i + 1] == '/')) {
        i++;
    }
    if (ptr[i] != '\0')
        i += 2;

#if 0
    char *buf = copy_string_region_ptr (ptr + off, ptr + i);
    printf ("block_comment(%d-%d)=|%s|\n", off, i, buf);
    set_token_int (ptr, 0, i - off, TK_COMMENT, off);
#endif
    return i;
}

static int skip_whitespaces(char *ptr, int off) {
    int i = off;

    while (ptr[i] != '\0') {
        int c = ptr[i];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            i++;
        } else {
            break;
        }
    }
#if 0
    char *buf = copy_string_region_ptr (ptr + off, ptr + i);
    printf ("whitespaces(%d, %d)=|%s|\n", off, i, buf);
    set_token_int (ptr, 0, i - off, TK_WHITESPACE, off);
#endif
    return i;
}

static int strncmp_r(char *s1, char *s2) {
    return strncmp(s1, s2, strlen(s2));
}

static int check_kind_id(char *lexeme) {
    if (!strcmp(lexeme, "char"))
        return TK_KW_CHAR;
    else if (!strcmp(lexeme, "else"))
        return TK_KW_ELSE;
    else if (!strcmp(lexeme, "goto"))
        return TK_KW_GOTO;
    else if (!strcmp(lexeme, "if"))
        return TK_KW_IF;
    else if (!strcmp(lexeme, "int"))
        return TK_KW_INT;
    else if (!strcmp(lexeme, "long"))
        return TK_KW_LONG;
    else if (!strcmp(lexeme, "return"))
        return TK_KW_RETURN;
    else if (!strcmp(lexeme, "void"))
        return TK_KW_VOID;
    else if (!strcmp(lexeme, "while"))
        return TK_KW_WHILE;
    else
        return TK_ID;
}

static int set_token_int(char *ptr, int begin, int end, int kind, int off) {
    assert(tokens_index < MAX_TOKENS);
    struct token *token_p = &tokens[tokens_index++];
    assert(begin == 0);
    token_p->kind = kind;
    token_p->offset_begin = off + begin;
    token_p->offset_end = off + end;
    token_p->lexeme = copy_string_region_int(ptr + off, begin, end);
#if 0
    printf ("topen_p->lexeme = |%s|\n", token_p->lexeme);
#endif
    return off + end;
}

static int set_token_regmatch(char *ptr, regmatch_t *regmatch_p, int kind, int off) {
    return set_token_int(ptr, regmatch_p->rm_so, regmatch_p->rm_eo, kind, off);
}

static void create_tokens(char *ptr) {
    static char str_id[] = "^[a-zA-Z_][a-zA-Z0-9_]*";
    static char str_int[] = "^(0|[1-9][0-9]*)";
    // regcomp に渡すパターンは ^'(\\n|\\'|\\\\|[^\'])' エスケープすると以下
    static char str_char[] = "^'(\\\\n|\\\\'|\\\\\\\\|[^\\'])'";
    // regcomp に渡すパターンは ^"(\\n|\\"|\\\\|[^\"])*" エスケープすると以下
    static char str_string[] = "^\"(\\\\n|\\\\\"|\\\\\\\\|[^\\\"])*\"";
    static regex_t regex_id, regex_int, regex_char, regex_string;
    static regmatch_t regmatch;

    char *ptr_start = ptr;
    int off = 0;

    my_regcomp(&regex_id, str_id);
    my_regcomp(&regex_int, str_int);
    my_regcomp(&regex_char, str_char);
    my_regcomp(&regex_string, str_string);

    while (ptr[off] != '\0') {
        int ret;

        if ((ptr[off] == '/') && (ptr[off + 1] == '*')) {
            off = skip_block_comment(ptr, off);
            continue;
        } else if (ptr[off] == ' ' || ptr[off] == '\t' || ptr[off] == '\n' || ptr[off] == '\r') {
            off = skip_whitespaces(ptr, off);
            continue;
        }

        ret = my_regexec(&regex_id, ptr + off, &regmatch);
        if (ret == 0) {
            char *lexeme = copy_string_region_regmatch(ptr + off, &regmatch);
            int kind = check_kind_id(lexeme);
            off = set_token_regmatch(ptr, &regmatch, kind, off);
            free(lexeme);
            continue;
        }
        ret = my_regexec(&regex_int, ptr + off, &regmatch);
        if (ret == 0) {
            off = set_token_regmatch(ptr, &regmatch, TK_INT, off);
            continue;
        }
        ret = my_regexec(&regex_char, ptr + off, &regmatch);
        if (ret == 0) {
            off = set_token_regmatch(ptr, &regmatch, TK_CHAR, off);
            continue;
        }
        ret = my_regexec(&regex_string, ptr + off, &regmatch);
        if (ret == 0) {
            off = set_token_regmatch(ptr, &regmatch, TK_STRING, off);
            continue;
        }

        {
            if (!strncmp_r(ptr + off, "=="))
                off = set_token_int(ptr, 0, 2, TK_OP_EQ, off);
            else if (!strncmp_r(ptr + off, "&&"))
                off = set_token_int(ptr, 0, 2, TK_OP_AND, off);
            else if (!strncmp_r(ptr + off, "||"))
                off = set_token_int(ptr, 0, 2, TK_OP_OR, off);
            else {
                switch (ptr[off]) {
                    case ';':
                        off = set_token_int(ptr, 0, 1, ';', off);
                        break;
                    case ':':
                        off = set_token_int(ptr, 0, 1, ':', off);
                        break;
                    case '{':
                        off = set_token_int(ptr, 0, 1, '{', off);
                        break;
                    case '}':
                        off = set_token_int(ptr, 0, 1, '}', off);
                        break;
                    case ',':
                        off = set_token_int(ptr, 0, 1, ',', off);
                        break;
                    case '=':
                        off = set_token_int(ptr, 0, 1, '=', off);
                        break;
                    case '(':
                        off = set_token_int(ptr, 0, 1, '(', off);
                        break;
                    case ')':
                        off = set_token_int(ptr, 0, 1, ')', off);
                        break;
                    case '&':
                        off = set_token_int(ptr, 0, 1, '&', off);
                        break;
                    case '!':
                        off = set_token_int(ptr, 0, 1, '!', off);
                        break;
                    case '-':
                        off = set_token_int(ptr, 0, 1, '-', off);
                        break;
                    case '+':
                        off = set_token_int(ptr, 0, 1, '+', off);
                        break;
                    case '*':
                        off = set_token_int(ptr, 0, 1, '*', off);
                        break;
                    case '/':
                        off = set_token_int(ptr, 0, 1, '/', off);
                        break;
                    case '<':
                        off = set_token_int(ptr, 0, 1, '<', off);
                        break;
                    default:
                        printf("unexpected token (%d): %s\n", off, ptr + off);
                        exit(1);
                }
            }
        }
    }
}

static void dump_tokens() {
    int i;
    for (i = 0; i < MAX_TOKENS; i++) {
        struct token *token_p = &tokens[i];
        if (token_p->kind == TK_UNUSED)
            break;
        printf("%5d: %d-%d: %s (%s)\n", i,
               token_p->offset_begin,
               token_p->offset_end,
               token_p->lexeme,
               token_kind_string[token_p->kind]);
    }
}
/* ------------------------------------------------------- */
static void unparse_error(struct AST *ast) {
    printf("something wrong: %s\n", ast->ast_type);
    exit(1);
}

static void unparse_AST(struct AST *ast, int depth) {
    int i;
    if (!strcmp(ast->ast_type, "translation_unit")) {
        /*
        translation_unit
            : ( type_specifier declarator ( ";" | compound_statement ) )*
        */
        for (i = 0; i < ast->num_child; i++) {
            printf_ns(depth, "");
            unparse_AST(ast->child[i], depth);      // type specifier
            unparse_AST(ast->child[i + 1], depth);  // declarator

            if (!strcmp(ast->child[i + 2]->ast_type, ";")) {  // ";"
                printf(";\n");
            } else if (!strcmp(ast->child[i + 2]->ast_type,
                               "compound_statement")) {  // compound_statement
                printf("\n");
                unparse_AST(ast->child[i + 2], depth);
            } else {
                unparse_error(ast);
            }
            i += 2;
        }
    } else if (!strcmp(ast->ast_type, "compound_statement")) {
        /*
        compound_statement
            : "{" ( type_specifier declarator ";" )* ( statement )* "}"
        */
        // "{"
        printf_ns(depth, "{\n");  // "{" ast->child[0]->ast_type == "{"
        int ast_index = 1;
        // ( type_specifier declarator ";" )*
        while (!strcmp(ast->child[ast_index]->ast_type,
                       "type_specifier")) {                     // type_specifier declarator ";"
            unparse_AST(ast->child[ast_index], depth + 1);      // type_specifier
            unparse_AST(ast->child[ast_index + 1], depth + 1);  // declarator
            printf(";\n");                                      // ";" ast->child[ast_index + 2]->ast_type == ";"
            ast_index += 3;
        }
        // ( statement )*
        while (!strcmp(ast->child[ast_index]->ast_type, "statement")) {
            unparse_AST(ast->child[ast_index], depth + 1);  // statement
            ast_index++;
        }
        // "}"
        printf_ns(depth, "}\n");  // "}" ast->child[n]->ast_type == "}"

    } else if (!strcmp(ast->ast_type, "statement")) {
        /*
        statement
            : IDENTIFIER ":"
            | "if" "(" expression ")" statement ("else" statement)?
            | "while" "(" expression ")" statement
            | "goto" IDENTIFIER ";"
            | "return" (expression)? ";"
            | (expression)? ";"
        */
        if (strcmp(ast->child[0]->ast_type, "TK_ID") == 0 && !strcmp(ast->child[1]->ast_type, ":")) {
            // expression の first と衝突するので
            printf_ns(0, "%s:\n", ast->child[0]->lexeme);
        } else if (!strcmp(ast->child[0]->ast_type, "compound_statement")) {
            unparse_AST(ast->child[0], depth);
        } else if (!strcmp(ast->child[0]->ast_type, "TK_KW_IF")) {
            int is_not_compound_statement_flag = 0;

            printf_ns(depth, "if (");               // ast->child[0], ast->child[1]
            unparse_AST(ast->child[2], depth + 1);  // expression (補足: if ( expression ) の expression )
            if (strcmp(ast->child[4]->child[0]->ast_type, "compound_statement")) {
                // if ( expression ) statement
                // の statement の中身が compound_statement でない場合
                // 次が compound_statement でない場合
                printf_ns(0, ") {\n");  // if 文なので { は必要 ast->child[3]
                is_not_compound_statement_flag = 1;
            } else {
                // compound statement である場合
                printf_ns(0, ") \n");  // compound statement であるので {} の補完はいらない
            }

            unparse_AST(ast->child[4], depth + 1);  // statement
            if (is_not_compound_statement_flag) {
                // compound statement でない場合 は } を補う
                printf_ns(depth, "}\n");  // } if 文の最後の }
            }

            if (ast->num_child == 7) {  // 0-6 より 7個 childがあるので
                int is_elseif_flag = 0;
                if (!strcmp(ast->child[6]->child[0]->ast_type, "TK_KW_IF")) {
                    // else if
                    is_elseif_flag = 1;
                }

                if (is_elseif_flag) {
                    printf_ns(depth, "else ");  // else if
                    unparse_AST(ast->child[6], 0);  // statement
                } else {
                    printf_ns(depth, "else {\n");           // else 文なので { は必要  ast->child[5]
                    unparse_AST(ast->child[6], depth + 1);  // statement ast->child[6]
                    printf_ns(depth, "}\n");                // } else 文の最後の }
                }
            }
        } else if (!strcmp(ast->child[0]->ast_type, "TK_KW_WHILE")) {
            printf_ns(depth, "while (");            // ast->child[0], ast->child[1]
            unparse_AST(ast->child[2], depth + 1);  // expression ast->child[2]

            printf_ns(0, ") \n");                   // ast->child[3] while 文の最初の {
            unparse_AST(ast->child[4], depth + 1);  // statement ast->child[4]
            // printf_ns(depth, "}\n");                // while 文の最後の }
        } else if (!strcmp(ast->child[0]->ast_type, "TK_KW_GOTO")) {
            printf_ns(depth, "goto ");               // ast->child[0]
            printf("%s;\n", ast->child[1]->lexeme);  // ast->child[1] IDENTIFIER

        } else if (!strcmp(ast->child[0]->ast_type, "TK_KW_RETURN")) {
            printf_ns(depth, "return");
            if (ast->num_child > 2) {  // return ; でない場合 -> return expression ;
                print_nspace(1);
                unparse_AST(ast->child[1], depth + 1);
            }
            printf(";\n");  // return 文の最後の ;なので printf_ns でなくてよい
        } else if (!strcmp(ast->child[0]->ast_type, ";")) {
            printf_ns(depth, ";\n");
        } else if (!strcmp(ast->child[0]->ast_type, "expression")) {
            unparse_AST(ast->child[0], depth);  // expression
            printf(";\n");
        } else {
            unparse_error(ast);
        }
    } else if (!strcmp(ast->ast_type, "expression")) {
        /*
        expression
            : primary ( "(" ")" )?
        */
        unparse_AST(ast->child[0], 0);  // primary
        if (ast->num_child == 3) {
            printf("()");
        } else {
            assert(ast->num_child == 1);
        }
    } else if (!strcmp(ast->ast_type, "primary")) {
        /*
        primary
            : INTEGER | CHARACTER | STRING | IDENTIFIER | "(" expression ")"
        */
        if (!strcmp(ast->child[0]->ast_type, "TK_INT")) {
            printf_ns(depth, "%s", ast->child[0]->lexeme);
        } else if (!strcmp(ast->child[0]->ast_type, "TK_CHAR")) {
            printf_ns(depth, "%s", ast->child[0]->lexeme);
        } else if (!strcmp(ast->child[0]->ast_type, "TK_STRING")) {
            printf_ns(depth, "%s", ast->child[0]->lexeme);
        } else if (!strcmp(ast->child[0]->ast_type, "TK_ID")) {
            printf_ns(depth, "%s", ast->child[0]->lexeme);
        } else if (!strcmp(ast->child[0]->ast_type, "(")) {
            printf("( ");                       // ast->child[0]
            unparse_AST(ast->child[1], depth);  // expression ast->child[1]
            printf(" )");                       // ast->child[2]
        } else {
            unparse_error(ast);
        }
    } else if (!strcmp(ast->ast_type, "type_specifier")) {
        /*
        type_specifier
            : "void" | "int" | "char" | "long"
        */
        if (!strcmp(ast->child[0]->ast_type, "TK_KW_VOID")) {
            printf_ns(depth, "void ");
        } else if (!strcmp(ast->child[0]->ast_type, "TK_KW_INT")) {
            printf_ns(depth, "int ");
        } else if (!strcmp(ast->child[0]->ast_type, "TK_KW_CHAR")) {
            printf_ns(depth, "char ");
        } else if (!strcmp(ast->child[0]->ast_type, "TK_KW_LONG")) {
            printf_ns(depth, "long ");
        } else {
            unparse_error(ast);
        }
    } else if (!strcmp(ast->ast_type, "declarator")) {
        /*
        declarator
            : IDENTIFIER
            | IDENTIFIER  "(" ")"
        */
        if (ast->num_child == 1) {
            printf("%s ", ast->child[0]->lexeme);  // ast->child[0] IDENTIFIER
        } else if (ast->num_child == 3) {
            // ast->child[0] IDENTIFIER ast->child[1] "(" ast->child[2] ")"
            printf("%s ", ast->child[0]->lexeme);
            printf("(");
            printf(")");
        } else {
            unparse_error(ast);
        }
    } else {
        unparse_error(ast);
    }
}
/* ------------------------------------------------------- */

int main(int argc, char *argv[]) {
    char *ptr;
    struct AST *ast;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        exit(1);
    }

    ptr = map_file(argv[1]);
    create_tokens(ptr);
    reset_tokens();
    // dump_tokens();  // 提出時はコメントアウトしておくこと
    ast = parse_translation_unit();

    // show_AST(ast, 0);  // 提出時はコメントアウトしておくこと

    unparse_AST(ast, 0);
}
