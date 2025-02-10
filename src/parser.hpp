#pragma once

#include <variant>

#include "./arena.hpp"
#include "tokenization.hpp"


struct NodeExpr;
struct NodeStmt;

struct NodeTermIntLit {
    Token int_lit;
};

struct NodeTermIdent {
    Token ident;
};

struct NodeTermParen {
    NodeExpr *expr;
};

struct NodeTerm {
    std::variant<NodeTermIntLit *, NodeTermIdent *, NodeTermParen *> var;
};

struct NodeBinExprAdd {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprMulti {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprSub {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprDiv {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExpr {
    std::variant<NodeBinExprAdd *, NodeBinExprMulti *, NodeBinExprSub *, NodeBinExprDiv *> var;
};

struct NodeExpr {
    std::variant<NodeTerm *, NodeBinExpr *> var;
};

struct NodeComparison {
    Token comp;
};

struct NodeStmtExit {
    NodeExpr *expr;
};

struct NodeStmtLet {
    Token ident;
    NodeExpr *expr;
};

struct NodeStmtAssign {
    NodeTermIdent* lhs;
    NodeExpr *rhs;
};

struct NodeStmtScope {
    std::vector<NodeStmt *> stmts;
};

struct NodeStmtIf {
    NodeExpr *lhs;
    NodeExpr *rhs;
    NodeComparison *comparison;
    std::vector<NodeStmt *> body;
    std::vector<NodeStmt *> elif_body;
    std::vector<NodeStmt *> else_body;
};

struct NodeStmtWhile {
    NodeExpr *lhs;
    NodeExpr *rhs;
    NodeComparison *comparison;
    std::vector<NodeStmt *> body;
};

struct NodeStmtFor {
    std::variant<NodeStmtLet *, NodeStmtAssign *> init;
    NodeExpr *condition_lhs;
    NodeExpr *condition_rhs;
    NodeComparison *comparision;
    NodeStmtAssign *change;
    std::vector<NodeStmt *> body;
};

struct NodeFun {
    std::vector<NodeStmt *> body;
};

struct NodeStmt {
    std::variant<NodeStmtExit *, NodeStmtLet *, NodeStmtScope *, NodeStmtIf *, 
                NodeStmtWhile *, NodeStmtFor *, NodeStmtAssign *, NodeFun *> var;
};

struct NodeProg {
    std::vector<NodeStmt *> stmts;
};

class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
        : m_tokens(std::move(tokens)), m_allocator(1024 * 1024 * 4) // 4 mb
    {
    }

    std::optional<NodeTerm *> parse_term() {
        if (auto int_lit = try_consume(TokenType::int_lit)) {
            auto term_int_lit = m_allocator.alloc<NodeTermIntLit>();
            term_int_lit->int_lit = int_lit.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_int_lit;
            return term;
        } else if (auto ident = try_consume(TokenType::ident)) {
            auto expr_ident = m_allocator.alloc<NodeTermIdent>();
            expr_ident->ident = ident.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = expr_ident;
            return term;
        } else if (auto open_paren = try_consume(TokenType::open_paren)) {
            auto expr = parse_expr();
            if (!expr.has_value()) {
                std::cerr << "Expected expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected `)`");
            auto term_paren = m_allocator.alloc<NodeTermParen>();
            term_paren->expr = expr.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_paren;
            return term;
        } else {
            return {};
        }
    }

