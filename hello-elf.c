#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <elf.h>

Elf32_Ehdr CreateElfHeader();
Elf32_Phdr CreateProgramHeader();

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

    e_head.e_entry = 0;     // virtual memory entry point. 0 more than likely results in a seg fault. Or worse!

    fwrite(&e_head, sizeof(e_head), 1, efp);
    fwrite(&phdr, sizeof(phdr), 1, efp);
    
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
/*  Initializes an ELF header struct with MOST of the info needed for a functional
        ELF file.

    The only missing element is e_entry, which specifies the starting address in virtual memory to which the 
        kernel hands over control when executing.
    
    This is to be filled in in some other function. I'm too lazy to do it right now.
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
            
    e_head.e_type = ET_DYN;                     // specify this is a dynamic file
    e_head.e_machine = EM_X86_64;               // specify machine architecture
    e_head.e_version = EV_CURRENT;              // elf version. always EV_CURRENT
    // e_entry
    e_head.e_phoff = sizeof(Elf32_Ehdr);        // program header table offset (immediately after the ELF header)
    e_head.e_shoff = 0;                         // section header offset. 0 means no section header
    e_head.e_flags = 0;                         // OS ABI flags. none for SYSV (Linux)
    e_head.e_ehsize = sizeof(Elf32_Ehdr);       // ELF header size
    e_head.e_phentsize = sizeof(Elf32_Phdr);    // size of program header
    e_head.e_phnum = 1;                         // number of entries in the program header
    e_head.e_shentsize = 0;                     // section header entry size
    e_head.e_shnum = 0;                         // number of entries in section header
    e_head.e_shstrndx = 0;                      // i have no idea what this is rn but it's ok to leave it as 0 :D

    return e_head;
}

Elf32_Phdr CreateProgramHeader() {
/*  Initializes a program header struct containing SOME of the info
        necessary for a program header.

    The missing entries are:
        p_vaddr         - Virtual memory address of the first segment
        p_paddr         - Physical memory address of the first segment
        p_filesz        - Size of the whole file image in bytes
        p_memsz         - Size the file takes up in memory (filesize)

    I'll figure how to implement those things laterrrrr
*/

    Elf32_Phdr phdr;

    phdr.p_type = PT_LOAD;                      // specify that this program is loadable
    phdr.p_offset = 0;                          // offset from beginning of file for first segment. can be 0

    // missing vals go here //

    phdr.p_flags = PF_X | PF_R;                 // this segment is execuatble and readable
    phdr.p_align = 0x1000;                      // value to which segments are aligned to in memory

    return phdr;
}