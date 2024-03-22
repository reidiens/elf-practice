/*  HELLO WORLD AS AN ELF FILE
    Ruben Toledo
    03 - 21 - 2024
---------------------------------------------------------------
    This program generates an ELF file that, when run
        will execute the classic "Hello, World!" program
        
    Sources used:

        ELF Specification:
            https://refspecs.linuxfoundation.org/elf/elf.pdf

        System V ABI Specification:
            https://www.sco.com/developers/devspecs/gabi41.pdf 

        System V ABI i386 Supplement:
            https://www.sco.com/developers/devspecs/abi386-4.pdf

        Example ELF file that immediately returns 0:
            https://dacvs.neocities.org/1exit

        List of i386 opcodes:
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
void WriteMachineCode(FILE *fp);

int main(int argc, char *argv[]) {

    char *fname;

    if (argc >= 2) fname = argv[1];

    else {
        fprintf(stderr, "No file name provided.\n");
        return -1;    
    }

    FILE *efp = fopen(fname, "wb+");

    if (efp == NULL) {
        fprintf(stderr, "Could not open file\n");
        perror(strerror(errno));
        return -2;
    }

    Elf32_Ehdr ehdr = CreateElfHeader();
    Elf32_Phdr phdr = CreateProgramHeader();

    ehdr.e_entry = phdr.p_vaddr + 0xE;     // virtual memory entry point

    fwrite(&ehdr, sizeof(ehdr), 1, efp);
    fwrite(&phdr, sizeof(phdr), 1, efp);
    WriteMachineCode(efp); 
    
    if (fclose(efp) == EOF) {
        fprintf(stderr, "Could not close file stream (could not dump buffer)\n");
        perror(strerror(errno));
        return -3;
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

    Elf32_Ehdr ehdr;

    ehdr.e_ident[0] = 0x7F;                   // start of identifier " ELF"
    ehdr.e_ident[1] = 'E';            
    ehdr.e_ident[2] = 'L';        
    ehdr.e_ident[3] = 'F';        
    ehdr.e_ident[4] = ELFCLASS32;             // specify bit class
    ehdr.e_ident[5] = ELFDATA2LSB;            // specify endianness (little)
    ehdr.e_ident[6] = EV_CURRENT;             // ELF version (always EV_CURRENT)
    ehdr.e_ident[7] = ELFOSABI_SYSV;          // OS ABI
    ehdr.e_ident[8] = 0x0;                    // OS ABI version (always 0x0)
    
    for (int i = 9; i < sizeof(ehdr.e_ident); i++) 
        ehdr.e_ident[i] = 0x0;                // padding :p
            
    ehdr.e_type = ET_EXEC;                    // specify this is an executable
    ehdr.e_machine = EM_386;                  // specify machine architecture
    ehdr.e_version = EV_CURRENT;              // elf version. always EV_CURRENT
    // e_entry
    ehdr.e_phoff = sizeof(Elf32_Ehdr);        // program header table offset (immediately after the ELF header)
    ehdr.e_shoff = 0;                         // section header offset. 0 means no section header
    ehdr.e_flags = 0;                         // OS ABI flags. none for SYSV (Linux)
    ehdr.e_ehsize = sizeof(Elf32_Ehdr);       // ELF header size
    ehdr.e_phentsize = sizeof(Elf32_Phdr);    // size of program header
    ehdr.e_phnum = 1;                         // number of entries in the program header
    ehdr.e_shentsize = sizeof(Elf32_Shdr);    // section header entry size. 0x28 on 32-bit
    ehdr.e_shnum = 0;                         // number of entries in section header
    ehdr.e_shstrndx = 0;                      // section header string table offset

    return ehdr;
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

void WriteMachineCode(FILE *fp) {
/*  Prepare and write the machine code that goes after the program header  */

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

    char exit_code[5] = {
        0xBB,   0x00, 0x00, 0x00, 0x00      // MOV  EBX, 0      ; return value
    };

    char call[2] = { 
        0xCD, 0x80                                      // int 0x80         ; i386 equivalent of syscall 
    };          

    fwrite(msg, 1, sizeof(msg), fp);

    fwrite(write_call, 1, sizeof(write_call), fp);
    fwrite(write_stdout, 1, sizeof(write_stdout), fp);
    fwrite(write_msg, 1, sizeof(write_msg), fp);
    fwrite(write_msglen, 1, sizeof(write_msglen), fp);
    fwrite(call, 1, sizeof(call), fp);

    fwrite(exit_call, 1, sizeof(exit_call), fp);
    fwrite(exit_code, 1, sizeof(exit_code), fp);
    fwrite(call, 1, sizeof(call), fp);
}