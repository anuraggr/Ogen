#pragma once

#include <string>
#include <vector>
#include <unordered_map>

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
    div,
    open_curly,
    close_curly
    };

TokenType string_hash(const std::string& inString){
        static const std::unordered_map<std::string, TokenType> tokenMap = {
            {"exit", TokenType::exit},
            {"let", TokenType::let}
        };

        auto it = tokenMap.find(inString);
        if(it != tokenMap.end()){
            return it->second;
        }
        return TokenType::ident;
}

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
            case TokenType::plus: os << "plus"; break;
            case TokenType::star: os << "star"; break;
            case TokenType::sub: os << "sub"; break;
            case TokenType::div: os << "div"; break;
            case TokenType::open_curly: os << "open_curly"; break;
            case TokenType::close_curly: os << "close_curly"; break;
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

                char currentChar = peek().value();
                

                if (std::isalpha(currentChar)) {  //keywords or ident(x, y, etc)
                    
                    buf.push_back(consume());
                    while (peek().has_value() && std::isalnum(peek().value())) {
                        buf.push_back(consume());
                    }
                    switch(string_hash(buf)){
                        case TokenType::exit:
                            std::cout << "Buffer is exit" << std::endl; //debug
                            tokens.push_back({ .type = TokenType::exit });
                            buf.clear();
                            break;
                        case TokenType::let:
                            std::cout << "Buffer is let" << std::endl; //debug
                            tokens.push_back({ .type = TokenType::let });
                            buf.clear();
                            break;
                        case TokenType::ident:
                            std::cout << "Buffer is ident" << std::endl; //debug
                            tokens.push_back({ .type = TokenType::ident, .value = buf });
                            buf.clear();
                            break;
                        default:
                            std::cerr << "Unknown token type: " << buf << std::endl;
                            exit(EXIT_FAILURE);
                    }
                }
                else if (std::isdigit(peek().value())) {
                    buf.push_back(consume());
                    while (peek().has_value() && std::isdigit(peek().value())) {
                        buf.push_back(consume());
                    }
                    tokens.push_back({ .type = TokenType::int_lit, .value = buf });
                    buf.clear();
                }

                else{
                    switch(currentChar){
                        case '(':
                            consume();
                            tokens.push_back({ .type = TokenType::open_paren});
                            std::cout << "Buffer is (" << std::endl;
                            break;
                        case ')':
                            consume();
                            tokens.push_back({ .type = TokenType::close_paren});
                            std::cout << "Buffer is )" << std::endl;
                            break;
                        case ';':
                            consume();
                            tokens.push_back({ .type = TokenType::semi});
                            std::cout << "Buffer is ;" << std::endl;
                            break;
                        case '=':
                            consume();
                            tokens.push_back({ .type = TokenType::eq});
                            std::cout << "Buffer is =" << std::endl;
                            break;
                        case '+':
                            consume();
                            tokens.push_back({ .type = TokenType::plus});
                            std::cout << "Buffer is +" << std::endl;
                            break;
                        case '*':
                            consume();
                            tokens.push_back({ .type = TokenType::star});
                            std::cout << "Buffer is *" << std::endl;
                            break;
                        case '-':
                            consume();
                            tokens.push_back({ .type = TokenType::sub});
                            std::cout << "Buffer is -" << std::endl;
                            break;
                        case '/':
                            consume();
                            tokens.push_back({ .type = TokenType::div});
                            std::cout << "Buffer is div" << std::endl;
                            break;
                        case '{':
                            consume();
                            tokens.push_back({ .type = TokenType::open_curly});
                            std::cout << "Buffer is {" << std::endl;
                            break;
                        case '}':
                            consume();
                            tokens.push_back({ .type = TokenType::close_curly});
                            std::cout << "Buffer is }" << std::endl;
                            break;
                        case ' ':
                        case '\n':   //isspace() returns true for new line. here i have to handle it myself
                        case '\t':
                            consume();
                            std::cout << "Space" << std::endl;
                            break;
                        default:
                            std::cerr << "Unknown token: " << currentChar << std::endl;
                            exit(EXIT_FAILURE);
                            
                    }
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