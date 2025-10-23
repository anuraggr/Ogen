section .text
_print_int:
    push rbp          ; 1. Save the old base pointer
    mov rbp, rsp      ; 2. Set our new base pointer (this saves the stack position)
    mov rbx, 10       ; Divisor
    cmp rax, 0
    jge .to_str_loop
    push '-'          ; Push '-' for negative numbers
    neg rax           ; Make rax positive
.to_str_loop:
    xor rdx, rdx      ; Clear rdx for division
    div rbx           ; rax = rax / 10, rdx = remainder
    add rdx, '0'      ; Convert digit to ASCII
    push rdx          ; Push ASCII digit onto stack
    test rax, rax     ; Is quotient zero?
    jnz .to_str_loop  ; If not, loop again
.print:
    mov rax, 1        ; sys_write syscall
    mov rdi, 1        ; stdout file descriptor
    mov rsi, rsp      ; Address of string to print
    mov rdx, rbp      ; Calculate length using our saved base pointer
    sub rdx, rsp
    syscall           ; This clobbers rcx, but we are using rbp, so it's OK
    mov rsp, rbp      ; 3. Restore stack pointer from our saved value
    pop rbp           ; 4. Restore the old base pointer
    ret

_print_newline:
    mov rax, 1
    mov rdi, 1
    mov rsi, 0xA
    push rsi
    mov rsi, rsp
    mov rdx, 1
    syscall
    pop rsi
    ret

add:
    push rbp
    mov rbp, rsp
    push QWORD [rbp + 24]
    push QWORD [rbp + 16]
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    add rsp, 16
    mov rsp, rbp
    pop rbp
    ret
global _start

_start:
    push rbp
    mov rbp, rsp
    mov rax, 10
    push rax
    mov rax, 20
    push rax
    mov rax, 1
    push rax
    push QWORD [rbp - 8]
    push QWORD [rbp - 16]
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    call _print_int
    call _print_newline
    mov rax, 1
    push rax
    mov rax, 10
    push rax
    call add
    add rsp, 16
    push rax
    mov rax, 100
    push rax
    call add
    add rsp, 16
    push rax
    pop rax
    call _print_int
    call _print_newline
    mov rax, 10
    push rax
    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
