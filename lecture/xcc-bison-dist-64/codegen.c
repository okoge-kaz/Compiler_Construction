/* XCC (x86): Experimental C-subset Compiler.
  Copyright (c) 2002-2019, gondow@cs.titech.ac.jp, All rights reserved.
  $Id: codegen.c,v 1.8 2019/05/28 02:42:04 gondow Exp gondow $ */
/* ---------------------------------------------------------------------- */
#include "codegen.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AST.h"
#include "misc.h"
#include "symbol.h"
#include "type.h"
#include "xcc.h"

#define LABEL_PREFIX "L.XCC"
#if 0
// Mac の正式なセクション名はこちら
#define TEXT_SECTION ".section __TEXT,__text,regular,pure_instructions"
#define DATA_SECTION ".section __DATA,__data"
#define RDATA_SECTION ".section __TEXT,__cstring,cstring_literals"
#else
// でも，Linux と共通化したいのでこちらを使う
#define TEXT_SECTION ".text"
#define DATA_SECTION ".data"
#define RDATA_SECTION ".text"
// RDATA_SECTION は Mac と Linux でセクション名が違うので .text で逃げる
#endif
static char *func_name;
static int total_local_size;

static void emit_code(struct AST *ast, char *fmt, ...);
static void codegen_begin_block(struct AST *ast);
static void codegen_end_block(void);
static void codegen_begin_function(struct AST *ast);
static void codegen_end_function(void);
static int is_library_func(char *libname);

static void codegen_exp_id(struct AST *ast);
static void codegen_exp_funcall(struct AST *ast);
static void codegen_exp(struct AST *ast);
static void codegen_stmt(struct AST *ast_stmt);
static void codegen_block(struct AST *ast_block);
static void codegen_func(struct AST *ast);
static void codegen_dec(struct AST *ast);

/* ---------------------------------------------------------------------- */
static char *library_funcs[] = {
    // 必要に応じて呼び出したいライブラリ関数を追加する
    "printf", "malloc", "random", "srandom", "atol", NULL};

/* ---------------------------------------------------------------------- */

static void
emit_code(struct AST *ast, char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(xcc_out, fmt, argp);
    va_end(argp);

    /* the argument 'ast' can be used for debug purpose */
}

static void
codegen_begin_block(struct AST *ast) {
    assert(!strcmp(ast->ast_type, "AST_compound_statement"));
    sym_table.local[++sym_table.local_index] = ast->u.local;
}

static void
codegen_end_block(void) {
    sym_table.local_index--;
}

static void
codegen_begin_function(struct AST *ast) {
    assert(!strcmp(ast->ast_type, "AST_function_definition"));
    sym_table.local_index = -1;
    sym_table.global = ast->u.func.global;
    sym_table.arg = ast->u.func.arg;
    sym_table.label = ast->u.func.label;
    sym_table.string = ast->u.func.string;
}

static void
codegen_end_function(void) {
    /* do nothing */
}

static int
is_library_func(char *libname) {
    for (int i = 0; library_funcs[i] != NULL; i++) {
        if (!strcmp(libname, library_funcs[i]))
            return 1;
    }
    return 0;
}

// ここから上は（関数プロトタイプ宣言の追加等以外は）修正や拡張は不要のはず
/* ---------------------------------------------------------------------- */
// ここから下は好きに修正や拡張をしても構わない

static void codegen_exp_id_address(struct AST *ast) {
    /*
     * 左辺値としてアドレスをスタックに積む
     * 基本構造は、 codegen_exp_id と同じである。
     */
    int offset;
    char *reg = "%reg";
    struct Symbol *sym = sym_lookup(ast->child[0]->u.id);
    assert(sym != NULL);

    switch (sym->name_space) {
        case NS_LOCAL:
            /* local variable */
            offset = -(sym->offset + 8);
            printf("\t# DEBUG NS_LOCAL left value %s\n", sym->name);
            printf("\t# DEBUG NS_LOCAL left value %s\n", sym->type->id);
            emit_code(ast, "\tleaq %d(%%rbp), %%rax\n", offset);
            emit_code(ast, "\tpushq %%rax\n");
            // local 変数の address をスタックに積む
            break;
        case NS_ARG:
            if (sym->name_space == NS_LOCAL) {
                offset = -(sym->offset + 8);
            } else if (sym->name_space == NS_ARG) {
                if (sym->offset <= 40) {  // 1st-6th arguments
                    offset = -(sym->offset + 8);
                    switch (sym->offset) {
                        case 0:
                            reg = "%rdi";
                            break;
                        case 8:
                            reg = "%rsi";
                            break;
                        case 16:
                            reg = "%rdx";
                            break;
                        case 24:
                            reg = "%rcx";
                            break;
                        case 32:
                            reg = "%r8";
                            break;
                        case 40:
                            reg = "%r9";
                            break;
                        default:
                            assert(0);
                            break;
                    }
                } else {  // 7th and subsequent arguments
                    offset = sym->offset - 48 + 16;
                }
            } else {
                assert(0);
            }

            if (!((sym->name_space == NS_ARG) && (sym->offset <= 40))) {  // other than 1st-6th arguments
                // char型，int型には非対応
                emit_code(ast, "\tmovq    %d(%%rbp), %%rax \t# %s, %d\n",
                          offset, sym->name, sym->offset);
            }
            emit_code(ast, "\tpushq   %s\n", reg);
            break;
        case NS_GLOBAL:
            /*
             * グローバルスコープ
             *
             * TYPE_KIND_PRIM: Primitive Type (int, char, ...)
             * TYPE_KIND_FUNCTION: 関数
             * TYPE_KIND_POINTER: ポインタ
             */
            // char型，int型には非対応
            if (sym->type->kind == TYPE_KIND_FUNCTION) {
                if (is_library_func(sym->name)) {
                    // ライブラリ関数
                    emit_code(ast, "\tmovq    _%s@GOTPCREL(%%rip), %%rax\n", sym->name);
                } else {
                    // ユーザー定義関数
                    emit_code(ast, "\tleaq    _%s(%%rip), %%rax\n", sym->name);
                }
                emit_code(ast, "\tpushq   %%rax\n");
            } else {
                // TYPE_KIND_PRIM, TYPE_KIND_POINTER
                emit_code(ast, "\tleaq   _%s(%%rip), %%rax\n", sym->name);
                emit_code(ast, "\tpushq   %%rax\n");
                printf("\t# push global variable's address(left value) %s\n", sym->name);
                printf("\t# DEBUG: %d\n", sym->type->kind);
                // 帯域変数の左辺値 (アドレス) をスタックに積む
            }
            break;
        case NS_LABEL: /* falling through */
        default:
            assert(0);
            break;
    }
}

