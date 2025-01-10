global _start
_start:
    mov rax, 5
    push rax
    start_while_0:
    push QWORD [rsp + 0]

    mov rax, 5
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jle end_while_1
    mov rax, 6
    push rax
    mov rax, 7
    push rax
    mov rax, 60
    pop rdi
    syscall
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
