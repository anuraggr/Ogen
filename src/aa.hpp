// IF STATEMENT PARSING
/////////////////////////////////////////
else if (peek().has_value() && peek().value().type == TokenType::if_condition) {
    consume();
    try_consume(TokenType::open_paren, "Expected '('");
    std::cout << "If" << std::endl; // debug
    auto stmt_if = m_allocator.alloc<NodeStmtIf>();
    if (auto lhs = parse_expr()) {
        stmt_if->lhs = lhs.value();
    } else {
        std::cerr << "Invalid expression" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (peek().has_value() && peek().value().type == TokenType::close_paren) {
        consume();
        if (auto scope = parse_scope()) {
            stmt_if->body = (*scope)->stmts;
        } else {
            std::cerr << "Invalid scope on if statement" << std::endl;
            exit(EXIT_FAILURE);
        }
        NodeStmtIf *current_if = stmt_if;
        while (peek().has_value() && peek().value().type == TokenType::elif) {
            consume();
            try_consume(TokenType::open_paren, "Expected '('");
            auto stmt_elif = m_allocator.alloc<NodeStmtIf>();
            if (auto lhs = parse_expr()) {
                stmt_elif->lhs = lhs.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() && peek().value().type == TokenType::close_paren) {
                consume();
                if (auto scope = parse_scope()) {
                    stmt_elif->body = (*scope)->stmts;
                } else {
                    std::cerr << "Invalid scope on elif statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto elif_stmt = m_allocator.alloc<NodeStmt>();
                elif_stmt->var = stmt_elif;
                current_if->elif_body.push_back(elif_stmt);
                current_if = stmt_elif;
            } else {
                stmt_elif->comparison = m_allocator.alloc<NodeComparison>();
                stmt_elif->comparison->comp = consume();
                if (auto rhs = parse_expr()) {
                    stmt_elif->rhs = rhs.value();
                } else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                try_consume(TokenType::close_paren, "Expected ')'");
                if (auto scope = parse_scope()) {
                    stmt_elif->body = (*scope)->stmts;
                } else {
                    std::cerr << "Invalid scope on elif statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto elif_stmt = m_allocator.alloc<NodeStmt>();
                elif_stmt->var = stmt_elif;
                current_if->elif_body.push_back(elif_stmt);
                current_if = stmt_elif;
            }
        }
    } else {
        stmt_if->comparison = m_allocator.alloc<NodeComparison>();
        stmt_if->comparison->comp = consume();
        if (auto rhs = parse_expr()) {
            stmt_if->rhs = rhs.value();
        } else {
            std::cerr << "Invalid expression" << std::endl;
            exit(EXIT_FAILURE);
        }
        try_consume(TokenType::close_paren, "Expected ')'");
        if (auto scope = parse_scope()) {
            stmt_if->body = (*scope)->stmts;
        } else {
            std::cerr << "Invalid scope on if statement" << std::endl;
            exit(EXIT_FAILURE);
        }
        NodeStmtIf *current_if = stmt_if;
        while (peek().has_value() && peek().value().type == TokenType::elif) {
            consume();
            try_consume(TokenType::open_paren, "Expected '('");
            auto stmt_elif = m_allocator.alloc<NodeStmtIf>();
            if (auto lhs = parse_expr()) {
                stmt_elif->lhs = lhs.value();
            } else {
                std::cerr << "Invalid expression" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (peek().has_value() && peek().value().type == TokenType::close_paren) {
                consume();
                if (auto scope = parse_scope()) {
                    stmt_elif->body = (*scope)->stmts;
                } else {
                    std::cerr << "Invalid scope on elif statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto elif_stmt = m_allocator.alloc<NodeStmt>();
                elif_stmt->var = stmt_elif;
                current_if->elif_body.push_back(elif_stmt);
                current_if = stmt_elif;
            } else {
                stmt_elif->comparison = m_allocator.alloc<NodeComparison>();
                stmt_elif->comparison->comp = consume();
                if (auto rhs = parse_expr()) {
                    stmt_elif->rhs = rhs.value();
                } else {
                    std::cerr << "Invalid expression" << std::endl;
                    exit(EXIT_FAILURE);
                }
                try_consume(TokenType::close_paren, "Expected ')'");
                if (auto scope = parse_scope()) {
                    stmt_elif->body = (*scope)->stmts;
                } else {
                    std::cerr << "Invalid scope on elif statement" << std::endl;
                    exit(EXIT_FAILURE);
                }
                auto elif_stmt = m_allocator.alloc<NodeStmt>();
                elif_stmt->var = stmt_elif;
                current_if->elif_body.push_back(elif_stmt);
                current_if = stmt_elif;
            }
        }
    }

    if (peek().has_value() && peek().value().type == TokenType::else_condition) {
        consume();
        if (auto scope = parse_scope()) {
            stmt_if->else_body = (*scope)->stmts;
        } else {
            std::cerr << "Invalid scope on else statement" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    auto stmt = m_allocator.alloc<NodeStmt>();
    stmt->var = stmt_if;
    return stmt;
}