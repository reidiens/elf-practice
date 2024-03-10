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
        return errno;
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
        return errno;
    }

    printf("Created ELF file \"%s\" successfully :)\n", fname);
    return 0;
}

Elf64_Ehdr CreateElfHeader() {
    Elf64_Ehdr ehdr;
    ehdr.e_ident[0] = 0x7F;
    ehdr.e_ident[1] = 'E';
    ehdr.e_ident[2] = 'L';
    ehdr.e_ident[3] = 'F';
    ehdr.e_ident[4] = ELFCLASS64;
    ehdr.e_ident[5] = ELFDATA2LSB;
    ehdr.e_ident[6] = EV_CURRENT;
    ehdr.e_ident[7] = ELFOSABI_SYSV;
    ehdr.e_ident[8] = 0x00;

    for (int i = 9; i < sizeof(ehdr.e_ident); i++)
        ehdr.e_ident[i] = 0x00;

    ehdr.e_type = ET_EXEC;
    ehdr.e_machine = EM_X86_64;
    ehdr.e_version = EV_CURRENT;
    // e_entry
    ehdr.e_phoff = sizeof(Elf64_Ehdr);
    ehdr.e_shoff = 0x00;
    ehdr.e_flags = 0x00;
    ehdr.e_ehsize = sizeof(Elf64_Ehdr);
    ehdr.e_phentsize = sizeof(Elf64_Phdr);
    ehdr.e_phnum = 1;
    ehdr.e_shentsize = sizeof(Elf64_Shdr);
    ehdr.e_shnum = 0x00;
    ehdr.e_shstrndx = 0x00;

    return ehdr;
}

Elf64_Phdr CreateProgramHeader() {
    Elf64_Phdr phdr;
    phdr.p_type = PT_LOAD;
    phdr.p_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
    phdr.p_vaddr = 0x0000000000400078;

    // phdr.p_filesz =
    // phdr.p_memsz = 

    phdr.p_flags = PF_X | PF_R;
    phdr.p_align = 0x1000;

    return phdr;
}

void WriteMachineCode(FILE *fp) {
    char msg[14] = "Hello, World!\n";

    char write_call[7] = {
        0x48,  0xC7, 0xC0,  0x01, 0x00, 0x00, 0x00
    };

    char write_stdout[10] = {
        0x48,  0xBF,    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    char write_msg[10] = {
        0x48,  0xBE,    0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00    
    };

    char write_msglen[10] = {
        0x48,  0xBA,    0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    char exit_call[7] = {
        0x48,  0xC7, 0xC0,  0x3C, 0x00, 0x00, 0x00
    };

    char exit_code[10] = {
        0x48,  0xBF,    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    char syscall[2] = { 
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