global _start
_start:
    mov rax, 5
    push rax
    push QWORD [rsp + 0]

    pop rax
    cmp rax, 0
    je end_if_0
    mov rax, 100
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_1
    end_if_0:
    mov rax, 1
    push rax
    pop rax
    cmp rax, 0
    je end_elif_2
    mov rax, 74
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_1
    end_elif_2:
    mov rax, 99
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    end_if_else_1:
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
