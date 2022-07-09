# コンパイラ構成 

## XC-small がLL(1)文法でない

LL(1)文法は、講義資料3-parse.pdf 92ページより「（先読み記号が1文字で）うまく変換できる文法クラス」である。
しかし、xcc-small.pdfに 4ページにあるように、statementを `IDENTIFIER ";"`に展開するべきか、 `[ exp ] ";"`に展開するべきかが1トークン先読みでは決定できず、expか IDENTIFIER のどちらかを判別するために先読みをもう一度行う必要が生じる。そのためLL(1)文法の要件を満たしていない。

director(`statement`, `IDENTIFIER ";"`) = first(`IDENTIFIER ";"`) = {`IDENTIFIER`}   
director(`statement`, `[ exp ] ";"`) = first(`[ exp ] ";"`) = {`INTEGER`, `CHARACTER`, `STRING`, `IDENTIFIER, (`}

以上より    
director(`statement`, `IDENTIFIER ";"`) $\cap$ director(`statement`, `[ exp ] ";"`) $\neq \phi$であり、LL(1)文法ではない。

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
= first(`"if" "(" exp ")"  statement "else" statement`) = {`"if"`}

director(`statement`, `"if" "(" exp ")"  "if" "(" exp ")" statement "else" statement`)
= first(`"if" "(" exp ")"  "if" "(" exp ")" statement "else" statement`) = {`"if"`}

となるためLL(1)文法ではない。

## xcc-small 

### 実装方針

### 工夫点

### アピールポイント

### 既知のバグ

### 改善可能点