static void codegen_exp_address(struct AST *ast) {
    printf("\t# codegen_exp_address is called\n");
    printf("\t# codegen_exp_address num_child: %d\n", ast->num_child);
    if (!strcmp(ast->ast_type, "AST_expression_id")) {
        codegen_exp_id_address(ast);
    } else if (ast->num_child == 1) {
        printf("\t# codegen_exp_address: *(%s)\n", ast->child[0]->ast_type);
        if (!strcmp(ast->child[0]->ast_type, "AST_expression_id")) {
            // *(id) のとき
            codegen_exp_id_address(ast->child[0]);
        } else if (!strcmp(ast->child[0]->ast_type, "AST_expression_paren")) {
            // * (()) のとき
            codegen_exp_address(ast->child[0]);
        } else if (!strcmp(ast->child[0]->ast_type, "AST_expression_add")) {
            // *(a + b) のとき
            printf("\t# codegen_exp_address: *(a + b) : a := %s\n", ast->child[0]->child[0]->ast_type);
            printf("\t# codegen_exp_address: *(a + b) : b := %s\n", ast->child[0]->child[1]->ast_type);

            // スタックトップにある 2つのアドレスを取り出して、それらを足し合わせる
            // ただしその際に場合分けが必要
            if (ast->child[0]->child[0]->type->kind == TYPE_KIND_POINTER && ast->child[0]->child[1]->type->kind == TYPE_KIND_PRIM) {
                // *(a + b) のとき
                // a := pointer, b := long
                // rax = rax + ( rdx * 8 )
                codegen_exp_address(ast->child[0]->child[0]);  // a address
                codegen_exp(ast->child[0]->child[1]);          // b アドレスではなく 値をスタックに積む

                emit_code(ast, "\tpopq    %%rdx\n");         // b : rdx (right value)
                emit_code(ast, "\tpopq    %%rax\n");         // a : rax (left value's address)
                emit_code(ast, "\timulq  $8, %%rdx\n");      // rdx *= 8
                emit_code(ast, "\taddq    %%rdx, %%rax\n");  // rax += rdx
                emit_code(ast, "\tpushq   %%rax\n");         // rax (left value's address)

                ast->child[0]->type->kind = TYPE_KIND_POINTER;  // pointer + long :=> pointer
            } else if (ast->child[0]->child[0]->type->kind == TYPE_KIND_PRIM && ast->child[0]->child[1]->type->kind == TYPE_KIND_PRIM) {
                // 通常時と同じ計算になる long + long -> long
                codegen_exp(ast->child[0]->child[0]);// a left value
                codegen_exp(ast->child[0]->child[1]);// b right value

                emit_code(ast, "\tpopq    %%rdx\n");// b : rdx (right value)
                emit_code(ast, "\tpopq    %%rax\n");// a : rax (left value)
                emit_code(ast, "\taddq    %%rdx, %%rax\n");// rax += rdx : left value += right value
                emit_code(ast, "\tpushq   %%rax\n");

                ast->child[0]->type->kind = TYPE_KIND_PRIM;  // long + long :=> long
            } else {
                fprintf(stderr, "codegen_exp_address: *(%d + %d)\n", ast->child[0]->child[0]->type->kind, ast->child[0]->child[1]->type->kind);
                assert(0);
            }

        } else if (!strcmp(ast->child[0]->ast_type, "AST_expression_sub")) {
            // *(a - b) のとき
            printf("\t# codegen_exp_address: *(a - b) : a := %s\n", ast->child[0]->child[0]->ast_type);
            printf("\t# codegen_exp_address: *(a - b) : b := %s\n", ast->child[0]->child[1]->ast_type);

            // スタックトップにある 2つのアドレスを取り出して、それらを引き算する
            // ただしその際に場合分けが必要
            if (ast->child[0]->child[0]->type->kind == TYPE_KIND_POINTER && ast->child[0]->child[1]->type->kind == TYPE_KIND_PRIM) {
                // *(a - b) のとき
                // a := pointer, b := long
                // rax = rax - ( rdx * 8 )
                codegen_exp_address(ast->child[0]->child[0]);  // a address
                codegen_exp(ast->child[0]->child[1]);          // b value

                emit_code(ast, "\tpopq    %%rdx\n");         // b : rdx (right value)
                emit_code(ast, "\tpopq    %%rax\n");         // a : rax (left value's address)
                emit_code(ast, "\timulq  $8, %%rdx\n");      // rdx *= 8
                emit_code(ast, "\tsubq    %%rdx, %%rax\n");  // rax -= rdx
                emit_code(ast, "\tpushq   %%rax\n");         // rax (left value's address)

                ast->child[0]->type->kind = TYPE_KIND_POINTER;  // pointer - long :=> pointer
            } else {
                if (ast->child[0]->child[0]->type->kind == TYPE_KIND_POINTER && ast->child[0]->child[1]->type->kind == TYPE_KIND_POINTER) {
                    // *(a - b) のとき
                    // a := pointer, b := pointer
                    // rax = (rax - rdx) / 8
                    codegen_exp_address(ast->child[0]->child[0]);
                    codegen_exp_address(ast->child[0]->child[1]);

                    emit_code(ast, "\tpopq    %%rdx\n");         // b : rdx (right value's address)
                    emit_code(ast, "\tpopq    %%rax\n");         // a : rax (left value's address)
                    emit_code(ast, "\tsubq    %%rdx, %%rax\n");  // rax -= rdx (pointer address)
                    emit_code(ast, "\tmovq    $8, %%r10\n");     // r10 := 8
                    emit_code(ast, "\tcqto\n");                  // sign extend
                    emit_code(ast, "\tidivq    %%r10\n");        // rax /= 8
                    emit_code(ast, "\tpushq   %%rax\n");         // rax (left value's address)

                    ast->child[0]->type->kind = TYPE_KIND_PRIM;  // pointer - pointer :=> long
                } else if (ast->child[0]->child[0]->type->kind == TYPE_KIND_PRIM && ast->child[0]->child[1]->type->kind == TYPE_KIND_PRIM) {
                    // 通常時と同じ計算になる long - long -> long
                    codegen_exp(ast->child[0]->child[0]);
                    codegen_exp(ast->child[0]->child[1]);

                    emit_code(ast, "\tpopq    %%rdx\n");
                    emit_code(ast, "\tpopq    %%rax\n");
                    emit_code(ast, "\tsubq    %%rdx, %%rax\n");
                    emit_code(ast, "\tpushq   %%rax\n");

                    ast->child[0]->type->kind = TYPE_KIND_PRIM;  // long - long :=> long
                } else {
                    fprintf(stderr, "codegen_exp_address: *(%d - %d)\n", ast->child[0]->child[0]->type->kind, ast->child[0]->child[1]->type->kind);
                    assert(0);
                }
            }

        } else {
            printf("\t# codegen_exp_address: *(%s)\n", ast->child[0]->ast_type);
            assert(0);
        }

    } else {
        printf("\t# codegen_exp_address: num_child%d\n", ast->child[0]->num_child);
        printf("\t# codegen_exp_address: %s\n", ast->child[0]->ast_type);
        assert(0);
    }
}

