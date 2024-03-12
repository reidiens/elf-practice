global _start

section .data
    fname   db  "file",0

    msg1    db "creating file...",0xA
    msg1len equ $ - msg1

    msg2    db "file created!", 0xA
    msg2len equ $ - msg2

    err     db "file not opened.",0xA
    errlen  equ $ - err

    EXIT    equ 0x01
    WRITE   equ 0x04
    OPEN    equ 0x05

    STDOUT  equ 0x01

    WRONLY  equ 1o
    CREAT   equ 100o
    TRUNC   equ 1000o

section .text

_start:
    mov     eax, WRITE
    mov     ebx, STDOUT
    mov     ecx, msg1
    mov     edx, msg1len
    int 80h

    mov     eax, OPEN
    mov     ebx, fname
    mov     ecx, CREAT+WRONLY
    or      ecx, CREAT
    int 80h

    mov     esi, eax
    cmp     esi, -1
    je      error

    mov     eax, WRITE
    mov     ebx, STDOUT
    mov     ecx, msg2
    mov     edx, msg2len
    int 80h

ret:
    mov     eax, 0x01
    mov     ebx, 0x00
    int 80h

error:
    mov     eax, WRITE
    mov     ebx, STDOUT
    mov     ecx, err
    mov     edx, errlen
    int 80h

    goto ret


