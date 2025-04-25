#pragma once
#include<variant>
#include "tokenization.hpp"
using namespace std;

struct NodeExpIntLit{
    Token int_lit;
};

struct NodeExpIdent{
    Token ident;
};

struct NodeExpr{
    variant<NodeExpIntLit,NodeExpIdent> var;
};

struct NodeStmtExit{
    NodeExpr expr;
};

struct NodeStmtLet{
    Token ident;
    NodeExpr expr;
};

struct Nodestmt{
    variant<NodeStmtExit,NodeStmtLet> var;
};

struct NodeProg{
    vector<Nodestmt> stmts;
};

class Parser
{
public:
    inline explicit Parser(vector<Token> tokens) : m_tokens(move(tokens))
    {
    }

    optional<NodeExpr> parse_expr(){
        if(peek().has_value() && peek().value().type==TokenType::int_lit){
            return NodeExpr{.var=NodeExpIntLit{.int_lit=consume()}};
        }
        else if(peek().has_value() && peek().value().type==TokenType::ident){
            return NodeExpr{.var=NodeExpIdent{.ident=consume()}};
        }
        else{
            return {};
        }
    }

    optional<Nodestmt> parse_stmt(){
        if(peek().value().type==TokenType::exit && peek(1).has_value() && peek(1).value().type==TokenType::open_paren){
            consume();
            consume();
            NodeStmtExit stmt_exit;
            if(auto node_expr=parse_expr()){
                stmt_exit={.expr=node_expr.value()};
            }else{
                cerr<<"Error parsing expression"<<endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value() && peek().value().type==TokenType::close_paren){
                consume();
            }else{
                cerr<<"Expected ')'after expression"<<endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value() && peek().value().type==TokenType::semi){
                consume();
            }else{
                cerr<<"Expected ';' after expression"<<endl;
                exit(EXIT_FAILURE);
            }
            return Nodestmt{.var=stmt_exit};
        }else if(peek().has_value() && peek().value().type==TokenType::let && peek(1).has_value() && peek(1).value().type==TokenType::ident && peek(2).has_value() && peek(2).value().type==TokenType::eq){
            consume();
            auto stmt_let = NodeStmtLet{.ident=consume()};
            consume();
            if(auto expr=parse_expr()){
                stmt_let.expr=expr.value();
            }else{
                cerr<<"Error parsing expression"<<endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value() && peek().value().type==TokenType::semi){
                consume();
            }else{
                cerr<<"Expected ';' after expression"<<endl;
                exit(EXIT_FAILURE);
            }
            return Nodestmt{.var=stmt_let};
        }else{
            return {};
        }
    }

    optional<NodeProg> parse_prog(){
        NodeProg prog;
        while(peek().has_value()){
            if(auto stmt = parse_stmt()){
                prog.stmts.push_back(stmt.value());
            }else{
                cerr<<"Error parsing statement"<<endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }

private:
    [[nodiscard]] inline optional<Token> peek(int offset=0) const
    {
        if (m_index + offset >= m_tokens.size())
        {
            return {};
        }
        else
        {
            return m_tokens[m_index + offset];
        }
    }

    inline Token consume()
    {
        return m_tokens.at(m_index++);
    }

    const vector<Token> m_tokens;
    size_t m_index=0;
};