static void codegen_exp_id(struct AST *ast) {
    int offset;
    char *reg = "%rax";
    struct Symbol *sym = sym_lookup(ast->child[0]->u.id);
    assert(sym != NULL);

    switch (sym->name_space) {
        case NS_LOCAL:
            /* local variable */
            offset = -(sym->offset + 8);
            printf("\t# DEBUG NS_LOCAL right value %s\n", sym->name);
            printf("\t# DEBUG NS_LOCAL right value %s\n", sym->type->id);
            emit_code(ast, "\tpushq  %d(%%rbp)\n", offset);
            // 右辺値 right value 形式なのでメモリの中身をスタックにpushする
            break;
        case NS_ARG:
            if (sym->name_space == NS_LOCAL) {
                offset = -(sym->offset + 8);
            } else if (sym->name_space == NS_ARG) {
                if (sym->offset <= 40) {  // 1st-6th arguments
                    offset = -(sym->offset + 8);
                    switch (sym->offset) {
                        case 0:
                            reg = "%rdi";
                            break;
                        case 8:
                            reg = "%rsi";
                            break;
                        case 16:
                            reg = "%rdx";
                            break;
                        case 24:
                            reg = "%rcx";
                            break;
                        case 32:
                            reg = "%r8";
                            break;
                        case 40:
                            reg = "%r9";
                            break;
                        default:
                            assert(0);
                            break;
                    }
                } else {  // 7th and subsequent arguments
                    offset = sym->offset - 48 + 16;
                }
            } else {
                assert(0);
            }

            if (!((sym->name_space == NS_ARG) && (sym->offset <= 40))) {  // other than 1st-6th arguments
                // char型，int型には非対応
                emit_code(ast, "\tmovq    %d(%%rbp), %%rax \t# %s, %d\n",
                          offset, sym->name, sym->offset);
            }
            emit_code(ast, "\tpushq   %s\n", reg);
            break;
        case NS_GLOBAL:
            /*
             * グローバルスコープ
             *
             * TYPE_KIND_PRIM: Primitive Type (int, char, ...)
             * TYPE_KIND_FUNCTION: 関数
             * TYPE_KIND_POINTER: ポインタ
             */
            // char型，int型には非対応
            if (sym->type->kind == TYPE_KIND_FUNCTION) {
                if (is_library_func(sym->name)) {
                    // ライブラリ関数
                    emit_code(ast, "\tmovq    _%s@GOTPCREL(%%rip), %%rax\n", sym->name);
                } else {
                    // ユーザー定義関数
                    emit_code(ast, "\tleaq    _%s(%%rip), %%rax\n", sym->name);
                }
                emit_code(ast, "\tpushq   %%rax\n");
            } else {
                // TYPE_KIND_PRIM, TYPE_KIND_POINTER
                emit_code(ast, "\tpushq   _%s(%%rip)\n", sym->name);
                printf("\t# push global variable (right value) %s\n", sym->name);
                printf("\t# DEBUG: %d\n", sym->type->kind);
                // 帯域変数の右辺値
            }
            break;
        case NS_LABEL: /* falling through */
        default:
            assert(0);
            break;
    }
}

