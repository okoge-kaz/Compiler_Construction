struct AST {
    char        *ast_type;   // 生成規則を区別
    struct AST	*parent;     // 親へのバックポインタ
    int	       	nth;         // 自分が何番目の強大化
    int         num_child;   // 子ノードの数
    struct AST  **child;     // 子ノードポインタの配列
    char        *lexeme;     // 葉ノード用
};
