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

    Elf32_Off selection_header_offset = elf_header.e_shoff;
    size_t headers_entry_size = elf_header.e_shentsize;
    int headers_entry_num = elf_header.e_shnum;

    printf("====== Reading ELF File ======\n");
    printf("e_shoff: %d \n", selection_header_offset);
    printf("e_shentsize: %ld\t e_shnum: %d \n", headers_entry_size, headers_entry_num);
}