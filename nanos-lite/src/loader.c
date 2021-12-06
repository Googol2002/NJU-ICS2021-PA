#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define ELF_Off  Elf64_Off

#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define ELF_Off  Elf32_Off
#endif

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static void read(int fd, void *buf, size_t offset, size_t len){
  fs_lseek(fd, offset, SEEK_SET);
  fs_read(fd, buf, len);
}

// static void write(int fd, const void *buf, size_t offset, size_t len){

// }

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  
  Elf_Ehdr elf_header;
  read(fd, &elf_header, 0, sizeof(elf_header));
  //根据小端法 0x7F E L F
  assert(*(uint32_t *)elf_header.e_ident == 0x464c457f);
  
  ELF_Off program_header_offset = elf_header.e_phoff;
  size_t headers_entry_size = elf_header.e_phentsize;
  int headers_entry_num = elf_header.e_phnum;

  for (int i = 0; i < headers_entry_num; ++i){
    Elf_Phdr section_entry;
    read(fd, &section_entry, 
      i * headers_entry_size + program_header_offset, sizeof(elf_header));
    void * virt_addr;
    switch (section_entry.p_type) {
    case PT_LOAD:
      virt_addr = (void *)section_entry.p_vaddr; 
      read(fd, virt_addr, section_entry.p_offset, section_entry.p_filesz);
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
  assert(0);
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg){
  Area karea;
  karea.start = &pcb->cp;
  karea.end = &pcb->cp + STACK_SIZE;

  pcb->cp = kcontext(karea, entry, arg);
}

static size_t ceil_4_bytes(size_t size){
  if (size & 0x3)
    return (size & (~0x3)) + 0x4;
  return size;
}

#define NR_PAGE 8

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  uintptr_t entry = loader(pcb, filename);

  Area karea;
  karea.start = &pcb->cp;
  karea.end = &pcb->cp + STACK_SIZE;

  Context* context = ucontext(NULL, karea, (void *)entry);
  pcb->cp = context;

  int envc = 0, argc = 0;
  if (envp){
    for (; envp[envc]; ++envc){}
  }
  if (argv){
    for (; argv[argc]; ++argc){}
  }
  if (argv){
    for (int i = 0; argv[i]; ++i){
      printf("arg:%s\n", argv[i]);
    }
  }
  // if (envp){
  //   for (int i = 0; envp[i]; ++i){
  //     printf("env:%s\n", envp[i]);
  //   }
  // }
  printf("I'm OK, %d, %x\n", argc, &envp);

  char *envp_ustack[envc];
  char *brk = (char *)new_page(NR_PAGE);
  
  // 拷贝字符区
  for (int i = 0; i < envc; ++i){
    brk -= (ceil_4_bytes(strlen(envp[i]) + 1)); // 分配大小
    envp_ustack[i] = brk;
    strcpy(brk, envp[i]);
  }

  char *argv_ustack[envc];
  for (int i = 0; i < argc; ++i){
    brk -= (ceil_4_bytes(strlen(argv[i]) + 1)); // 分配大小
    argv_ustack[i] = brk;
    strcpy(brk, argv[i]);
  }
  
  intptr_t *ptr_brk = (intptr_t *)(brk);
  
  // 分配envp空间
  ptr_brk -= 1;
  *ptr_brk = 0;
  ptr_brk -= envc;
  for (int i = 0; i < envc; ++i){
    ptr_brk[i] = (intptr_t)(envp_ustack[i]);
  }

  // 分配argv空间
  ptr_brk -= 1;
  *ptr_brk = 0;
  ptr_brk -= argc;
  for (int i = 0; i < argc; ++i){
    ptr_brk[i] = (intptr_t)(argv_ustack[i]);
  }

  ptr_brk -= 1;
  *ptr_brk = argc;

  context->GPRx = (intptr_t)ptr_brk;
}
