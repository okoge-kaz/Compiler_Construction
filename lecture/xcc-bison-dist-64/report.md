# コンパイラ構成 xcc-codegen 
20B30790 藤井一喜

## はじめに

5-codegen.pdf の内容に沿った実装を行なっており、かつ実装過程でコメントを多数残しているため、ここでは詳細な実装方針には立ち入らず概観のみを説明する。

##  到達度
レベル2 まで完了  
レベル3 の途中(以下のような出力結果となってしまう)   
Bad Accessにはならないため、アドレス計算のどこが間違っているのかがわからず、修正を断念した。

```
sh test_3.sh kadai3
105553135419392
1
12973320048
8601089326
0
0
0
0
8601592736
1107296256
8601089427
8601510256
12973319920
8601067520
8601067520
8601592736
1073741824
8601089517
8601510304
8601067520
```


## 実装

### 自作関数

- codegen_exp_id_address   
AST_expression_id が左辺値である際に address を計算するための関数   
assign等でスタックにaddressを積みたいときに使用する。

- codegen_exp_address   
*(data + i) のように場合について address を計算するための関数   
pointer + long の演算、 pointer - pointer の演算などについても場合分けを行うことで、適切に計算がなされるように実装した。   
上記の関数とは異なり、AST_expression_id ではない場合においても () 内部の計算を適切に行なったのち address をスタックに積むことができる。

### 定義済み関数

- codegen_exp   
5-codegen.pdfを参考に、 assign, or, and, equal, less, add, sub, mul, div, unaryに対応する箇所を追加した。   
and, or については、左側の要素を評価し、その結果だけで値が定まる場合は、右側を評価しなくてもよいように実装した。

- codegen_stmt   
5-codegen.pdfを参考に if 文, if-else文, while文, return文に対応する箇所を追加した。


## アピール

- emit_code だけでは何をしたいのかが分かりにくいため、これを改善するためにコメントを多用し、どのような処理を意図しているのかが分かりやすくした。
- gcc kadaix.c をコンパイルして ./a.outで実行した結果と、gcc kadaix.s の後、./a.out した結果が同じかどうかをテストするtest.sh, test_3.shを作成した。make testx で実行できる。
- デバッグを行いやすくするために、アセンブリコードにコメントでどのような順番で処理が進んだかを表示するようにした。（最終的には消去するべきであるが、今回はレベル3の実装が間に合わなかったため、デバッグ用のコメントも出力される形で提出している。）


## 自作テストの結果

```
❯ make test0
make
make[1]: Nothing to be done for `all'.
sh test.sh kadai0
hello, world, 10, 20
test/kadai0.c:2:5: warning: incompatible redeclaration of library function 'printf' [-Wincompatible-library-redeclaration]
int printf ();
    ^
