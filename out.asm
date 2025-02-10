global _start
_start:
    mov rax, 10
    push rax
    mov rax, 20
    push rax
    push QWORD [rsp + 0]

    push QWORD [rsp + 16]

    pop rax
    pop rbx
    add rax, rbx
    push rax
    mov rax, 10
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    je end_if_0
    mov rax, 99
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_1
    end_if_0:
    add rsp, 0
    end_if_else_1:
    mov rax, 60
    mov rdi, 0
    syscall
