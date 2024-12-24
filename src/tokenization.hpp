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
    plus
    };
    
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
            while (peak().has_value()) {
                if (std::isalpha(peak().value())) {
                    buf.push_back(consume());
                    while (peak().has_value() && std::isalnum(peak().value())) {
                        buf.push_back(consume());
                    }
                    if (buf == "exit") {
                        std::cout << "Buffer is exit" << std::endl; //debug
                        tokens.push_back({ .type = TokenType::exit });
                        buf.clear();
                        continue;
                    }

                    else if (buf == "let") {
                        std::cout << "Buffer is let" << std::endl; //debug
                        tokens.push_back({ .type = TokenType::let });
                        buf.clear();
                        continue;
                    }

                    else {
                        std::cout << "Buffer is ident" << std::endl; //debug
                        tokens.push_back({ .type = TokenType::ident, .value = buf });
                        buf.clear();
                        continue;
                        
                    }
                }
                else if (std::isdigit(peak().value())) {
                    buf.push_back(consume());
                    while (peak().has_value() && std::isdigit(peak().value())) {
                        buf.push_back(consume());
                    }
                    tokens.push_back({ .type = TokenType::int_lit, .value = buf });
                    std::cout << "Buffer is int_lit" << std::endl; //debug
                    buf.clear();
                    continue;
                }

                else if (peak().value() == '('){
                    consume();
                    tokens.push_back({ .type = TokenType::open_paren });
                    std::cout << "Buffer is (" << std::endl; //debug
                    continue;
                }

                else if (peak().value() == ')'){
                    consume();
                    tokens.push_back({ .type = TokenType::close_paren });
                    std::cout << "Buffer is )" << std::endl; //debug
                    continue;
                }

                else if (peak().value() == ';') {
                    consume();
                    tokens.push_back({ .type = TokenType::semi });
                    std::cout << "Buffer is ;" << std::endl; //debug
                    continue;
                }

                else if (peak().value() == '=') {
                    consume();
                    tokens.push_back({ .type = TokenType::eq});
                    std::cout << "Buffer is =" << std::endl; //debug
                    continue;
                }


                else if (peak().value() == '+') {
                    consume();
                    tokens.push_back({ .type = TokenType::plus });
                    continue;
                }


                else if (std::isspace(peak().value())) {
                    consume();
                    std::cout << "empty space" << std::endl; //debug
                    continue;
                }
                else {
                    std::cerr << "Unknown token: " << peak().value() << std::endl;
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
        [[nodiscard]] inline std::optional<char> peak(int offset = 0) const
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