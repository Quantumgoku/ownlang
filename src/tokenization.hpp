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
    plus
};

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
                    continue;
                }
                else if(buff=="let"){
                    tokens.push_back({.type=TokenType::let});
                    buff.clear();
                    continue;
                }
                else{
                    tokens.push_back({.type=TokenType::ident, .value=buff});
                    buff.clear();
                    continue;
                }
            }
            else if(isdigit(peek().value())){
                buff.push_back(consume());
                while(peek().has_value() && isdigit(peek().value())){
                    buff.push_back(consume());
                }
                tokens.push_back({.type = TokenType::int_lit, .value = buff});
                buff.clear();
                continue;
            }
            else if(peek().value()=='('){
                consume();
                tokens.push_back({.type=TokenType::open_paren});
                continue;
            }
            else if(peek().value()==')'){
                consume();
                tokens.push_back({.type=TokenType::close_paren});
                continue;
            }
            else if(peek().value()=='='){
                consume();
                tokens.push_back({.type=TokenType::eq});
                continue;
            }
            else if(peek().value()==';'){
                consume();
                tokens.push_back({.type=TokenType::semi});
                continue;
            }
            else if(peek().value()=='+'){
                consume();
                tokens.push_back({.type=TokenType::plus});
                continue;
            }
            else if(isspace(peek().value())){
                consume();
                continue;
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