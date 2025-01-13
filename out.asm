global _start
_start:
    mov rax, 1
    push rax
    mov rax, 10
    push rax
    pop rax
    mov QWORD [rsp + 0], rax
    start_for_0:
    push QWORD [rsp + 0]

    mov rax, 100
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jge end_for_1
    push QWORD [rsp + 0]

    mov rax, 90
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jle end_if_2
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp end_if_else_3
    end_if_2:
    add rsp, 0
    end_if_else_3:
    add rsp, 0
    mov rax, 1
    push rax
    push QWORD [rsp + 8]

    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    mov QWORD [rsp + 0], rax
    jmp start_for_0
    end_for_1:
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
