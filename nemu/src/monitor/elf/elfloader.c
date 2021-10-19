#include <elf.h>
#include <stdio.h>

#include "elfloader.h"

// typedef Elf32_Shdr Elf_Shdr;
// typedef Elf32_Ehdr Elf_Ehdr;



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

    Elf32_Off section_header_offset = elf_header.e_shoff;
    size_t headers_entry_size = elf_header.e_shentsize;
    int headers_entry_num = elf_header.e_shnum;

    printf("====== Reading ELF File ======\n");
    printf("e_shoff: %d \n", section_header_offset);
    printf("e_shentsize: %ld\t e_shnum: %d \n", headers_entry_size, headers_entry_num);
    
    assert(sizeof(Elf32_Shdr) == headers_entry_size);
    
    printf("====== Selection Headers ======\n");
    
    Elf32_Off sh_string_table_offset, symbol_table_offset, string_table_offset;
    for (int i = 0; i < headers_entry_num; ++i){
        Elf32_Shdr section_entry;
        read_from_file(elf, i * headers_entry_size + section_header_offset,
            headers_entry_size, &section_entry);
        switch(section_entry.sh_type){
            case SHT_SYMTAB:
                symbol_table_offset = section_entry.sh_offset;
            break;

            case SHT_STRTAB:
                if (i == elf_header.e_shstrndx)
                    sh_string_table_offset = section_entry.sh_offset;
                else
                    string_table_offset = section_entry.sh_offset;
            break;
        }
    }

    printf("String Table Offset: %#x\n", string_table_offset);
    printf("shstr  Table Offset: %#x\n", sh_string_table_offset);
    printf("Symbol Table Offset: %#x\n", symbol_table_offset);
    

}