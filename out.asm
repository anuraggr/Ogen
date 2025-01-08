global _start
_start:
    mov rax, 3
    push rax
    push QWORD [rsp + 0]

    mov rax, 4
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    jl end_if_0
    mov rax, 1
    push rax
    push QWORD [rsp + 8]

    pop rax
    pop rbx
    add rax, rbx
    push rax
    end_if_0:
    push QWORD [rsp + 0]

    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
