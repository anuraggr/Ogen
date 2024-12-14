#pragma once
#include <variant>

#include "tokenization.hpp"


struct NodeExprIntLit{
    Token int_lit;
};

struct NodeExprIdent {
    Token ident;
};

struct NodeExpr {
    std::variant<NodeExprIntLit, NodeExprIdent> var;      //varient enables for either of the two types
};

struct NodeStmtExit {
    NodeExpr expr;
};

struct NodeStmtLet {
    Token ident;
    NodeExpr expr;
};

struct NodeStmt{
    std::variant<NodeStmtExit, NodeStmtLet> var;
};

struct NodeProg{
    std::vector<NodeStmt> stmts;
};

class Parser {
    public:
        inline explicit Parser(std::vector<Token> tokens)
            : m_tokens(std::move(tokens))
        {
            
        }

        std::optional<NodeExpr> parse_expr()
        {
            if (peak().has_value() && peak().value().type == TokenType::int_lit) {
                return NodeExpr{.var = NodeExprIntLit{.int_lit = consume()}};
            }
            else if(peak().has_value() && peak().value().type == TokenType::ident) {
                return NodeExpr{.var = NodeExprIdent{.ident = consume()}};
            }
            else {
                return {};
            }
        }

        std::optional<NodeStmt> parse_stmt(){
            if (peak().value().type == TokenType::exit && peak(1).has_value() && peak(1).value().type == TokenType::open_prem) {
                    consume();
                    consume();
                    NodeStmtExit stmt_exit;
                    if (auto node_expr = parse_expr()) {
                        stmt_exit = { .expr = node_expr.value() };

                    }
                    else {
                        std::cerr << "Expected expression after 'exit'" << std::endl;
                        exit(EXIT_FAILURE);
                    }

                    if(peak().has_value() && peak().value().type == TokenType::close_prem) {
                        consume();
                    }
                    else {
                        std::cerr << "Expected ')' after expression" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    
                    if (peak().has_value() && peak().value().type == TokenType::semi) {
                        consume();
                    }
                    else {
                        std::cerr << "Expected ';' after expression" << std::endl;
                        exit(EXIT_FAILURE);
                        
                    }
                    return NodeStmt{.var = stmt_exit};
                 
            }
            else if(peak().has_value() && peak().value().type == TokenType::let && peak(1).has_value() && peak(1).value().type==TokenType::ident && peak(2).has_value() && peak(2).value().type==TokenType::eq){
                consume();
                auto stmt_let = NodeStmtLet{.ident = consume()};
                consume();
                if(auto expr = parse_expr()){
                    stmt_let.expr = expr.value();
                }
                else{
                    std::cerr << "Expected expression after 'let'" << std::endl;
                    exit(EXIT_FAILURE);
                }
                if(peak().has_value() && peak().value().type == TokenType::semi){
                    consume();
                }
                else{
                    std::cerr << "Expected ';' after expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return NodeStmt{.var = stmt_let};
            }
            else{
                std::cerr << "Unexpected token" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        std::optional<NodeProg> parse_prog()
        {
            NodeProg prog;
            while(peak().has_value()){
                if(auto stmt = parse_stmt()){
                    prog.stmts.push_back(stmt.value());
                }
                else{
                    std::cerr << "Unexpected statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            return prog;
        }

    private:
        [[nodiscard]] inline std::optional<Token> peak(int offset = 0) const
        {
            if (m_index + offset >= m_tokens.size()) {
                return {};
            }
            else {
                return m_tokens.at(m_index + offset);
            }
        }

        inline Token consume()
        {
            return m_tokens.at(m_index++);
        }

        const std::vector<Token> m_tokens;
        size_t m_index = 0;
};