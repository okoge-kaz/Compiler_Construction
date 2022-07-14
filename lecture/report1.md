# コンパイラ構成

## XC-small が LL(1)文法でない

LL(1)文法は、講義資料 3-parse.pdf 92 ページより「（先読み記号が 1 文字で）うまく変換できる文法クラス」である。
しかし、xcc-small.pdf に 4 ページにあるように、statement を `IDENTIFIER ":"`に展開するべきか、 `[ exp ] ";"`に展開するべきかが 1 トークン先読みでは決定できず、exp か IDENTIFIER のどちらかを判別するために先読みをもう一度行う必要が生じる。そのため LL(1)文法の要件を満たしていない。

director(`statement`, `IDENTIFIER ":"`) = first(`IDENTIFIER ":"`) = {`IDENTIFIER`}  
director(`statement`, `[ exp ] ";"`) = first(`[ exp ] ";"`) = {`INTEGER`, `CHARACTER`, `STRING`, `IDENTIFIER, (`}

以上より  
director(`statement`, `IDENTIFIER ":"`) $\cap$ director(`statement`, `[ exp ] ";"`) $\neq \phi$であり、LL(1)文法ではない。

さらに

```
statement :
    (略)
    | "if" "(" exp ")"  statement ["else" statement]
    (略)
```

についても LL(1)文法ではない。

なぜならば

**ここに関しては要検証**書き方がまずいと思われる。

director(`statement`, `"if" "(" exp ")" statement "else" statement`)  
= first(`"if" "(" exp ")" statement "else" statement`) = {`"if"`}

director(`statement`, `"if" "(" exp ")" "if" "(" exp ")" statement "else" statement`)
= first(`"if" "(" exp ")" "if" "(" exp ")" statement "else" statement`) = {`"if"`}

となるため LL(1)文法ではない。

## xcc-small

### 実装方針

### 工夫点

### アピールポイント

### 既知のバグ

なし

### 改善可能点

```c
if (exp) {
    statement;
} else {
    statement;
}
```

のようなネスト構造に改善することが可能である。

具体的には、 compound_statement "{" を出力する前の `if () `の時点で次の `statement->child[0]`が compound_statement であるならば、`if () `の後改行しないという方法が考えられる。同様に、else 文についても考えられるが、私の実装方針であると、この方法を愚直に実装すると、compound statement の `printf_ns(depth, )`の問題が生じるため、時間的な問題から改善を断念した。

補足:   
前述の `printf_ns(depth, )` の問題とは、 compound_statement の開始の `{`と終わりの `}`で`printf_ns(depth, )`に渡す、depth の値が異なり、場合分けが煩雑になるという問題である。