    std::optional<NodeExpr *> parse_expr(int min_prec = 0) // precedence climbing algo used. i dont have it fully understood ngl
    {

        std::optional<NodeTerm *> term_lhs = parse_term();
        if (!term_lhs.has_value()) {
            std::cout << "Term LHS has no value" << std::endl; // debug
            return {};
        }

        auto expr_lhs = m_allocator.alloc<NodeExpr>();
        expr_lhs->var = term_lhs.value();

        while (true) {
            std::optional<Token> curr_token = peek();
            std::optional<int> prec;
            if (curr_token.has_value()) {
                prec = bin_prec(curr_token->type);
                if (!prec.has_value() || prec < min_prec) {
                    break;
                }
            } else {
                break;
            }

            Token op = consume();
            int next_min_prec = prec.value() + 1;
            auto expr_rhs = parse_expr(next_min_prec);
            if (!expr_rhs.has_value()) {
                std::cerr << "Expected expression" << std::endl;
                exit(EXIT_FAILURE);
            }

            auto expr = m_allocator.alloc<NodeBinExpr>();
            auto expr_lhs2 = m_allocator.alloc<NodeExpr>();
            if (op.type == TokenType::plus) {
                auto add = m_allocator.alloc<NodeBinExprAdd>();
                expr_lhs2->var = expr_lhs->var;
                add->lhs = expr_lhs2;
                add->rhs = expr_rhs.value();
                expr->var = add;
            } else if (op.type == TokenType::star) {
                auto multi = m_allocator.alloc<NodeBinExprMulti>();
                expr_lhs2->var = expr_lhs->var;
                multi->lhs = expr_lhs2;
                multi->rhs = expr_rhs.value();
                expr->var = multi;
            } else if (op.type == TokenType::sub) {
                auto sub = m_allocator.alloc<NodeBinExprSub>();
                expr_lhs2->var = expr_lhs->var;
                sub->lhs = expr_lhs2;
                sub->rhs = expr_rhs.value();
                expr->var = sub;
            } else if (op.type == TokenType::div) {
                auto div = m_allocator.alloc<NodeBinExprDiv>();
                expr_lhs2->var = expr_lhs->var;
                div->lhs = expr_lhs2;
                div->rhs = expr_rhs.value();
                expr->var = div;
            }

            else {
                std::cerr << "Invalid operator" << std::endl;
                exit(EXIT_FAILURE);
            }
            expr_lhs->var = expr;
        }
        return expr_lhs;
    }

