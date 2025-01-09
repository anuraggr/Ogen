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
    be,
    plus,
    star,
    sub,
    div,
    uni_plus,
    uni_sub,
    open_curly,
    close_curly,
    if_condition,
    end_if,
    eq,
    greater_than,
    less_than,
    greater_eq,
    less_eq,
    };

// Converts a string to its corresponding TokenType for Switch case below.
TokenType getStringToTokenType(const std::string& inString){
        static const std::unordered_map<std::string, TokenType> tokenMap = {
            {"exit", TokenType::exit},
            {"let", TokenType::let},
            {"be", TokenType::be},
            {"if", TokenType::if_condition},
            {"end_if", TokenType::end_if}
        };

        auto it = tokenMap.find(inString);
        if(it != tokenMap.end()){
            return it->second;
        }
        return TokenType::ident;
}

// Returns the precedence of a binary operator.
std::optional<int> bin_prec(TokenType type) {
    switch (type) {
        case TokenType::plus:
        case TokenType::sub:
            return 0;
        case TokenType::div:
        case TokenType::star:
            return 1;
        case TokenType::uni_plus:
        case TokenType::uni_sub:    //unused as of yet. unary operators
            return 2;
        default:
            return {};
    }
}

// This is here for debugging.
   inline std::ostream& operator<<(std::ostream& os, const TokenType& type) {  //debug for printing tokens
     switch (type) {
            case TokenType::exit: os << "exit"; break;
            case TokenType::int_lit: os << "int_lit"; break;
            case TokenType::semi: os << "semi"; break;
            case TokenType::open_paren: os << "open_prem"; break;
            case TokenType::close_paren: os << "close_prem"; break;
            case TokenType::ident: os << "ident"; break;
            case TokenType::let: os << "let"; break;
            case TokenType::be: os << "eq"; break;
            case TokenType::plus: os << "plus"; break;
            case TokenType::star: os << "star"; break;
            case TokenType::sub: os << "sub"; break;
            case TokenType::div: os << "div"; break;
            case TokenType::open_curly: os << "open_curly"; break;
            case TokenType::close_curly: os << "close_curly"; break;
            case TokenType::if_condition: os << "if"; break;
            case TokenType::end_if: os << "end_if"; break;
            case TokenType::greater_than: os << "greater_than"; break;
            case TokenType::less_than: os << "less_than"; break;
            case TokenType::greater_eq: os << "greater_eq"; break;
            case TokenType::less_eq: os << "less_eq"; break;
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
            : m_src(std::move(src))                 //member initializer list
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
                    while (peek().has_value() && (std::isalnum(peek().value()) || peek().value() == '_')) {
                        buf.push_back(consume());
                    }
                    switch(getStringToTokenType(buf)){
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
                        case TokenType::be:
                            std::cout << "Buffer is be" << std::endl; //debug
                            tokens.push_back({ .type = TokenType::be });
                            buf.clear();
                            break;
                        case TokenType::if_condition:
                            std::cout << "Buffer is if" << std::endl; //debug
                            tokens.push_back({ .type = TokenType::if_condition });
                            buf.clear();
                            break;
                        case TokenType::end_if:
                            std::cout << "Buffer is end_if" << std::endl; //debug
                            tokens.push_back({ .type = TokenType::end_if });
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
                        case '=':                                       //comparison eq. assignment is 'be'
                            if(peek(1).has_value() && peek(1).value() == '='){
                                consume();
                                consume();
                                tokens.push_back({ .type = TokenType::eq});
                                std::cout << "Buffer is ==" << std::endl;
                                break;
                            }
                            consume();
                            tokens.push_back({ .type = TokenType::be});
                            break;
                        case '>':
                            if(peek(1).has_value() && peek(1).value()=='='){
                                consume();
                                consume();
                                tokens.push_back({ .type = TokenType::greater_eq});
                                std::cout << "Buffer is >=" << std::endl;
                                break;
                            }
                            consume();
                            tokens.push_back({ .type = TokenType::greater_than});
                            std::cout << "Buffer is >" << std::endl;
                            break;
                        case '<':
                            if(peek(1).has_value() && peek(1).value()=='='){
                                consume();
                                consume();
                                tokens.push_back({ .type = TokenType::less_eq});
                                std::cout << "Buffer is <=" << std::endl;
                                break;
                            }
                            consume();
                            tokens.push_back({ .type = TokenType::less_than});
                            std::cout << "Buffer is <" << std::endl;
                            break;
                        case '+':
                            if(peek(1).has_value() && peek(1).value() == '+' && peek(2).has_value() 
                                && peek(2).value() == '+'){
                                consume();
                                consume();
                                tokens.push_back({ .type = TokenType::uni_plus});
                                std::cout << "Buffer is ++" << std::endl;
                                break;
                            }
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
                            if(peek(1).has_value() && peek(1).value() == '-' &&
                                peek(2).has_value() && peek(2).value() == '-'){
                                consume();
                                consume();
                                tokens.push_back({ .type = TokenType::uni_sub});
                                std::cout << "Buffer is --" << std::endl;
                                break;
                            }
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
                        case '#':
                            while(peek().has_value() && peek().value()!='\n'){
                                consume();
                            }
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
                return m_src.at(m_index + offset);
            }
        }

        inline char consume()
        {
            return m_src.at(m_index++);
        }

        const std::string m_src;
        size_t m_index = 0;
};