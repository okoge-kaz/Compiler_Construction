# コンパイラ構成

課題を解く中で学んだことや、実装方針用のメモなどを保存するための雑記帳です。

## xcc-small 課題

### 用語解説

- first($\alpha$)集合   
非終端記号$\alpha$を展開していった際に、一番最初に左側に出現する終端記号の集合（展開方法を拘束しない場合は、かなり多くの終端記号が考えられる。）

- follow($A$)集合   
非終端記号$A$に右側に出現する終端記号の集合 (A自体を展開するわけではなく、開始記号からA以外の記号を展開していった際にAのすぐ右側に出現する終端記号の集合)

- director ($A, \alpha$) 集合   
Aを$\alpha$に展開すべきかを指示する終端記号の集合

- LL(1)文法   
詳細は講義資料 3-parse.pdf 106ページ、簡単な理解としては   
先読み記号を見れば、Aをどの生成規則で展開すべきかが一意に決まる


director(`statement`, `"return" exp ";"`) 
= first(`"return" exp ";"`) = {`"return"`}

director(`statement`, `"return" ";"`)
= first(`"return" ";"`) = {`"return"`}

これが成立するからといってLL(1)文法ではないとは言えない。

## xcc-codegen 課題