static void codegen_exp_funcall(struct AST *ast_func) {
    struct AST *ast, *ast_exp;
    int args_size = 0, narg = 0;
    char *regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

    assert(!strcmp(ast_func->ast_type, "AST_expression_funcall1") || !strcmp(ast_func->ast_type, "AST_expression_funcall2"));

    // count "narg" first
    if (!strcmp(ast_func->ast_type, "AST_expression_funcall1")) {
        for (ast = ast_func->child[1], narg = 1;; ast = ast->child[0], narg++) {
            if (!strcmp(ast->ast_type, "AST_argument_expression_list_single"))
                break;
        }
    }

    // save all caller-save registers to the stack
    emit_code(ast_func, "# save caller-save registers\n");
    for (int i = 1; i <= 6; i++) {
        emit_code(ast_func, "\tpushq   %s\n", regs[i - 1]);
    }

    /* push arguments in reverse order (funcall2 has no arguments) */
    if (!strcmp(ast_func->ast_type, "AST_expression_funcall1")) {
        /* for Mac 16-bytes alignment */
        if (narg >= 7 && narg % 2 != 0) {
            emit_code(ast_func, "\tsubq    $8, %%rsp\n");
        }

        for (ast = ast_func->child[1];; ast = ast->child[0]) {
            if (!strcmp(ast->ast_type,
                        "AST_argument_expression_list_single")) {
                ast_exp = ast->child[0];
            } else if (!strcmp(ast->ast_type,
                               "AST_argument_expression_list_pair")) {
                ast_exp = ast->child[1];
            } else {
                assert(0);
            }
            args_size += ROUNDUP_LONG(ast_exp->type->size);
            codegen_exp(ast_exp);
            if (!strcmp(ast->ast_type,
                        "AST_argument_expression_list_single"))
                break;
        }
    }

    codegen_exp(ast_func->child[0]);
    emit_code(ast_func, "\tpopq    %%rax\n");

    // move 1st-6th args to registers
    for (int i = 1; i <= narg && i <= 6; i++) {
        char *regs[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        emit_code(ast_func, "\tpopq    %s\n", regs[i - 1]);
    }
    emit_code(ast_func, "\tcall    *%%rax\n");
    if (narg >= 7) {
        emit_code(ast_func, "\taddq    $%d, %%rsp \t# pop args\n",
                  ROUNDUP_STACK(args_size) - 48);
    }
    // restore caller-save registers from the stack
    emit_code(ast_func, "# restore caller-save registers\n");
    for (int i = 6; i >= 1; i--) {
        emit_code(ast_func, "\tpopq   %s\n", regs[i - 1]);
    }

    emit_code(ast_func, "\tpushq   %%rax\n");
}

static void codegen_exp(struct AST *ast) {
    /*
     *  AST_expression_id : identifier 変数名
     *  AST_expression_int : int
     *  AST_expression_long : long
     *  AST_expression_char : char
     *  AST_expression_string : string (char*)
     *  AST_expression_assign : = 代入
     *  AST_expression_lor : || 論理演算子 OR
     *  AST_expression_land : && 論理演算子 AND
     *  AST_expression_eq : == 比較演算子
     *  AST_expression_less : < 比較演算子
     *  AST_expression_add : + 演算子
     *  AST_expression_sub : - 演算子
     *  AST_expression_mul : * 演算子
     *  AST_expression_div : / 演算子
     *  AST_expression_unary : + - ! 単項演算子
     *  AST_expression_list : 配列 int *
     *  AST_expression_funcall1 : 関数呼び出し1
     *  AST_expression_funcall2 : 関数呼び出し2
     *  AST_expression_paren : 括弧 ()
     *  AST_expression_opt_single : 式文 [expression] ;
     */
    if (!strcmp(ast->ast_type, "AST_expression_id")) {
        /*
         *  AST_expression_id : identifier 変数名
         */
        printf("\t# codegen_exp_id called\n");
        codegen_exp_id(ast);

    } else if (!strcmp(ast->ast_type, "AST_expression_int") ||
               !strcmp(ast->ast_type, "AST_expression_char") ||
               !strcmp(ast->ast_type, "AST_expression_long")) {
        /*
         *  AST_expression_int : int
         *  AST_expression_char : char
         *  AST_expression_long : long
         *  16進数表記 lx で rax ;= ast->u.long_valとする
         */
        emit_code(ast, "\tmovq    $0x%lx, %%rax\n", ast->u.long_val);
        emit_code(ast, "\tpushq   %%rax\n");

    } else if (!strcmp(ast->ast_type, "AST_expression_string")) {
        /*
         *  AST_expression_string : string (char*)
         */

        struct String *string = string_lookup(ast->u.id);
        assert(string != NULL);
        emit_code(ast, "\tleaq    %s.%s(%%rip), %%rax \t# \"%s\"\n", LABEL_PREFIX, string->label, string->data);
        emit_code(ast, "\tpushq   %%rax\n");

    } else if (!strcmp(ast->ast_type, "AST_expression_assign")) {
        /*
         *  AST_expression_assign : = 代入
         *  以下のように 右辺 -> 左辺の順にするのは、スタックに積まれる値の順番を考えてのこと
         */
        printf("\t# assign child 1:%s\n", ast->child[1]->ast_type);
        codegen_exp(ast->child[1]);  // 右辺 代入する値

        if (!strcmp(ast->child[0]->ast_type, "AST_expression_unary")) {
            /*
             * *(address) = () のとき
             * ast->child[0]->child[0] := AST_unary_operator_deref
             * ast->child[0]->child[1] := AST_expression_id
             */
            printf("\t# assign unary operator child[1]:%s\n", ast->child[0]->child[1]->ast_type);
            if (!strcmp(ast->child[0]->child[1]->ast_type, "AST_expression_id")) {
                /*
                 * *( 変数名 address )
                 */
                codegen_exp_id_address(ast->child[0]->child[1]);
            } else if (!strcmp(ast->child[0]->child[1]->ast_type, "AST_expression_paren")) {
                /*
                 * * ( i + j ) : ()の中で演算処理がある
                 *  アドレス計算専用の codegen を呼び出す
                 */
                codegen_exp_address(ast->child[0]->child[1]);

            } else {
                fprintf(stderr, "codegen_exp_assign: unary operator child[1] is not AST_expression_id or AST_expression_paren %s\n", ast->child[0]->child[1]->ast_type);
                assert(0);
            }

        } else {
            printf("\t# assign child 0:%s\n", ast->child[0]->ast_type);
            codegen_exp_id_address(ast->child[0]);  // 左辺 代入先アドレス
        }

        emit_code(ast, "\tpopq    %%rax\n");           // rax := 代入先アドレス
        emit_code(ast, "\tpopq    %%rcx\n");           // rcx := 代入する値
        emit_code(ast, "\tmovq    %%rcx, (%%rax)\n");  // *(rax) := 代入する値

        emit_code(ast, "\tpushq   %%rcx\n");

    } else if (!strcmp(ast->ast_type, "AST_expression_lor") ||
               !strcmp(ast->ast_type, "AST_expression_land")) {
        /*
         *  AST_expression_lor : || 論理演算子 OR
         *  AST_expression_land : && 論理演算子 AND
         */
        static int or_label_id = 0;
        static int and_label_id = 0;

        int local_or_label_id = or_label_id;
        int local_and_label_id = and_label_id;
        or_label_id += 2;
        and_label_id += 2;

        codegen_exp(ast->child[0]);                                  // evaluate left value
        printf("\t# or and child 0:%s\n", ast->child[0]->ast_type);  // left value abstract syntax tree's type

        if (!strcmp(ast->ast_type, "AST_expression_lor")) {
            /*
             *  AST_expression_lor : || 論理演算子 OR
             */
            emit_code(ast, "\tpopq    %%rax\n");  // %rax := left value (0 or 1)
            // if left_value == true -> true
            emit_code(ast, "\tcmpq    $0, %%rax\n");
            emit_code(ast, "\tjne      L_or_%d\n", local_or_label_id);  // L0: %rax != 0 -> %rax == 1 -> true

            // if left_value == false -> calc right value
            codegen_exp(ast->child[1]);  // right value
            printf("\t# or child 1:%s\n", ast->child[1]->ast_type);
            emit_code(ast, "\tpopq    %%rax\n");                           // rax := right value
            emit_code(ast, "\tpushq   %%rax\n");                           // push right value(すでに評価済みなのでpush)
            emit_code(ast, "\tjmp     L_or_%d\n", local_or_label_id + 1);  // L1 left_value == false -> right_value

            emit_code(ast, "L_or_%d:\n", local_or_label_id);      // L0:
            emit_code(ast, "\tpushq   $1\n");                     // push true
            emit_code(ast, "L_or_%d:\n", local_or_label_id + 1);  // L1
        } else {
            /*
             *  AST_expression_land : && 論理演算子 AND
             */
            // if left_value == false -> false
            emit_code(ast, "\tpopq    %%rax\n");  // rax := left value (0 or 1)
            emit_code(ast, "\tcmpq    $0, %%rax\n");
            emit_code(ast, "\tje      L_and_%d\n", local_and_label_id);  // L0: %rax == 0 -> false
            // if left_value == true -> calc right value
            codegen_exp(ast->child[1]);  // right value
            printf("\t# and child 1:%s\n", ast->child[1]->ast_type);
            emit_code(ast, "\tpopq    %%rax\n");                             // rax := right value
            emit_code(ast, "\tpushq   %%rax\n");                             // push right value(すでに評価済みなのでpush)
            emit_code(ast, "\tjmp     L_and_%d\n", local_and_label_id + 1);  // L1 left_value == true -> right_value

            emit_code(ast, "L_and_%d:\n", local_and_label_id);      // L0: left_value == false -> push false
            emit_code(ast, "\tpushq   $0\n");                       // push false
            emit_code(ast, "L_and_%d:\n", local_and_label_id + 1);  // L1
        }

    } else if (!strcmp(ast->ast_type, "AST_expression_eq")) {
        /*
         *  AST_expression_eq : == 比較演算子
         */
        codegen_exp(ast->child[0]);  // left value
        codegen_exp(ast->child[1]);  // right value

        emit_code(ast, "\tpopq    %%rcx\n");  // rcx := right value
        emit_code(ast, "\tpopq    %%rax\n");  // rax := left value

        emit_code(ast, "\tcmpq    %%rax, %%rcx\n");  // rax == rcx
        emit_code(ast, "\tsete    %%al\n");          // al := rax == rcx ? 1 : 0
        emit_code(ast, "\tmovzbq  %%al, %%rax\n");   // rax := rax == rcx ? 1 : 0

        emit_code(ast, "\tpushq   %%rax\n");

    } else if (!strcmp(ast->ast_type, "AST_expression_less") ||
               !strcmp(ast->ast_type, "AST_expression_add") ||
               !strcmp(ast->ast_type, "AST_expression_sub")) {
        /*
         *  AST_expression_less : < 比較演算子
         *  AST_expression_add : + 演算子
         *  AST_expression_sub : - 演算子
         *
         * 方針としてはスタック機械としての振る舞いを実装する。
         * まず 2回 pop する -> 演算ごとに分岐 -> 演算 -> 結果を push する
         */

        if (!strcmp(ast->ast_type, "AST_expression_less")) {
            /*
             *  < 比較演算子
             */
            codegen_exp(ast->child[1]);  // right
            codegen_exp(ast->child[0]);  // left

            emit_code(ast, "\tpopq    %%rax\n");  // rax := left value
            emit_code(ast, "\tpopq    %%rdx\n");  // rdx := right value

            emit_code(ast, "\tcmpq    %%rdx, %%rax\n");  // rax < rdx ? 1 : 0
            emit_code(ast, "\tsetl    %%al\n");          // al := rax < rdx ? 1 : 0
            emit_code(ast, "\tmovzbq  %%al, %%rax\n");   // rax := rax < rdx ? 1 : 0

        } else if (!strcmp(ast->ast_type, "AST_expression_add")) {
            /*
             *  + 演算子
             *  %rax += %rdx
             */
            // if child[0] is long and child[1] is long -> 通常演算
            if (ast->child[0]->type->kind == TYPE_KIND_PRIM && ast->child[1]->type->kind == TYPE_KIND_PRIM) {
                codegen_exp(ast->child[1]);  // right
                codegen_exp(ast->child[0]);  // left

                emit_code(ast, "\tpopq    %%rax\n");  // rax := left value
                emit_code(ast, "\tpopq    %%rdx\n");  // rdx := right value

                emit_code(ast, "\taddq    %%rdx, %%rax\n");
                ast->type->kind = TYPE_KIND_PRIM;
            } else {
                //  unary operator がない箇所でも pointer の演算は行われるため、ここは必要
                // if child[0] is pointer and child[1] is long -> ポインタ演算
                if (ast->child[0]->type->kind == TYPE_KIND_POINTER && ast->child[1]->type->kind == TYPE_KIND_PRIM) {
                    codegen_exp(ast->child[1]);          // right
                    codegen_exp_address(ast->child[0]);  // left

                    emit_code(ast, "\tpopq    %%rax\n");  // rax := left value
                    emit_code(ast, "\tpopq    %%rdx\n");  // rdx := right value

                    // rax = rax + rdx * sizeof(type of rax) (size of rax = 8)
                    emit_code(ast, "\timulq   $8, %%rdx\n");     // rdx *= 8 (size of rax)
                    emit_code(ast, "\taddq    %%rdx, %%rax\n");  // rax += rdx
                    ast->type->kind = TYPE_KIND_POINTER;         // pointer - long :=> pointer

                } else {
                    // if child[0] is long and child[1] is pointer -> コンパイルエラー
                    if (ast->child[0]->type->kind == TYPE_KIND_PRIM && ast->child[1]->type->kind == TYPE_KIND_POINTER) {
                        fprintf(stderr, "Error: cannot add long and pointer\n");
                        exit(1);
                    } else {
                        // if child[0] is pointer and child[1] is pointer -> コンパイルエラー
                        fprintf(stderr, "Error: cannot add pointer and pointer\n");
                        exit(1);
                    }
                }
            }

        } else {
            /*
             *  - 演算子
             */
            // if child[0] is long and child[1] is long -> 通常演算
            if (ast->child[0]->type->kind == TYPE_KIND_PRIM && ast->child[1]->type->kind == TYPE_KIND_PRIM) {
                codegen_exp(ast->child[1]);  // right
                codegen_exp(ast->child[0]);  // left

                emit_code(ast, "\tpopq    %%rax\n");  // rax := left value
                emit_code(ast, "\tpopq    %%rdx\n");  // rdx := right value

                emit_code(ast, "\tsubq    %%rdx, %%rax\n");
                ast->type->kind = TYPE_KIND_PRIM;
            } else {
                //  unary operator がない箇所でも pointer の演算は行われるため、ここは必要
                // if child[0] is pointer and child[1] is long -> ポインタ演算
                if (ast->child[0]->type->kind == TYPE_KIND_POINTER && ast->child[1]->type->kind == TYPE_KIND_PRIM) {
                    codegen_exp(ast->child[1]);          // right
                    codegen_exp_address(ast->child[0]);  // left

                    emit_code(ast, "\tpopq    %%rax\n");  // rax := left value
                    emit_code(ast, "\tpopq    %%rdx\n");  // rdx := right value

                    // rax -= (rdx * sizeof(type of rdx)) (size of rax = 8)
                    emit_code(ast, "\timulq   $8, %%rdx\n");     // rdx *= 8 (size of rax)
                    emit_code(ast, "\tsubq    %%rdx, %%rax\n");  // rax -= rdx
                    ast->type->kind = TYPE_KIND_POINTER;         // pointer - long :=> pointer
                } else {
                    // if child[0] is long and child[1] is pointer -> コンパイルエラー
                    if (ast->child[0]->type->kind == TYPE_KIND_PRIM && ast->child[1]->type->kind == TYPE_KIND_POINTER) {
                        fprintf(stderr, "Error: cannot subtract long and pointer\n");
                        exit(1);
                    } else {
                        // if child[0] is pointer and child[1] is pointer -> ポインタ演算
                        if (ast->child[0]->type->kind == TYPE_KIND_POINTER && ast->child[1]->type->kind == TYPE_KIND_POINTER) {
                            codegen_exp_address(ast->child[1]);          // right
                            codegen_exp_address(ast->child[0]);  // left

                            emit_code(ast, "\tpopq    %%rax\n");  // rax := left value
                            emit_code(ast, "\tpopq    %%rdx\n");  // rdx := right value

                            // rax = (rax - rdx) / sizeof(type of rdx)
                            emit_code(ast, "\tsubq    %%rdx, %%rax\n");  // rax = rax - rdx
                            emit_code(ast, "\tmovq    $8,  %%r10\n");    // r10 = 8
                            emit_code(ast, "\tcqto\n");                  // Sign Extend(R[%rax] -> R[%rdx:%rax])
                            emit_code(ast, "\tidivq   %%r10\n");         // rax = rax / 8
                            ast->type->kind = TYPE_KIND_PRIM;            // pointer - pointer :=> long

                        } else {
                            // コンパイルエラー
                            fprintf(stderr, "Error: cannot subtract pointer and pointer\n");
                            exit(1);
                        }
                    }
                }
            }
        }

        emit_code(ast, "\tpushq   %%rax\n");

    } else if (!strcmp(ast->ast_type, "AST_expression_mul") ||
               !strcmp(ast->ast_type, "AST_expression_div")) {
        /*
         *  AST_expression_mul : * 演算子 乗算
         *  AST_expression_div : / 演算子 除算
         */
        codegen_exp(ast->child[0]);  // left value
        codegen_exp(ast->child[1]);  // right value

        if (!strcmp(ast->ast_type, "AST_expression_mul")) {
            /*
             *  * 演算子
             *  rax *= rdx
             */
            emit_code(ast, "\tpopq    %%rdx\n");  // rdx := right value
            emit_code(ast, "\tpopq    %%rax\n");  // rax := left value

            emit_code(ast, "\timulq   %%rdx, %%rax\n");
            emit_code(ast, "\tpushq   %%rax\n");

        } else if (!strcmp(ast->ast_type, "AST_expression_div")) {
            /*
             *  / 演算子
             *  %rax = (%rdx:%rax) / %rbx
             */
            emit_code(ast, "\tpopq    %%r10\n");  // r10 := right value
            emit_code(ast, "\tpopq    %%rax\n");  // rax := left value
            // sign extend %rax -> %rdx:%rax
            emit_code(ast, "\tcqto\n");  // SignExtend(R[%rax]) -> R[%rdx:%rax]
            emit_code(ast, "\tidivq   %%r10\n");
            emit_code(ast, "\tpushq   %%rax\n");
        }

    } else if (!strcmp(ast->ast_type, "AST_expression_unary")) {
        /*
         *  AST_expression_unary : 単項演算子
         *  AST_unary_operator_deref: *
         *  AST_unary_operator_address: &
         *
         * ast->child[0] : AST_unary_operator_deref, AST_unary_operator_address : 単項演算子の子ノード & or *
         * ast->child[1] : AST_expression_id
         *
         *  ここで処理されるのは 右辺値 の unary operator のみ
         */
        printf("\t# AST_expression_unary: num-child: %d\n", ast->num_child);
        printf("\t# AST_expression_unary: child[0]: %s\n", ast->child[0]->ast_type);
        printf("\t# AST_expression_unary: child[1]: %s\n", ast->child[1]->ast_type);

        if (!strcmp(ast->child[0]->ast_type, "AST_unary_operator_deref")) {
            /*
             *  * 演算子
             *  *( address )
             * 5-codegen.pdf p.47 参照のこと
             */
            if (!strcmp(ast->child[1]->ast_type, "AST_expression_id")) {
                // * ( 変数名 ) の場合 -> 変数名のアドレスを rax に格納するだけでよい
                codegen_exp_id_address(ast->child[1]);

            } else if (!strcmp(ast->child[1]->ast_type, "AST_expression_paren")) {
                // * ( i + j ) のように () で囲まれた演算があるとき
                // アドレス計算専用の codegen を呼び出す
                codegen_exp_address(ast->child[1]);

            } else {
                fprintf(stderr, "Error: unary_operator_deref %s\n", ast->child[1]->ast_type);
                exit(1);
            }
            emit_code(ast, "\tpopq    %%rax\n");           // rax := address
            emit_code(ast, "\tmovq    (%%rax), %%rax\n");  // rax := *(address)
            emit_code(ast, "\tpushq   %%rax\n");

        } else if (!strcmp(ast->child[0]->ast_type, "AST_unary_operator_address")) {
            /*
             *  & 演算子
             */
            codegen_exp_address(ast->child[1]);  // address を stack にpush
        }

    } else if (!strcmp(ast->ast_type, "AST_expression_funcall1") ||
               !strcmp(ast->ast_type, "AST_expression_funcall2")) {
        /*
         *  AST_expression_funcall1 : 関数呼び出し
         *  AST_expression_funcall2 : 関数呼び出し
         */
        codegen_exp_funcall(ast);
    } else if (!strcmp(ast->ast_type, "AST_expression_paren")) {
        /*
         *  AST_expression_paren : 括弧
         */
        printf("\t# AST_expression_paren: num-child: %d\n", ast->num_child);
        printf("\t# AST_expression_paren: child[0]: %s\n", ast->child[0]->ast_type);

        codegen_exp(ast->child[0]);
    } else if (!strcmp(ast->ast_type, "AST_expression_opt_single")) {
        /*
         *  AST_expression_opt_single : [expression] ;
         *  return []; の部分のみがここに分類されるよう。 int i; や i = 0;などはここには分類されない。
         */
        printf("\t# AST_expression_opt_single:\n");
        printf("\t# opt single child num = %d\n", ast->num_child);
        printf("\t# opt single child[0]->ast_type = %s\n", ast->child[0]->ast_type);
        printf("\t# opt single child[0]->u.long_val = %ld\n", ast->child[0]->u.long_val);

        // そのまま処理を他のところに流してしまう方針(特別な処理がいる場合は変更必要)
        codegen_exp(ast->child[0]);

    } else {
        printf("codegen_exp: unknown ast_type: %s\n", ast->ast_type);
        assert(0);
    }
}

static void codegen_stmt(struct AST *ast_stmt) {
    /*
     *  AST_statement_exp : expression_opt
     *  AST_statement_comp : compound statement
     *  AST_statement_if : if (条件) { 処理 }
     *  AST_statement_ifelse : if (条件) { 処理 } else { 処理 }
     *  AST_statement_while : while (条件) { ステートメント }
     *  AST_statement_goto : goto 文
     *  AST_statement_label : goto 文の飛び先 Label
     *  AST_statement_return : return 文
     */
    if (!strcmp(ast_stmt->ast_type, "AST_statement_exp")) {
        /*
         *  AST_statement_exp : expression
         */
        if (!strcmp(ast_stmt->child[0]->ast_type, "AST_expression_opt_single")) {
            codegen_exp(ast_stmt->child[0]->child[0]);
            emit_code(ast_stmt, "\taddq    $8, %%rsp\n");
        } else if (!strcmp(ast_stmt->child[0]->ast_type, "AST_expression_opt_null")) {
            /* nothing to do */
        } else {
            assert(0);
        }
    } else if (!strcmp(ast_stmt->ast_type, "AST_statement_comp")) {
        /*
         *  AST_statement_comp : compound statement
         */
        codegen_block(ast_stmt->child[0]);

    } else if (!strcmp(ast_stmt->ast_type, "AST_statement_if")) {
        /*
         *  AST_statement_if : if (条件) { 処理 }
         *  ラベルの付け方に工夫をしないとネスト構造になった際に static int が更新されてしまい変なことになる。
         */
        static int if_label_id = 0;

        int local_if_label = if_label_id;
        if_label_id++;

        codegen_exp(ast_stmt->child[0]);

        emit_code(ast_stmt, "\tpopq    %%rax\n");
        emit_code(ast_stmt, "\tcmpq    $0, %%rax\n");

        emit_code(ast_stmt, "\tje      L_if_%d\n", local_if_label);
        codegen_stmt(ast_stmt->child[1]);
        emit_code(ast_stmt, "L_if_%d:\n", local_if_label);

    } else if (!strcmp(ast_stmt->ast_type, "AST_statement_ifelse")) {
        /*
         *  AST_statement_ifelse : if (条件) { 処理 } else { 処理 }
         *  ラベルの付け方に工夫をしないとネスト構造になった際に static int が更新されてしまい変なことになる。
         */
        static int if_else_label_id = 0;
        if_else_label_id += 2;

        int local_if_label_id = if_else_label_id - 2;
        int local_else_label_id = if_else_label_id - 1;

        codegen_exp(ast_stmt->child[0]);

        emit_code(ast_stmt, "\tpopq    %%rax\n");
        emit_code(ast_stmt, "\tcmpq    $0, %%rax\n");

        emit_code(ast_stmt, "\tje      L_if_else_%d\n", local_if_label_id);
        codegen_stmt(ast_stmt->child[1]);
        emit_code(ast_stmt, "\tjmp     L_if_else_%d\n", local_else_label_id);
        emit_code(ast_stmt, "L_if_else_%d:\n", local_if_label_id);
        codegen_stmt(ast_stmt->child[2]);
        emit_code(ast_stmt, "L_if_else_%d:\n", local_else_label_id);

    } else if (!strcmp(ast_stmt->ast_type, "AST_statement_while")) {
        /*
         *  AST_statement_while : while (条件) { ステートメント }
         *  ラベルの付け方に工夫をしないとネスト構造になった際に static int が更新されてしまい変なことになる。
         */
        static int while_label_id = 0;
        int local_while_label_id = while_label_id;
        while_label_id += 2;

        emit_code(ast_stmt, "L_while_%d:\n", local_while_label_id);

        // while の条件解釈
        codegen_exp(ast_stmt->child[0]);
        emit_code(ast_stmt, "\tpopq    %%rax\n");

        emit_code(ast_stmt, "\tcmpq    $0, %%rax\n");
        emit_code(ast_stmt, "\tje      L_while_%d\n", local_while_label_id + 1);  // 0 == %rax なら while 文を抜ける

        // while 文のブロック処理
        codegen_stmt(ast_stmt->child[1]);

        emit_code(ast_stmt, "\tjmp     L_while_%d\n", local_while_label_id);
        emit_code(ast_stmt, "L_while_%d:\n", local_while_label_id + 1);

    } else if (!strcmp(ast_stmt->ast_type, "AST_statement_return")) {
        /*
         *  AST_statement_return : return 文
         */
        codegen_exp(ast_stmt->child[0]);

        emit_code(ast_stmt, "\tpopq    %%rax\n");
        emit_code(ast_stmt, "\tpopq    %%rbp\n");
        emit_code(ast_stmt, "\tret\n");

    } else {
        assert(0);
    }
}

static void codegen_block(struct AST *ast_block) {
    struct AST *ast, *ast_stmt_list;
    assert(!strcmp(ast_block->ast_type, "AST_compound_statement"));
    codegen_begin_block(ast_block);

    ast_stmt_list = ast_block->child[1];
    ast = search_AST_bottom(ast_stmt_list, "AST_statement_list_single", NULL);
    while (1) {
        if (!strcmp(ast->ast_type, "AST_statement_list_single"))
            codegen_stmt(ast->child[0]);
        else if (!strcmp(ast->ast_type, "AST_statement_list_pair"))
            codegen_stmt(ast->child[1]);
        else
            assert(0);
        if (ast == ast_stmt_list)
            break;
        ast = ast->parent;
    }
    codegen_end_block();
}

static void codegen_dec(struct AST *ast) {
    /*
     *  AST_declaration : type_specifier declarator ;
     *  変数宣言の処理??
     */
    assert(!strcmp(ast->ast_type, "AST_declaration"));
    if (ast->type->size <= 0)
        return;

    emit_code(ast, "\t.globl  _%s\n", ast->type->id);
    emit_code(ast, "\t %s\n", DATA_SECTION);
    // char型，int型には非対応
    if (ast->type->size == 8) {
        emit_code(ast, "\t.p2align  3\n");
    }
    emit_code(ast, "_%s:\n", ast->type->id);
    emit_code(ast, "\t.skip   %d\n\n", ast->type->size);
}

static void codegen_func(struct AST *ast) {
    struct String *string, *head;

    assert(!strcmp(ast->ast_type, "AST_function_definition"));

    codegen_begin_function(ast);

    /* string literals */
    head = sym_table.string;
    if (head != NULL) {
        emit_code(ast, "\t%s\n", RDATA_SECTION);
        for (string = head; string != NULL; string = string->next) {
            emit_code(ast, "%s.%s:\n", LABEL_PREFIX, string->label);
            emit_code(ast, "\t.ascii  \"%s\\0\"\n", string->data);
        }
    }

    func_name = ast->type->id;
    total_local_size = ast->u.func.total_local_size;

    emit_code(ast, "\t%s\n", TEXT_SECTION);
    emit_code(ast, "\t.globl  _%s\n", func_name);
    emit_code(ast, "\t.p2align 4, 0x90\n");
    emit_code(ast, "_%s:\n", func_name);
    emit_code(ast, "\tpushq   %%rbp\n");
    emit_code(ast, "\tmovq    %%rsp, %%rbp\n");

    /* allocate space for local variables */
    emit_code(ast, "\tsubq    $%d, %%rsp\n", total_local_size);

    /* function body */
    codegen_block(ast->child[2]);

    /* function epilogue */
    emit_code(ast, "%s.RE.%s:\n", LABEL_PREFIX, func_name);
    emit_code(ast, "\tmovq    %%rbp, %%rsp\n");
    emit_code(ast, "\tpopq    %%rbp\n");
    emit_code(ast, "\tretq\n");

    codegen_end_function();
}
/* ---------------------------------------------------------------------- */
void codegen(void) {
    struct AST *ast, *ast_ext;
    ast = search_AST_bottom(ast_root, "AST_translation_unit_single", NULL);

    /*
        search_AST_bottom は、抽象構文木の最下部にある AST を返す。
        そして以下の while ループにより、 ast = ast->parent で徐々に上に上がっていきながらコード生成を行う。

        ast == ast_root では、抽象構文木の最上部にある AST にたどり着いたのでこれ以上処理を行わないことを表している。
    */
    while (1) {
        if (!strcmp(ast->ast_type, "AST_translation_unit_single"))
            ast_ext = ast->child[0];
        else if (!strcmp(ast->ast_type, "AST_translation_unit_pair"))
            ast_ext = ast->child[1];
        else
            assert(0);

        if (!strcmp(ast_ext->ast_type, "AST_external_declaration_func"))
            codegen_func(ast_ext->child[0]);
        else if (!strcmp(ast_ext->ast_type, "AST_external_declaration_dec"))
            codegen_dec(ast_ext->child[0]);
        else
            assert(0);

        if (ast == ast_root)
            break;
        ast = ast->parent;
    }
}
/* ---------------------------------------------------------------------- */
