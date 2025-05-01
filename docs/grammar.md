$$
\begin{align}
[\text{prog}] &\to [\text{stmt}]^*
\\
[\text{stmt}] &\to
\begin{cases}
exit([\text{expr}]);
\\
let\space \text{ident} = [\text{expr}]; \\
\text{if}([\text{Expr}]) \space [\text{Scope}] \\
[\text{Scope}]
\end{cases}
\\
[\text{Scope}] &\to \{[\text{Stmt}]^*\} 
\\
[\text{expr}] &\to 
\begin{cases}
\text{[Term]}\\
\text{[BinExpr]}
\end{cases}
\\
[\text{BinExpr}] &\to
\begin{cases}
[\text{Expr}] - [\text{Expr}] &\text{prec} = 0\\
[\text{Expr}] + [\text{Expr}] &\text{prec} = 1\\
[\text{Expr}] * [\text{Expr}] &\text{prec} = 2\\
[\text{Expr}] / [\text{Expr}] &\text{prec} = 3\\
\end{cases}
\\
[\text{Term}] &\to
\begin{cases}
\text{int\_lit} \\
\text{ident} \\
([\text{Expr}])
\end{cases}
\end{align}
$$