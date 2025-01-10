global _start
_start:
    mov rax, 4
    push rax
    push QWORD [rsp + 0]

    mov rax, 5
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jne end_if_0
    mov rax, 100
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_1
    end_if_0:
    push QWORD [rsp + 0]

    mov rax, 4
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jne end_elif_2
    push QWORD [rsp + 0]

    mov rax, 4
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    je end_if_3
    mov rax, 74
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_4
    end_if_3:
    add rsp, 0
    end_if_else_4:
    mov rax, 9
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