    std::optional<NodeStmtScope *> parse_scope() {
        if (auto open_curly = try_consume(TokenType::open_curly)) {
            auto scope = m_allocator.alloc<NodeStmtScope>();
            while (auto stmt = parse_stmt()) {
                scope->stmts.push_back(stmt.value());
            }
            try_consume(TokenType::close_curly, "Expected `}`");
            return scope;
        } else {
            return {};
        }
    }

//function to parse elif in the if-statement
    void resolveElif(NodeStmtIf* stmt_if){
        while (peek().has_value() && peek().value().type == TokenType::elif) {
            consume();
            try_consume(TokenType::open_paren, "Expected `(`");
            auto elif_stmt = m_allocator.alloc<NodeStmtIf>();
            if (auto lhs = parse_expr()) {
                elif_stmt->lhs = lhs.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value() && peek().value().type == TokenType::close_paren){
                consume();
                if(auto scope = parse_scope()){
                    elif_stmt->body = scope.value()->stmts;
                }
                else{
                    std::cerr << "Invalid scope on if statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto elif_stmt_node = m_allocator.alloc<NodeStmt>();
                elif_stmt_node->var = elif_stmt;
                stmt_if->elif_body.push_back(elif_stmt_node);
            }
            else{
                elif_stmt->comparison = m_allocator.alloc<NodeComparison>();
                elif_stmt->comparison->comp = consume(); // consumes comparison
                if (auto rhs = parse_expr()) {
                    elif_stmt->rhs = rhs.value();
                } else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                try_consume(TokenType::close_paren, "Expected `)`");
                if (auto scope = parse_scope()) {
                    elif_stmt->body = scope.value()->stmts;
                } else {
                    std::cerr << "Invalid scope on elif statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto elif_stmt_node = m_allocator.alloc<NodeStmt>();
                elif_stmt_node->var = elif_stmt;
                stmt_if->elif_body.push_back(elif_stmt_node);
            }
        }
    }

    std::optional<NodeStmt *> parse_stmt() {
        if (peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_paren) {
            consume();
            consume();
            std::cout << "Exit" << std::endl;
            auto stmt_exit = m_allocator.alloc<NodeStmtExit>();
            if (auto node_expr = parse_expr()) {
                stmt_exit->expr = node_expr.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected `)`");
            try_consume(TokenType::semi, "Expected `;`");
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_exit;
            return stmt;
        }

//LET
        else if (
            peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value() && peek(1).value().type == TokenType::ident && peek(2).has_value() && peek(2).value().type == TokenType::eq) {
            consume();
            std::cout << "Let" << std::endl;
            auto stmt_let = m_allocator.alloc<NodeStmtLet>();
            stmt_let->ident = consume(); // consumes indet
            consume();                   // consumes =
            if (auto expr = parse_expr()) {
                stmt_let->expr = expr.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::semi, "Expected `;`");
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
        }

        else if(peek().has_value() && peek().value().type == TokenType::ident){
            auto stmt_assign = m_allocator.alloc<NodeStmtAssign>();
            auto term_ident = m_allocator.alloc<NodeTermIdent>();
            term_ident->ident = consume();
            stmt_assign->lhs = term_ident;
            try_consume(TokenType::eq, "Incomplete statement");
            if(auto rhs = parse_expr()){
                stmt_assign->rhs = rhs.value();
            }
            else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
                }
            try_consume(TokenType::semi, "Expected `;`");
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_assign;
            return stmt;
        }


//IF CONDITION
        else if (peek().has_value() && peek().value().type == TokenType::if_condition) {
            consume();
            try_consume(TokenType::open_paren, "Expected `(`");
            std::cout << "If" << std::endl; // debug
            auto stmt_if = m_allocator.alloc<NodeStmtIf>();
            if (auto lhs = parse_expr()) {
                stmt_if->lhs = lhs.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if(peek().has_value() && peek().value().type == TokenType::close_paren){
                consume();
                if(auto scope = parse_scope()){
                    stmt_if->body = scope.value()->stmts;
                }
                else{
                    std::cerr << "Invalid scope on if statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                resolveElif(stmt_if);
            }
            else{
                stmt_if->comparison = m_allocator.alloc<NodeComparison>();
                stmt_if->comparison->comp = consume(); // consumes comparison
                if (auto rhs = parse_expr()) {
                    stmt_if->rhs = rhs.value();
                } else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                try_consume(TokenType::close_paren, "Expected `)`");
                if (auto scope = parse_scope()) {
                    stmt_if->body = scope.value()->stmts;
                } else {
                    std::cerr << "Invalid scope on if statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                resolveElif(stmt_if);
            }
            if (peek().has_value() && peek().value().type == TokenType::else_condition) {
                consume();
                if (auto scope = parse_scope()) {
                    stmt_if->else_body = scope.value()->stmts;
                } else {
                    std::cerr << "Invalid scope on else statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_if;
            return stmt;
        }

//WHILE
        else if(peek().has_value() && peek().value().type == TokenType::while_condition){
            consume();
            try_consume(TokenType::open_paren, "Expected '('");
            auto stmt_while = m_allocator.alloc<NodeStmtWhile>();
            if(auto lhs = parse_expr()){
                stmt_while->lhs = lhs.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            stmt_while->comparison = m_allocator.alloc<NodeComparison>();
            stmt_while->comparison->comp = consume();
            if (auto rhs = parse_expr()) {
                stmt_while->rhs = rhs.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected ')");
            if(auto scope = parse_scope()){
                stmt_while->body = scope.value()->stmts;
            }
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_while;
            return stmt;
        }

//FOR LOOP

        else if(peek().has_value() && peek().value().type == TokenType::for_loop){
            consume();
            try_consume(TokenType::open_paren, "Expected '('");
            auto stmt_for = m_allocator.alloc<NodeStmtFor>();

            if(peek().has_value() && peek().value().type == TokenType::let){
            consume();
                if(peek().has_value() && peek().value().type == TokenType::ident && peek(1).has_value() && peek(1).value().type == TokenType::eq) {
                    std::cout << "Let" << std::endl;
                    auto stmt_let = m_allocator.alloc<NodeStmtLet>();
                    stmt_let->ident = consume(); // consumes indet
                    consume();                   // consumes =
                    if (auto expr = parse_expr()) {
                        stmt_let->expr = expr.value();
                    } else {
                        std::cerr << "Invalid expression" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    stmt_for->init = stmt_let;
                }
                else {
                    std::cerr << "Incorrect identifier initialization in for loop" << std::endl;
                    exit(EXIT_FAILURE);
                }
            } else if (peek().has_value() && peek().value().type == TokenType::ident){
                auto stmt_assign = m_allocator.alloc<NodeStmtAssign>();
                auto term_ident = m_allocator.alloc<NodeTermIdent>();
                term_ident->ident = consume();
                stmt_assign->lhs = term_ident;
                try_consume(TokenType::eq, "Incomplete statement");
                if (auto rhs = parse_expr()) {
                    stmt_assign->rhs = rhs.value();
                } else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                stmt_for->init = stmt_assign;
            }

            try_consume(TokenType::semi, "Expected `;`");

            if (auto lhs = parse_expr()) {
                stmt_for->condition_lhs = lhs.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            stmt_for->comparision = m_allocator.alloc<NodeComparison>();
            stmt_for->comparision->comp = consume();
            if (auto rhs = parse_expr()) {
                stmt_for->condition_rhs = rhs.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::semi, "Expected `;`");

            if(peek().has_value() && peek().value().type != TokenType::close_paren) {
                auto stmt_assign = m_allocator.alloc<NodeStmtAssign>();
                auto term_ident = m_allocator.alloc<NodeTermIdent>();
                term_ident->ident = consume();
                stmt_assign->lhs = term_ident;
                try_consume(TokenType::eq, "Incomplete statement");
                if (auto rhs = parse_expr()) {
                    stmt_assign->rhs = rhs.value();
                } else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                stmt_for->change = stmt_assign;
            }
            try_consume(TokenType::close_paren, "Expected ')'");

            if (auto scope = parse_scope()) {
                stmt_for->body = scope.value()->stmts;
            }

            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_for;
            return stmt;
        }

// FUNCTION
        else if(peek().has_value() && peek().value().type == TokenType::fun){
            consume();
            try_consume(TokenType::open_paren, "Expected '('");
            try_consume(TokenType::close_paren, "Expected ')'");
            try_consume(TokenType::open_curly, "Expected Scope for function");
            auto stmt_fun = m_allocator.alloc<NodeFun>();
            if (auto scope = parse_scope()) {
                stmt_fun->body = scope.value()->stmts;
            }
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_fun;
            return stmt;
        }


// SCOPE

        else if (auto open_curly = try_consume(TokenType::open_curly)) {
                std::cout << "Scope Open" << std::endl; // debug
                auto scope = m_allocator.alloc<NodeStmtScope>();
                while (auto stmt = parse_stmt()) {
                    scope->stmts.push_back(stmt.value());
                }
                try_consume(TokenType::close_curly, "Expected `}`");
                auto stmt = m_allocator.alloc<NodeStmt>();
                stmt->var = scope;
                return stmt;
            } else {
                return {};
            }
    }

    std::optional<NodeProg> parse_prog() {
        NodeProg prog;
        while (peek().has_value()) {
            if (auto stmt = parse_stmt()) {
                prog.stmts.push_back(stmt.value());
            } else {
                std::cerr << "Exited with error: Invalid statement" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        return prog;
    }

private:
    [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        } else {
            return m_tokens.at(m_index + offset);
        }
    }

    inline Token consume() {
        return m_tokens.at(m_index++);
    }

    inline Token try_consume(TokenType type, const std::string &err_msg) {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        } else {
            std::cerr << err_msg << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    inline std::optional<Token> try_consume(TokenType type) {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        } else {
            return {};
        }
    }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;
};