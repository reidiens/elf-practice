#include <stdio.h>
#include <elf.h>

FILE* CreateElfMagic(const char *fname);

int main() {

    FILE *efp = CreateElfMagic("elf-file");
    if (efp == NULL) {
        fprintf(stderr, "Could not open file for writing");
        fclose(efp);
        return -1;
    }

    fclose(efp);
    return 0;
}

FILE* CreateElfMagic(const char *fname) {
    FILE *fp = fopen(fname, "wb+");
    if (fp == NULL) return NULL;

    unsigned char e_ident[16] = {
        0x7F, 'E', 'L', 'F',        // elf identifier
        ELFCLASS32,                             // specify the bitwidth of the machine
        ELFDATA2LSB,                            // specify endianness
        EV_CURRENT,                             // specify elf version (always EV_CURRENT)
        ELFOSABI_SYSV,                          // specify OS ABI
        0x0,                                    // ABI version (always 0x0)
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0   // padding
    }; 

    fwrite(e_ident, sizeof(unsigned char), sizeof(e_ident), fp);

    return fp;
}