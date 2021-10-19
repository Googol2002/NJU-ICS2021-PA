#include <elf.h>
#include <stdio.h>

#include "elfloader.h"

static void read_from_file(FILE *elf, size_t offset, size_t size, void* dest){
    fseek(elf, offset, SEEK_SET);
    int flag = fread(dest, size, 1, elf);
    assert(flag == 1);
}

void init_elf(const char* path){
    FILE *elf = fopen(path, "rb");
    assert(elf != NULL);
    Elf32_Ehdr elf_header;
    read_from_file(elf, 0, sizeof elf_header, &elf_header);

    printf("====== Reading ELF File ======\n");
    printf("e_shoff: %x \n", elf_header.e_shoff);
    printf("e_shentsize: %d\t e_shnum: %d \n", elf_header.e_shentsize, elf_header.e_shnum);
}