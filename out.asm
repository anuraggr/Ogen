global _start
_start:
    mov rax, 3
    push rax
    mov rax, 1
    push rax
    mov rax, 1
    push rax
    mov rax, 2
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    mov rax, 1
    push rax
    push QWORD [rsp + 16]

    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jne end_if_0
    mov rax, 10
    push rax
    mov rax, 9
    push rax
    add rsp, 8
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    add rsp, 8
    end_if_0:
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
