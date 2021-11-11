#include <proc.h>
#include <elf.h>


#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define ELF_Off  Elf64_Off

#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define ELF_Off  Elf32_Off
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

size_t ramdisk_write(const void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf_header;
  ramdisk_read(&elf_header, 0, sizeof(elf_header));
  //根据小端法 0x7F E L F
  assert(*(uint32_t *)elf_header.e_ident == 0x464c457f);
  
  ELF_Off program_header_offset = elf_header.e_phoff;
  size_t headers_entry_size = elf_header.e_phentsize;
  int headers_entry_num = elf_header.e_phnum;

  for (int i = 0; i < headers_entry_num; ++i){
    Elf_Phdr section_entry;
    ramdisk_read(&section_entry, 
      i * headers_entry_size + program_header_offset, sizeof(elf_header));
    void * virt_addr;
    switch (section_entry.p_type) {
    case PT_LOAD:
      virt_addr = (void *)section_entry.p_vaddr; 
      ramdisk_read(virt_addr, section_entry.p_offset, section_entry.p_filesz);
      memset(virt_addr + section_entry.p_filesz, 0, 
        section_entry.p_memsz - section_entry.p_filesz);
      break;
    
    default:
      break;
    }

  }
  
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