test/kadai0.c:2:5: note: 'printf' is a builtin with type 'int (const char *, ...)'
1 warning generated.
✅ : PASSED
make clean
rm -f a.out *.o lex.yy.c xcc.tab.c xcc.tab.h xcc.output xcc *~ \#*\#
rm -rf xcc.dSYM
❯ make test1
make
bison -d -v xcc.y
flex xcc.l
gcc -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -g -o xcc AST.c type.c symbol.c misc.c codegen.c xcc.tab.c lex.yy.c  
symbol.c:42:1: warning: unused function 'sym_free' [-Wunused-function]
sym_free(struct Symbol *sym) {
^
1 warning generated.
lex.yy.c:1371:16: warning: function 'input' is not needed and will not be emitted [-Wunneeded-internal-declaration]
    static int input  (void)
               ^
1 warning generated.
sh test.sh kadai1
i = 5
i = 4
i = 3
i = 2
i = 1
sum = 15
test/kadai1.c:1:5: warning: incompatible redeclaration of library function 'printf' [-Wincompatible-library-redeclaration]
int printf ();
    ^
test/kadai1.c:1:5: note: 'printf' is a builtin with type 'int (const char *, ...)'
1 warning generated.
✅ : PASSED
make clean
rm -f a.out *.o lex.yy.c xcc.tab.c xcc.tab.h xcc.output xcc *~ \#*\#
rm -rf xcc.dSYM
❯ make test2
make
bison -d -v xcc.y
flex xcc.l
gcc -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -g -o xcc AST.c type.c symbol.c misc.c codegen.c xcc.tab.c lex.yy.c  
symbol.c:42:1: warning: unused function 'sym_free' [-Wunused-function]
sym_free(struct Symbol *sym) {
^
1 warning generated.
lex.yy.c:1371:16: warning: function 'input' is not needed and will not be emitted [-Wunneeded-internal-declaration]
    static int input  (void)
               ^
1 warning generated.
sh test.sh kadai2
mrn(0) = 0
mrn(1) = 1
mrn(2) = 12
mrn(3) = 123
mrn(4) = 1234
mrn(5) = 12345
mrn(6) = 123456
mrn(7) = 1234567
mrn(8) = 12345678
mrn(9) = 123456789
mrn(10) = 1234567900
test/kadai2.c:1:5: warning: incompatible redeclaration of library function 'printf' [-Wincompatible-library-redeclaration]
int printf();
    ^
test/kadai2.c:1:5: note: 'printf' is a builtin with type 'int (const char *, ...)'
1 warning generated.
✅ : PASSED
make clean
rm -f a.out *.o lex.yy.c xcc.tab.c xcc.tab.h xcc.output xcc *~ \#*\#
rm -rf xcc.dSYM
❯ make test2
make
bison -d -v xcc.y
flex xcc.l
gcc -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -g -o xcc AST.c type.c symbol.c misc.c codegen.c xcc.tab.c lex.yy.c  
symbol.c:42:1: warning: unused function 'sym_free' [-Wunused-function]
sym_free(struct Symbol *sym) {
^
1 warning generated.
lex.yy.c:1371:16: warning: function 'input' is not needed and will not be emitted [-Wunneeded-internal-declaration]
    static int input  (void)
               ^
1 warning generated.
sh test.sh kadai2
mrn(0) = 0
mrn(1) = 1
mrn(2) = 12
mrn(3) = 123
mrn(4) = 1234
mrn(5) = 12345
mrn(6) = 123456
mrn(7) = 1234567
mrn(8) = 12345678
mrn(9) = 123456789
mrn(10) = 1234567900
test/kadai2.c:1:5: warning: incompatible redeclaration of library function 'printf' [-Wincompatible-library-redeclaration]
int printf();
    ^
test/kadai2.c:1:5: note: 'printf' is a builtin with type 'int (const char *, ...)'
1 warning generated.
✅ : PASSED
make clean
rm -f a.out *.o lex.yy.c xcc.tab.c xcc.tab.h xcc.output xcc *~ \#*\#
rm -rf xcc.dSYM
❯ make test2-easy
make
bison -d -v xcc.y
flex xcc.l
gcc -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -g -o xcc AST.c type.c symbol.c misc.c codegen.c xcc.tab.c lex.yy.c  
symbol.c:42:1: warning: unused function 'sym_free' [-Wunused-function]
sym_free(struct Symbol *sym) {
^
1 warning generated.
lex.yy.c:1371:16: warning: function 'input' is not needed and will not be emitted [-Wunneeded-internal-declaration]
    static int input  (void)
               ^
1 warning generated.
sh test.sh kadai2-easy
0 * 0 == 0
else
1 * 1 == 1
else
i == 2
2 / 2 == 1
else
2 < 3 < 8
2 < 4 < 8
2 < 5 < 8
2 < 6 < 8
2 < 7 < 8
else
else
else
test: 11
i: 12
test/kadai2-easy.c:1:5: warning: incompatible redeclaration of library function 'printf' [-Wincompatible-library-redeclaration]
int printf();
    ^
test/kadai2-easy.c:1:5: note: 'printf' is a builtin with type 'int (const char *, ...)'
1 warning generated.
✅ : PASSED
make clean
rm -f a.out *.o lex.yy.c xcc.tab.c xcc.tab.h xcc.output xcc *~ \#*\#
rm -rf xcc.dSYM
❯ make test3
make
bison -d -v xcc.y
flex xcc.l
gcc -Wall -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -g -o xcc AST.c type.c symbol.c misc.c codegen.c xcc.tab.c lex.yy.c  
symbol.c:42:1: warning: unused function 'sym_free' [-Wunused-function]
sym_free(struct Symbol *sym) {
^
1 warning generated.
lex.yy.c:1371:16: warning: function 'input' is not needed and will not be emitted [-Wunneeded-internal-declaration]
    static int input  (void)
               ^
1 warning generated.
sh test_3.sh kadai3
105553154473984
1
13041780592
8635475246
0
0
0
0
8635978656
1107296256
8635475347
8635896176
13041780464
8635453440
8635453440
8635978656
1073741824
8635475437
8635896224
8635453440
test/kadai3.c:1:7: warning: incompatible redeclaration of library function 'printf' [-Wincompatible-library-redeclaration]
int   printf ();
      ^
test/kadai3.c:1:7: note: 'printf' is a builtin with type 'int (const char *, ...)'
1 warning generated.
❌ : FAILED
make clean
rm -f a.out *.o lex.yy.c xcc.tab.c xcc.tab.h xcc.output xcc *~ \#*\#
rm -rf xcc.dSYM
```