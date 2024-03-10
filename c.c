#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <elf.h>

Elf32_Ehdr CreateElfHeader();

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

    fwrite(&e_head, sizeof(e_head), 1, efp);
    
    if (fclose(efp) == EOF) {
        fprintf(stderr, "Could not close file stream (could not dump buffer)\n");
        perror(strerror(errno));
        return errno;
    }

    printf("Created ELF file \"%s\" successfully :)\n", fname);
    puts("(as of this build the file isnt fully functional)\n");
    return 0;
}

Elf32_Ehdr CreateElfHeader() {
    /* initiates a struct containing SOME of the info that goes in the ELF header.

        missing entries:
            Elf32_addr  e_entry     - Address of the entry point, where the program starts execution
            e_phoff                 - Program header table offset
            e_phentsize             - Size of the individual entries in the program header
            e_phnum                 - Number of entries in teh program header
            e_shstrndx              - String table offset. idk what this is and i'm too tired to figure it out rn
    
        these are to be filled in later when the information is known
    */    


    Elf32_Ehdr e_head;

    e_head.e_ident[0] = 0x7F;           // start of identifier " ELF"
    e_head.e_ident[1] = 'E';    
    e_head.e_ident[2] = 'L';
    e_head.e_ident[3] = 'F';
    e_head.e_ident[4] = ELFCLASS32;     // define 32-bit architecture
    e_head.e_ident[5] = ELFDATA2LSB;    // specify endianness (little)
    e_head.e_ident[6] = EV_CURRENT;     // ELF version (always EV_CURRENT)
    e_head.e_ident[7] = ELFOSABI_SYSV;  // OS ABI
    e_head.e_ident[8] = 0x0;            // OS ABI version (always 0x0)
    
    for (int i = 9; i < sizeof(e_head.e_ident); i++) 
        e_head.e_ident[i] = 0x0;        // padding :p
    
    e_head.e_type = ET_DYN;             // specify this is a dynamic file
    e_head.e_machine = EM_X86_64;       // specify machine architecture
    e_head.e_version = EV_CURRENT;      // elf version. always EV_CURRENT
    e_head.e_shoff = 0;                 // section header offset. 0 means no section header
    e_head.e_shentsize = 0;             // section header entry size
    e_head.e_shnum = 0;                 // number of entries in section header
    
    return e_head;
}
