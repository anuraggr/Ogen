#pragma once

#include "parser.hpp"
#include <cassert>
#include <algorithm>


class Generator {
public:
    inline explicit Generator(NodeProg prog)
        : m_prog(std::move(prog))
    {
    }

    void gen_term(const NodeTerm* term) {
        struct TermVisitor {
            Generator* gen;
            void operator()(const NodeTermIntLit* term_int_lit) const {
                gen->m_output << "    mov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen->push("rax");
            }
            void operator()(const NodeTermIdent* term_ident) const {
                auto it = std::find_if(gen->m_vars.begin(), gen->m_vars.end(), [&](const auto& var) {
                    return var.name == term_ident->ident.value.value();
                });
                if(it == gen->m_vars.end()){
                    std::cerr << "Identifier decleared in scope: " << term_ident->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack_size - (*it).stack_loc - 1) * 8 << "]\n";
                gen->push(offset.str());
            }

            void operator()(const NodeTermParen* term_paren) const {
                gen->gen_expr(term_paren->expr);
            }
        };
        TermVisitor visitor({.gen = this});
        std::visit(visitor, term->var);
    }


    void gen_bin_expr(const NodeBinExpr* bin_expr)
    {
        struct BinExprVisitor {
            Generator* gen;
            void operator()(const NodeBinExprSub* sub) const {
                gen->gen_expr(sub->rhs);
                gen->gen_expr(sub->lhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "    sub rax, rbx\n";
                gen->push("rax");
            }
            void operator()(const NodeBinExprAdd* add) const {
                gen->gen_expr(add->rhs);
                gen->gen_expr(add->lhs);
                
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "    add rax, rbx\n";
                gen->push("rax");
            }

            void operator()(const NodeBinExprMulti* multi) const {
                gen->gen_expr(multi->rhs);
                gen->gen_expr(multi->lhs);
                
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "    mul rbx\n";
                gen->push("rax");
            }

            void operator()(const NodeBinExprDiv* div) const {
                gen->gen_expr(div->rhs);
                gen->gen_expr(div->lhs);
                
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "    mov rdx, 0\n";
                gen->m_output << "    div rbx\n";
                gen->push("rax");
            }
        };

        BinExprVisitor visitor { .gen = this };
        std::visit(visitor, bin_expr->var);
    }

    void gen_expr(const NodeExpr* expr)
    {
        struct ExprVisitor {
            Generator* gen;
            void operator()(const NodeTerm* term) const
            {
                gen->gen_term(term);
            }
            void operator()(const NodeBinExpr* bin_expr) const
            {
                gen->gen_bin_expr(bin_expr);
            }
        };

        ExprVisitor visitor { .gen = this };
        std::visit(visitor, expr->var);
    }

//func to handle elif in if statement
    void resolveElif(const NodeStmtIf *if_condition, const std::string &end_if_else,
                     Generator *gen, const std::string &end_label, const std::string &elif_end_label) {

        for (const NodeStmt *elif_stmt : if_condition->elif_body) {
            const NodeStmtIf *elif_condition = std::get<NodeStmtIf *>(elif_stmt->var);
            std::string elif_end_label = gen->generate_label("end_elif");
            if(!elif_condition->rhs){
                gen->gen_expr(elif_condition->lhs);
                gen->pop("rax");
                gen->m_output << "    cmp rax, 0\n";
                gen->m_output << "    je " << elif_end_label << "\n";
                gen->begin_scope();
                for (const NodeStmt *stmt : elif_condition->body) {
                    gen->gen_stmt(stmt);
                }
                gen->end_scope();
                gen->m_output << "    jmp " << end_if_else << "\n";
                gen->m_output << "    " << elif_end_label << ":\n";
            }
            else{
                gen->gen_expr(elif_condition->lhs);
                gen->gen_expr(elif_condition->rhs);
                gen->pop("rbx");
                gen->pop("rax");
                gen->m_output << "    cmp rax, rbx\n";
                if (elif_condition->comparison->comp.type == TokenType::eq_eq) {
                    gen->m_output << "    jne " << elif_end_label << "\n";
                } else if (elif_condition->comparison->comp.type == TokenType::greater_than) {
                    gen->m_output << "    jle " << elif_end_label << "\n";
                } else if (elif_condition->comparison->comp.type == TokenType::less_than) {
                    gen->m_output << "    jge " << elif_end_label << "\n";
                } else if (elif_condition->comparison->comp.type == TokenType::greater_eq) {
                    gen->m_output << "    jl " << elif_end_label << "\n";
                } else if (elif_condition->comparison->comp.type == TokenType::less_eq) {
                    gen->m_output << "    jg " << elif_end_label << "\n";
                } else if (elif_condition->comparison->comp.type == TokenType::n_eq) {
                    gen->m_output << "    je " << elif_end_label << "\n";
                } else {
                    std::cerr << "Invalid comparison" << std::endl;
                    exit(EXIT_FAILURE);
                }

                gen->begin_scope();
                for (const NodeStmt *stmt : elif_condition->body) {
                    gen->gen_stmt(stmt);
                }
                gen->end_scope();

                gen->m_output << "    jmp " << end_if_else << "\n"; // jump to end of else cuz elif condition is true

                gen->m_output << "    " << elif_end_label << ":\n"; // end of elif
            }
        }
    }

    void gen_stmt(const NodeStmt* stmt)
    {
        struct StmtVisitor {
            Generator* gen;
            void operator()(const NodeStmtExit* stmt_exit) const
            {
                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "    mov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "    syscall\n";
            }
            void operator()(const NodeStmtLet* stmt_let) const
            {
                auto it = std::find_if(gen->m_vars.begin(), gen->m_vars.end(), [&](const auto& var) {
                    return var.name == stmt_let->ident.value.value();
                });
                if (it != gen->m_vars.cend()) {
                    std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.push_back({ .name = stmt_let->ident.value.value(), .stack_loc = gen->m_stack_size });
                gen->gen_expr(stmt_let->expr);
            }
            void operator()(const NodeStmtScope* scope) const
            {
                gen->begin_scope();
                for (const NodeStmt* stmt : scope->stmts) {
                    gen->gen_stmt(stmt);
                }
                gen->end_scope();
            } 
            void operator()(const NodeStmtIf* if_condition) const
            {
                std::cout << "If statement" << std::endl; //debug
                std::string end_label = gen->generate_label("end_if");
                std::string end_if_else = gen->generate_label("end_if_else");
                if(!if_condition->rhs){
                    gen->gen_expr(if_condition->lhs);
                    gen->pop("rax");
                    gen->m_output << "    cmp rax, 0\n";
                    gen->m_output << "    je " << end_label << "\n";
                    gen->begin_scope();
                    for(const NodeStmt* stmt : if_condition->body){
                        gen->gen_stmt(stmt);
                    }
                    gen->end_scope();
                    gen->m_output << "    jmp " << end_if_else << "\n";
                    gen->m_output << "    " << end_label << ":\n";
                    
                    gen->resolveElif(if_condition, end_if_else, gen, end_label, "");

                    gen->begin_scope();
                    for (const NodeStmt *stmt : if_condition->else_body) {
                        gen->gen_stmt(stmt);
                    }
                    gen->end_scope();

                }
                else{
                    gen->gen_expr(if_condition->lhs);
                    gen->gen_expr(if_condition->rhs);
                    gen->pop("rbx");
                    gen->pop("rax");
                    gen->m_output << "    cmp rax, rbx\n";

                    
                    if(if_condition->comparison->comp.type == TokenType::eq_eq){
                        gen->m_output << "    jne " << end_label << "\n";
                    }
                    else if(if_condition->comparison->comp.type == TokenType::greater_than){
                        gen->m_output << "    jle " << end_label << "\n";
                    }
                    else if(if_condition->comparison->comp.type == TokenType::less_than){
                        gen->m_output << "    jge " << end_label << "\n";
                    }
                    else if(if_condition->comparison->comp.type == TokenType::greater_eq){
                        gen->m_output << "    jl " << end_label << "\n";
                    }
                    else if(if_condition->comparison->comp.type == TokenType::less_eq){
                        gen->m_output << "    jg " << end_label << "\n";
                    }
                    else if(if_condition->comparison->comp.type == TokenType::n_eq){
                        gen->m_output << "    je " << end_label << "\n";
                    }
                    else{
                        std::cerr << "Invalid comparison" << std::endl;
                        exit(EXIT_FAILURE);
                    }

                    gen->begin_scope();
                    for(const NodeStmt* stmt : if_condition->body){
                        gen->gen_stmt(stmt);
                    }
                    gen->end_scope();

                    gen->m_output << "    jmp " << end_if_else << "\n";         //jump to end of else cuz if condition is true
                    
                    gen->m_output << "    " <<  end_label << ":\n";             //end of if

                    gen->resolveElif(if_condition, end_if_else, gen, end_label, "");

                    gen->begin_scope();
                    for(const NodeStmt* stmt : if_condition->else_body){
                        gen->gen_stmt(stmt);
                    }
                    gen->end_scope();
                    
                }
                gen->m_output << "    " << end_if_else << ":\n"; // end of else
    }
        };

        StmtVisitor visitor { .gen = this };
        std::visit(visitor, stmt->var);
    }

    [[nodiscard]] std::string gen_prog()
    {
        m_output << "global _start\n_start:\n";

        for (const NodeStmt* stmt : m_prog.stmts) {
            gen_stmt(stmt);
        }

        m_output << "    mov rax, 60\n";
        m_output << "    mov rdi, 0\n";
        m_output << "    syscall\n";
        return m_output.str();
    }

private:
    void push(const std::string& reg)
    {
        m_output << "    push " << reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string& reg)
    {
        m_output << "    pop " << reg << "\n";
        m_stack_size--;
    }

    void begin_scope()
    {
        m_scopes.push_back(m_vars.size());
    }
    void end_scope()
    {
        size_t pop_count = m_vars.size() - m_scopes.back();
        m_output << "    add rsp, " << pop_count * 8 << "\n";
        m_stack_size -= pop_count;
        for(int i = 0; i<pop_count; i++){
            m_vars.pop_back();
        }
        m_scopes.pop_back();
    }

    struct Var {
        std::string name;
        size_t stack_loc;
    };

    std::string generate_label(const std::string& base) {
        static int label_counter = 0;
        return base + "_" + std::to_string(label_counter++);
    }

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::vector<Var> m_vars {};
    std::vector<size_t> m_scopes {};
};