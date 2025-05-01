#pragma once
#include <variant>
#include<cassert>

#include "./arena.hpp"
#include "tokenization.hpp"
using namespace std;

struct NodeTermIntLit
{
    Token int_lit;
};

struct NodeTermIdent
{
    Token ident;
};

struct NodeExpr;

struct NodeTermParen{
    NodeExpr* expr;
};


struct NodeBinExprAdd
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprMulti
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprSub
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprDiv
{
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExpr
{
    variant<NodeBinExprAdd *, NodeBinExprMulti *, NodeBinExprDiv*, NodeBinExprSub*> var;
};

struct NodeTerm
{
    variant<NodeTermIntLit *, NodeTermIdent *,NodeTermParen*> var;
};

struct NodeExpr
{
    variant<NodeTerm *, NodeBinExpr *> var;
};

struct NodeStmtExit
{
    NodeExpr *expr;
};

struct NodeStmtLet
{
    Token ident;
    NodeExpr *expr;
};

struct NodeStmt;

struct NodeScope{
    vector<NodeStmt*> stmts;
};

struct NodeStmtIf{
    NodeExpr* expr;
    NodeScope* scope;
};

struct NodeStmt
{
    variant<NodeStmtExit *, NodeStmtLet *, NodeScope*, NodeStmtIf*> var;
};

struct NodeProg
{
    vector<NodeStmt *> stmts;
};

class Parser
{
public:
    inline explicit Parser(vector<Token> tokens) : m_tokens(move(tokens)), m_allocator(1024 * 1024 * 4)
    {
    }

    optional<NodeTerm *> parse_term()
    {
        if(auto int_lit=try_consume(TokenType::int_lit)){
            auto term_int_lit = m_allocator.alloc<NodeTermIntLit>();
            term_int_lit->int_lit = int_lit.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_int_lit;
            return term;
        }
        else if (auto ident = try_consume(TokenType::ident)){
            auto term_ident = m_allocator.alloc<NodeTermIdent>();
            term_ident->ident = ident.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_ident;
            return term;
        }
        else if(auto open_paren = try_consume(TokenType::open_paren)){
            auto expr = parse_expr();
            if(!expr.has_value()){
                cerr<<"Expected expression"<<endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected ')'");
            auto term_paren = m_allocator.alloc<NodeTermParen>();
            term_paren->expr = expr.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_paren;
            return term;
        }
        else
        {
            return {};
        }
    }

    optional<NodeExpr *> parse_expr(int min_prec = 0)
    {
        optional<NodeTerm*> term_lhs = parse_term();
        if(!term_lhs.has_value()){
            return {};
        }
        auto expr_lhs = m_allocator.alloc<NodeExpr>();
        expr_lhs->var = term_lhs.value();
        while(true){
            optional<Token> curr_token = peek();
            optional<int> prec;
            if(curr_token.has_value()){
                prec = bin_prec(curr_token->type);
                if(!prec.has_value() || prec<min_prec){
                    break;
                }
            }else{
                break;
            }
            Token op = consume();
            int next_min_prec = prec.value() +1;
            auto expr_rhs = parse_expr(next_min_prec);
            if(!expr_rhs.has_value()){
                cerr<<"Unable to parse Expression"<<endl;
                exit(EXIT_FAILURE);
            }
            auto expr = m_allocator.alloc<NodeBinExpr>();
            auto expr_lhs2 = m_allocator.alloc<NodeExpr>();
            expr_lhs2->var = expr_lhs->var;
            if(op.type == TokenType::plus){
                auto add = m_allocator.alloc<NodeBinExprAdd>();
                add->lhs = expr_lhs2;
                add->rhs = expr_rhs.value();
                expr->var = add;
            }else if(op.type == TokenType::star){
                auto multi = m_allocator.alloc<NodeBinExprMulti>();
                multi->lhs = expr_lhs2;
                multi->rhs = expr_rhs.value();
                expr->var = multi;
            }else if(op.type == TokenType::minus){
                auto sub = m_allocator.alloc<NodeBinExprSub>();
                sub->lhs = expr_lhs2;
                sub->rhs = expr_rhs.value();
                expr->var = sub;
            }else if(op.type == TokenType::fslash){
                auto div = m_allocator.alloc<NodeBinExprDiv>();
                div->lhs = expr_lhs2;
                div->rhs = expr_rhs.value();
                expr->var = div;
            }else{
                assert(false && "Unknown operator");
            }
            expr_lhs->var = expr;
        }
        return expr_lhs;
    }

    optional<NodeScope*> parse_scope(){
        if(!try_consume(TokenType::open_curly).has_value()){
            return {};
        }
        auto stmt_scope = m_allocator.alloc<NodeScope>();
        while(peek().has_value() && peek().value().type != TokenType::close_curly){
            if(auto stmt = parse_stmt()){
                stmt_scope->stmts.push_back(stmt.value());
            }else{
                cerr<<"Error parsing statement"<<endl;
                exit(EXIT_FAILURE);
            }
        }
        try_consume(TokenType::close_curly, "Expected '}' after scope" );
        return stmt_scope;
    }

    optional<NodeStmt *> parse_stmt()
    {
        if (peek().has_value() && peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren)
        {
            consume();
            consume();
            auto stmt_exit = m_allocator.alloc<NodeStmtExit>();
            if (auto node_expr = parse_expr())
            {
                stmt_exit->expr = node_expr.value();
            }
            else
            {
                cerr << "Error parsing expression" << endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected 'param' after expression" );
            try_consume(TokenType::semi, "Expected stmt 1 ';' after expression" );
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_exit;
            return stmt;
        }
        else if (peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value() && peek(1).value().type == TokenType::ident && peek(2).has_value() && peek(2).value().type == TokenType::eq)
        {
            consume();
            auto stmt_let = m_allocator.alloc<NodeStmtLet>();
            stmt_let->ident = try_consume(TokenType::ident, "Expected identifier after 'let'");
            try_consume(TokenType::eq, "Expected '=' after identifier");
            if (auto expr = parse_expr())
            {
                stmt_let->expr = expr.value();
            }
            else
            {
                cerr << "Error parsing expression" << endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::semi, "Expected stmt ';' after expression" );
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
        }
        else if(peek().has_value() && peek().value().type == TokenType::open_curly){
            if(auto scope = parse_scope()){
                auto stmt = m_allocator.alloc<NodeStmt>();
                stmt->var = scope.value();
                return stmt;
            }else{
                cerr<<"Error parsing scope"<<endl;
                exit(EXIT_FAILURE);
            }
        }
        else if(auto if_ = try_consume(TokenType::if_)){
            try_consume(TokenType::open_paren, "Expected '(' after 'if'");
            auto stmt_if = m_allocator.alloc<NodeStmtIf>();
            if(auto expr = parse_expr()){
                stmt_if->expr = expr.value();
            }else{
                cerr<<"Error parsing expression"<<endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected ')' after expression" );
            if(auto scope = parse_scope()){
                stmt_if->scope = scope.value();
            }else{
                cerr<<"Error parsing scope"<<endl;
                exit(EXIT_FAILURE);
            }
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_if;
            return stmt;
        }
        else
        {
            cerr<<"Error parsing statement" << endl;
            return {};
        }
    }

    optional<NodeProg> parse_prog()
    {
        NodeProg prog;
        while (peek().has_value())
        {
            if (auto stmt = parse_stmt())
            {
                prog.stmts.push_back(stmt.value());
            }
            else
            {
                cerr << "Error parsing statement" << endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }

private:
    [[nodiscard]] inline optional<Token> peek(int offset = 0) const
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

    inline Token try_consume(TokenType type, const string& err_msg){
        if (peek().has_value() && peek().value().type == type){
            return consume();
        }else{
            cerr << err_msg<< endl;
            exit(EXIT_FAILURE);
        }
    }

    inline optional<Token> try_consume(TokenType type){
        if (peek().has_value() && peek().value().type == type){
            return consume();
        }else{
            return {};
        }
    }

    const vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;
};