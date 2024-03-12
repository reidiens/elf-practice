global _start

section .data
    fname   db  "file",0                ; temp var

    msg1    db "creating file...",0xA   
    msg1len equ $ - msg1

    msg2    db "file created!", 0xA
    msg2len equ $ - msg2

    fmsg    db "still figuring it out...",0xA
    fmsglen equ $ - fmsg

    err     db "file not created.",0xA
    errlen  equ $ - err

    EXIT    equ 0x01
    WRITE   equ 0x04
    CREAT   equ 0x08
    OPEN    equ 0x05
    CLOSE   equ 0x06

    STDOUT  equ 0x01

    perms   equ 0333o

section .text

_start:
    mov     eax, WRITE
    mov     ebx, STDOUT
    mov     ecx, msg1
    mov     edx, msg1len
    int 80h

    mov     eax, CREAT
    mov     ebx, fname
    mov     ecx, perms
    int 80h

    mov     eax, WRITE
    mov     ebx, esi
    mov     ecx, fmsg
    mov     edx, fmsglen
    int 80h 

    mov     eax, CLOSE
    mov     ebx, esi
    int 80h 

    mov     eax, 0x01
    mov     ebx, 0x00
    int 80h