#pragma once
#include <iostream>
#include <string>
#include <optional>
#include <vector>
using namespace std;

enum class TokenType
{
    exit,
    int_lit,
    semi,
    open_paren,
    close_paren,
    ident,
    let,
    eq,
    plus,
    star,
    minus,
    fslash,
    open_curly,
    close_curly,
    if_
};

optional<int> bin_prec(TokenType type){
    switch(type){
        case TokenType::minus:
        return 0;
        case TokenType::plus:
        return 1;
        case TokenType::star:
        return 2;
        case TokenType::fslash:
        return 3;
        default:
        return {};
    }
}

struct Token
{
    TokenType type;
    optional<string> value;
};

class Tokenizer
{
public:
    inline explicit Tokenizer(string &src) : m_src(move(src))
    {
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;
        string buff;
        while(peek().has_value()){
            if(isalpha(peek().value())){
                //where keywords are checked
                
                buff.push_back(consume());
                while(peek().has_value() && isalpha(peek().value())){
                    buff.push_back(consume());
                }
                if(buff=="exit"){
                    tokens.push_back({.type=TokenType::exit});
                    buff.clear();
                }
                else if(buff=="let"){
                    tokens.push_back({.type=TokenType::let});
                    buff.clear();
                }
                else if(buff=="if"){
                    tokens.push_back({.type=TokenType::if_});
                    buff.clear();
                }
                else{
                    tokens.push_back({.type=TokenType::ident, .value=buff});
                    buff.clear();
                }
            }
            else if(isdigit(peek().value())){
                buff.push_back(consume());
                while(peek().has_value() && isdigit(peek().value())){
                    buff.push_back(consume());
                }
                tokens.push_back({.type = TokenType::int_lit, .value = buff});
                buff.clear();
            }
            else if(peek().value()=='/' && peek(1).has_value() && peek(1).value()=='/'){
                consume();
                consume();
                while(peek().has_value() && peek().value()!='\n'){
                    consume();
                }
            }
            else if(peek().value()=='('){
                consume();
                tokens.push_back({.type=TokenType::open_paren});
            }
            else if(peek().value()==')'){
                consume();
                tokens.push_back({.type=TokenType::close_paren});
            }
            else if(peek().value()=='='){
                consume();
                tokens.push_back({.type=TokenType::eq});
            }
            else if(peek().value()==';'){
                consume();
                tokens.push_back({.type=TokenType::semi});
            }
            else if(peek().value()=='+'){
                consume();
                tokens.push_back({.type=TokenType::plus});
            }
            else if(peek().value()=='*'){
                consume();
                tokens.push_back({.type=TokenType::star});
            }
            else if(peek().value()=='-'){
                consume();
                tokens.push_back({.type=TokenType::minus});
            }
            else if(peek().value()=='/'){
                consume();
                tokens.push_back({.type=TokenType::fslash});
            }
            else if(peek().value()=='{'){
                consume();
                tokens.push_back({.type=TokenType::open_curly});
            }
            else if(peek().value()=='}'){
                consume();
                tokens.push_back({.type=TokenType::close_curly});
            }
            else if(isspace(peek().value())){
                consume();
            }else{
                cerr<<"Unknown character: "<<peek().value()<<"Index: "<<m_index<<endl;
                exit(EXIT_FAILURE);
            }
        }
        m_index=0;
        return tokens;
    }

private:

    [[nodiscard]] inline optional<char> peek(int offset=0) const{
        if(m_index+offset>=m_src.size()){
            return {};
        }else{
            return m_src[m_index+offset];
        }
    }

    inline char consume(){
        return m_src.at(m_index++);
    }

    const string m_src;
    size_t m_index=0;
};