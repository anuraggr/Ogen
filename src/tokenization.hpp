#pragma once //includes header file only once
#include <iostream>
#include <string>
#include <vector>
#include <optional>


enum class TokenType {
    _exit,
    int_lit,
    semi
};

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

        inline std::vector<Token> tokenize() {

            std::vector<Token> tokens;

            std::string buf;

            while(peak().has_value()) {
                if(std::isalpha(peak().value())){
                    buf.push_back(consume());
                    while(peak().has_value() && std::isalnum(peak().value())){
                        buf.push_back(consume());
                    }
                    if(buf == "exit") {
                        tokens.push_back({.type = TokenType::_exit});
                        buf.clear();
                        continue;
                    }
                    else{
                        std::cerr << "Unknown token: " << buf << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
                else if(peak().has_value() && std::isdigit(peak().value())) {

                    buf.push_back(consume());
                    while(peak().has_value() && std::isdigit(peak().value())){
                        buf.push_back(consume());
                    }
                    tokens.push_back({.type = TokenType::int_lit, .value = buf});
                    buf.clear();
                    continue;
                }
                else if(peak().value() == ';') {
                    consume();
                    tokens.push_back({.type = TokenType::semi});
                    continue;
                }
                else if(std::isspace(peak().value())) {
                    consume();
                    continue;
                }
                else{
                    std::cerr << "Unknown token: " << peak().value() << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            m_index = 0;
            return tokens;

        }
        

    private:

        [[nodiscard]]std::optional<char> peak(int ahead = 1) const {
            if(m_index + ahead > m_src.length()){
                return {};
            }
            else{
                return m_src.at(m_index);
            }
        }

        char consume() {
            return m_src.at(m_index++);
        }

        const std::string m_src;
        int m_index = 0;
};