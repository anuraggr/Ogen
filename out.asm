global _start
_start:
    mov rax, 3
    push rax
    push QWORD [rsp + 0]

    mov rax, 10
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jne end_if_0
    mov rax, 9
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_1
    end_if_0:
    add rsp, 0
    end_if_else_1:
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
