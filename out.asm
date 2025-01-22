global _start
_start:
    mov rax, 1
    push rax
    start_while_0:
    push QWORD [rsp + 0]

    mov rax, 11
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jge end_while_1
    mov rax, 1
    push rax
    push QWORD [rsp + 8]

    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    mov QWORD [rsp + 0], rax
    add rsp, 0
    jmp start_while_0
    end_while_1:
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
