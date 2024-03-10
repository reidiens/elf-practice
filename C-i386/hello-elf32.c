/*  HELLO WORLD AS AN ELF FILE
    Ruben Toledo
    03-10-2024
---------------------------------------------------------------
    This program generates an ELF file that, when run
        will execute the classic "Hello, World!" program
        
    Sources used:
    
        System V ABI Specification:
            https://www.sco.com/developers/devspecs/gabi41.pdf 

        System V Abi i386 Supplement:
            https://www.sco.com/developers/devspecs/abi386-4.pdf

        Example ELF file that immediately returns 0:
            https://dacvs.neocities.org/1exit

        List of i286 opcodes:
            http://ref.x86asm.net/coder32.html#xB8

        List of x86 Linux system calls:
            https://syscalls.w3challs.com/?arch=x86

        i386 Programmer's Reference Manual:
            http://css.csail.mit.edu/6.858/2013/readings/i386.pdf

---------------------------------------------------------------
*/

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <elf.h>

Elf32_Ehdr CreateElfHeader();
Elf32_Phdr CreateProgramHeader();
unsigned char WriteMachineCode(FILE *fp);

int main(int argc, char *argv[]) {

    char *fname;

    if (argc >= 2)  
        fname = argv[1];
    else 
        fname = "elf-file";

    FILE *efp = fopen(fname, "wb+");

    if (efp == NULL) {
        fprintf(stderr, "Could not open file\n");
        perror(strerror(errno));
        return errno;
    }

    Elf32_Ehdr e_head = CreateElfHeader();
    Elf32_Phdr phdr = CreateProgramHeader();

    e_head.e_entry = phdr.p_vaddr + 0xE;     // virtual memory entry point

    fwrite(&e_head, sizeof(e_head), 1, efp);
    fwrite(&phdr, sizeof(phdr), 1, efp);
    
    if (!WriteMachineCode(efp)) {
        fprintf(stderr, "Could not write machine code to file");
        fclose(efp);
        return -1;
    }
    
    if (fclose(efp) == EOF) {
        fprintf(stderr, "Could not close file stream (could not dump buffer)\n");
        perror(strerror(errno));
        return errno;
    }

    printf("Created ELF file \"%s\" successfully :)\n", fname);
    return 0;
}

Elf32_Ehdr CreateElfHeader() {
/*  Initializes an ELF header struct with MOST of the info needed for a functional
        ELF file.

    The only missing element is e_entry, which specifies the starting address in virtual memory to which the 
        kernel hands over control when executing.
    
    Its value is offset from p_vaddr in this program to avoid making a section
        table for the "Hello, World!\n" string
*/

    Elf32_Ehdr e_head;

    e_head.e_ident[0] = 0x7F;                   // start of identifier " ELF"
    e_head.e_ident[1] = 'E';            
    e_head.e_ident[2] = 'L';        
    e_head.e_ident[3] = 'F';        
    e_head.e_ident[4] = ELFCLASS32;             // define 32-bit architecture
    e_head.e_ident[5] = ELFDATA2LSB;            // specify endianness (little)
    e_head.e_ident[6] = EV_CURRENT;             // ELF version (always EV_CURRENT)
    e_head.e_ident[7] = ELFOSABI_SYSV;          // OS ABI
    e_head.e_ident[8] = 0x0;                    // OS ABI version (always 0x0)
    
    for (int i = 9; i < sizeof(e_head.e_ident); i++) 
        e_head.e_ident[i] = 0x0;                // padding :p
            
    e_head.e_type = ET_EXEC;                    // specify this is an executable
    e_head.e_machine = EM_386;                  // specify machine architecture
    e_head.e_version = EV_CURRENT;              // elf version. always EV_CURRENT
    // e_entry
    e_head.e_phoff = sizeof(Elf32_Ehdr);        // program header table offset (immediately after the ELF header)
    e_head.e_shoff = 0;                         // section header offset. 0 means no section header
    e_head.e_flags = 0;                         // OS ABI flags. none for SYSV (Linux)
    e_head.e_ehsize = sizeof(Elf32_Ehdr);       // ELF header size
    e_head.e_phentsize = sizeof(Elf32_Phdr);    // size of program header
    e_head.e_phnum = 1;                         // number of entries in the program header
    e_head.e_shentsize = 0x28;                  // section header entry size. 0x28 on 32-bit
    e_head.e_shnum = 0;                         // number of entries in section header
    e_head.e_shstrndx = 0;                      // i have no idea what this is rn but it's ok to leave it as 0 :D

    return e_head;
}

Elf32_Phdr CreateProgramHeader() {
/*  Initializes a structure containing the information needed for the 
        program header

    Most of this stuff (of course) is architecture and ABI dependent.

    Refer to 
        https://www.sco.com/developers/devspecs/gabi41.pdf
    and 
        https://www.sco.com/developers/devspecs/abi386-4.pdf

    for more information on the values I'm using.
*/
    Elf32_Phdr phdr;

    phdr.p_type = PT_LOAD;                                      // specify that this program is loadable
    phdr.p_offset = sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);    // offset from beginning of file for first segment. can be 0
    phdr.p_vaddr = 0x08048054;                                  // virtual memory address. refer to system v ABI i386 supplement
    phdr.p_paddr = 0;                                           // unused in x86

    phdr.p_filesz = 0x2E;                       // size of the following segment 
    phdr.p_memsz = phdr.p_filesz;

    phdr.p_flags = PF_X | PF_R;                 // this segment is execuatble an// size of the following segmentd readable
    phdr.p_align = 0x1000;                      // value to which segments are aligned to in memory. 0x1000 for x86 

    return phdr;
}

unsigned char WriteMachineCode(FILE *fp) {
    if (fp == NULL) return 0;

    char msg[14] = "Hello, World!\n";                           // size = 14

    char write_call[5] = {
        0xB8,   0x04, 0x0, 0x0, 0x0         // MOV  EAX, 4      ; write
    };

    char write_stdout[5] = {
        0xBB,   0x01, 0x00, 0x00, 0x00      // MOV  EBX, 1      ; to stdout
    };

    char write_msg[5] = {
        0x0B9,  0x54, 0x80, 0x04, 0x08      // MOV  EBX, &msg      
    };

    char write_msglen[5] = {
        0xBA,   0x0E, 0x00, 0x00, 0x00      // MOV  ECX, 14
    };

    char exit_call[5] = {
        0xB8,   0x01, 0x00, 0x00, 0x00      // MOV  EAX, 1      ; exit
    };

    char errcode[5] = {
        0xBB,   0x00, 0x00, 0x00, 0x00      // MOV  EBX, 0      ; return value
    };

    char call[2] = { 0xCD, 0x80 };                      // int 0x80         ; i386 equivalent of syscall

    fwrite(msg, 1, sizeof(msg), fp);

    fwrite(write_call, 1, sizeof(write_call), fp);
    fwrite(write_stdout, 1, sizeof(write_stdout), fp);
    fwrite(write_msg, 1, sizeof(write_msg), fp);
    fwrite(write_msglen, 1, sizeof(write_msglen), fp);
    fwrite(call, 1, sizeof(call), fp);

    fwrite(exit_call, 1, sizeof(exit_call), fp);
    fwrite(errcode, 1, sizeof(errcode), fp);
    fwrite(call, 1, sizeof(call), fp);
    
    return 1;
}