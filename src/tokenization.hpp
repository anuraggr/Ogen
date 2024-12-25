#pragma once

#include <string>
#include <vector>

enum class TokenType { 
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
    sub,
    div
    };


std::optional<int> bin_prec(TokenType type) {
    switch (type) {
        case TokenType::plus:
        case TokenType::sub:
            return 0;
        case TokenType::div:
        case TokenType::star:
            return 1;
        default:
            return {};
    }
}
    
   inline std::ostream& operator<<(std::ostream& os, const TokenType& type) {  //debug for printing tokens
     switch (type) {
            case TokenType::exit: os << "exit"; break;
            case TokenType::int_lit: os << "int_lit"; break;
            case TokenType::semi: os << "semi"; break;
             case TokenType::open_paren: os << "open_prem"; break;
             case TokenType::close_paren: os << "close_prem"; break;
             case TokenType::ident: os << "ident"; break;
             case TokenType::let: os << "let"; break;
             case TokenType::eq: os << "eq"; break;
         }
         return os;
     }

struct Token {
    TokenType type;
    std::optional<std::string> value {};
};

class Tokenizer {
    public:
        inline explicit Tokenizer(std::string src)
            : m_src(std::move(src))
        {
        }

        inline std::vector<Token> tokenize()
        {
            std::cout << "Tokenizing...\n" << m_src <<std::endl; //debug
            std::vector<Token> tokens; //type, value. value is optional 
            std::string buf;
            while (peek().has_value()) {
                if (std::isalpha(peek().value())) {
                    buf.push_back(consume());
                    while (peek().has_value() && std::isalnum(peek().value())) {
                        buf.push_back(consume());
                    }
                    if (buf == "exit") {
                        std::cout << "Buffer is exit" << std::endl; //debug
                        tokens.push_back({ .type = TokenType::exit });
                        buf.clear();
                    }

                    else if (buf == "let") {
                        std::cout << "Buffer is let" << std::endl; //debug
                        tokens.push_back({ .type = TokenType::let });
                        buf.clear();
                    }

                    else {
                        std::cout << "Buffer is ident" << std::endl; //debug
                        tokens.push_back({ .type = TokenType::ident, .value = buf });
                        buf.clear();
                    }
                }
                else if (std::isdigit(peek().value())) {
                    buf.push_back(consume());
                    while (peek().has_value() && std::isdigit(peek().value())) {
                        buf.push_back(consume());
                    }
                    tokens.push_back({ .type = TokenType::int_lit, .value = buf });
                    std::cout << "Buffer is int_lit" << std::endl; //debug
                    buf.clear();
                }

                else if (peek().value() == '('){
                    consume();
                    tokens.push_back({ .type = TokenType::open_paren });
                    std::cout << "Buffer is (" << std::endl; //debug
                }

                else if (peek().value() == ')'){
                    consume();
                    tokens.push_back({ .type = TokenType::close_paren });
                    std::cout << "Buffer is )" << std::endl; //debug
                }

                else if (peek().value() == ';') {
                    consume();
                    tokens.push_back({ .type = TokenType::semi });
                    std::cout << "Buffer is ;" << std::endl; //debug
                }

                else if (peek().value() == '=') {
                    consume();
                    tokens.push_back({ .type = TokenType::eq});
                    std::cout << "Buffer is =" << std::endl; //debug
                }


                else if (peek().value() == '+') {
                    consume();
                    tokens.push_back({ .type = TokenType::plus });
                }

                else if(peek().value() == '*') {
                    consume();
                    tokens.push_back({ .type = TokenType::star });
                }

                else if(peek().value() == '-') {
                    consume();
                    tokens.push_back({ .type = TokenType::sub });
                }

                else if(peek().value() == '/') {
                    consume();
                    tokens.push_back({ .type = TokenType::div });
                }


                else if (std::isspace(peek().value())) {
                    consume();
                    std::cout << "empty space" << std::endl; //debug
                }
                else {
                    std::cerr << "Unknown token: " << peek().value() << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

        std::cout << "\n" << std::endl;            //debug for printing tokens
        for(const auto& token : tokens){
                std::cout <<"Token: " << token.type << " " << token.value.value_or("") << std::endl; //debug
            }
            m_index = 0;
            return tokens;
        }

    private:
        [[nodiscard]] inline std::optional<char> peek(int offset = 0) const
        {
            if (m_index + offset >= m_src.length()) {
                return {};
            }
            else {
                std::cout << "Peaking: " << m_src.at(m_index + offset) << std::endl; //debug
                return m_src.at(m_index + offset);
            }
        }

        inline char consume()
        {
            std::cout << "Consuming: " << m_src.at(m_index) << std::endl; //debug
            return m_src.at(m_index++);
        }

        const std::string m_src;
        size_t m_index = 0;
};