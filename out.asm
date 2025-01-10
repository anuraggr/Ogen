global _start
_start:
    mov rax, 3
    push rax
    push QWORD [rsp + 0]

    mov rax, 3
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jne end_if_0
    push QWORD [rsp + 0]

    mov rax, 4
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jne end_if_2
    mov rax, 110
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_3
    end_if_2:
    add rsp, 0
    end_if_else_3:
    mov rax, 101
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_1
    end_if_0:
    push QWORD [rsp + 0]

    mov rax, 100
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jne end_if_4
    mov rax, 100
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_5
    end_if_4:
    mov rax, 20
    push rax
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    add rsp, 8
    end_if_else_5:
    add rsp, 0
    end_if_else_1:
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
