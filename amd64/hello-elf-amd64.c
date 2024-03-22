/*  HELLO WORLD AS AN ELF FILE
    Ruben Toledo
    03 - 21 - 2024
---------------------------------------------------------------
    This program generates an ELF file that, when run,
        will execute the classic "Hello, World!" program
        
    Sources used:

        ELF Specification:
            https://refspecs.linuxfoundation.org/elf/elf.pdf
        
        System V ABI Specification:
            https://www.sco.com/developers/devspecs/gabi41.pdf
            
        System V ABI amd64 Supplement:
            https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf
            
        List of amd64 opcodes:
            http://ref.x86asm.net/coder64.html
            
        List of x86_64 Linux system calls:
            https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md
            
---------------------------------------------------------------
*/
        
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <elf.h>

Elf64_Ehdr CreateElfHeader();
Elf64_Phdr CreateProgramHeader();
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

    Elf64_Ehdr ehdr = CreateElfHeader();
    Elf64_Phdr phdr = CreateProgramHeader();

    ehdr.e_entry = phdr.p_vaddr + 0x0E;

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

Elf64_Ehdr CreateElfHeader() {
/*  Initiates an ELF header struct with MOST of the required info for a functional executable.

    The only missing value is e_entry, which specifies the address in virtual memory
        to which the kernel hands over control when executing.
    
    Its value is defined in the main function as an offset to p_vaddr in order to avoid
        making a section header for the "Hello, World!" string
*/
    Elf64_Ehdr ehdr;

    ehdr.e_ident[0] = 0x7F;                 // beginning of ELF magic
    ehdr.e_ident[1] = 'E';
    ehdr.e_ident[2] = 'L';
    ehdr.e_ident[3] = 'F';
    ehdr.e_ident[4] = ELFCLASS64;           // specify bit class 
    ehdr.e_ident[5] = ELFDATA2LSB;          // specify endianness (little)
    ehdr.e_ident[6] = EV_CURRENT;           // ELF version (always EV_CURRENT)
    ehdr.e_ident[7] = ELFOSABI_SYSV;        // OS ABI
    ehdr.e_ident[8] = 0x00;                 // ABI version (0x00 for sys v)

    for (int i = 9; i < sizeof(ehdr.e_ident); i++)
        ehdr.e_ident[i] = 0x00;             // padding :p

    ehdr.e_type = ET_EXEC;                  // specify this is an executable
    ehdr.e_machine = EM_X86_64;             // specify architecture
    ehdr.e_version = EV_CURRENT;            // ELF version
    // e_entry
    ehdr.e_phoff = sizeof(Elf64_Ehdr);      // program header table offset (immediately after the ELF header)
    ehdr.e_shoff = 0x00;                    // section header table offset (none)                    
    ehdr.e_flags = 0x00;                    // ABI flags. none for sys v
    ehdr.e_ehsize = sizeof(Elf64_Ehdr);     // ELF header size
    ehdr.e_phentsize = sizeof(Elf64_Phdr);  // program header size
    ehdr.e_phnum = 1;                       // number of program table entries
    ehdr.e_shentsize = sizeof(Elf64_Shdr);  // section header size
    ehdr.e_shnum = 0x00;                    // number of sections
    ehdr.e_shstrndx = 0x00;                 // section header string table offset

    return ehdr;
}

Elf64_Phdr CreateProgramHeader() {
/*  Initializes a program header struct with the required information for a functional
        program header.
    
    Most of these values are (of course) ABI and architecture dependent

    For clarification on the values I'm using, refer to:
        https://refspecs.linuxbase.org/elf/x86_64-abi-0.99.pdf
*/
    Elf64_Phdr phdr;

    phdr.p_type = PT_LOAD;                                      // this segment is loadable
    phdr.p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);    // program header offset
    phdr.p_vaddr = 0x0000000000400078;                          // virtual address. refer to system v amd64 abi supplement

    phdr.p_filesz = 0x46;                                       // size of the following segment
    phdr.p_memsz = phdr.p_filesz;                   

    phdr.p_flags = PF_X | PF_R;                                 // program flags. executable and readable
    phdr.p_align = 0x1000;                                      // program alignment

    return phdr;
}

void WriteMachineCode(FILE *fp) {
/*  Prepare and write the machine code that will be executed after the program header  */

    char msg[14] = "Hello, World!\n";

    char write_call[7] = {  // MOV  RAX, 1      ; write
        0x48,  0xC7, 0xC0,  0x01, 0x00, 0x00, 0x00
    };

    char write_stdout[10] = {  // MOV  RDI, 1   ; to stdout
        0x48,  0xBF,    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    char write_msg[10] = {  // MOV  RSI, &msg   ; load msg
        0x48,  0xBE,    0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00    
    };

    char write_msglen[10] = {  // MOV  RDX, msglen ; load msglen
        0x48,  0xBA,    0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    char exit_call[7] = {  // MOV  RAX, 60      ; exit
        0x48,  0xC7, 0xC0,  0x3C, 0x00, 0x00, 0x00
    };

    char exit_code[10] = {  // MOV  RDI, 0      ; load exit code
        0x48,  0xBF,    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    char syscall[2] = {   // syscall
        0x0F, 0x05
    };

    fwrite(msg, 1, sizeof(msg), fp);
    fwrite(write_call, 1, sizeof(write_call), fp);
    fwrite(write_stdout, 1, sizeof(write_stdout), fp);
    fwrite(write_msg, 1, sizeof(write_msg), fp);
    fwrite(write_msglen, 1, sizeof(write_msglen), fp);
    fwrite(syscall, 1, sizeof(syscall), fp);

    fwrite(exit_call, 1, sizeof(exit_call), fp);
    fwrite(exit_code, 1, sizeof(exit_code), fp);
    fwrite(syscall, 1, sizeof(syscall), fp);
}