global _start
_start:
    mov rax, 10
    push rax
    start_while_0:
    push QWORD [rsp + 0]

    mov rax, 3
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jl end_while_1
    mov rax, 10
    push rax
    mov rax, 1
    push rax
    push QWORD [rsp + 16]

    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rax
    mov QWORD [rsp + 8], rax
    add rsp, 8
    jmp start_while_0
    end_while_1:
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
