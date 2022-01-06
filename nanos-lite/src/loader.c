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


#define NR_PAGE 8
#define PAGESIZE 4096

 __attribute__ ((__used__)) static void * alloc_section_space(AddrSpace *as, uintptr_t vaddr, size_t p_memsz){
  size_t page_n = p_memsz % PAGESIZE == 0 ? p_memsz / 4096 : (p_memsz / 4096 + 1);
  void *page_start = new_page(page_n);

  printf("Loaded Segment from [%x to %x)\n", vaddr, vaddr + p_memsz);
  
  for (int i = 0; i < page_n; ++i){
    // TODO: 这里prot参数不规范
    map(as, (void *)(vaddr + i * PAGESIZE), (void *)(page_start + i * PAGESIZE), 1);
  }

  return page_start;
}

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  if (fd == -1){ 
    assert(0); //filename指向文件不存在
  }
  
  AddrSpace *as = &pcb->as;
  
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
    void *phys_addr;
    uintptr_t virt_addr;
    switch (section_entry.p_type) {
    case PT_LOAD:
      //virt_addr = (void *)section_entry.p_vaddr; 
      // phys_addr = (void *)alloced_page_start + (section_entry.p_vaddr - 0x40000000); // 这里是把0x40000000加载到他对应的实际地址
      virt_addr = section_entry.p_vaddr;
      phys_addr = alloc_section_space(as, virt_addr, section_entry.p_memsz);

      // printf("Load to %x with offset %x\n", phys_addr, section_entry.p_offset);
      read(fd, phys_addr, section_entry.p_offset, section_entry.p_filesz);
      memset(phys_addr + section_entry.p_filesz, 0, 
        section_entry.p_memsz - section_entry.p_filesz);
      
      if (section_entry.p_filesz < section_entry.p_memsz){// 应该是.bss节
        printf("Setting .bss end %x\n", section_entry.p_vaddr + section_entry.p_memsz);
        //做一个向上的4kb取整数
        pcb->max_brk = (section_entry.p_vaddr + section_entry.p_memsz) % 0xfff == 0 ?
          section_entry.p_vaddr + section_entry.p_memsz : //已经对齐
          ((section_entry.p_vaddr + section_entry.p_memsz) & ~0xfff) + 0xfff; //向上对齐
      }
      
      break;
    
    default:
      break;
    }

  }
  
  printf("Entry: %p", elf_header.e_entry);
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


void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  int envc = 0, argc = 0;
  AddrSpace *as = &pcb->as;
  protect(as);
  
  if (envp){
    for (; envp[envc]; ++envc){}
  }
  if (argv){
    for (; argv[argc]; ++argc){}
  }
  char *envp_ustack[envc];

  void *alloced_page = new_page(NR_PAGE) + NR_PAGE * 4096; //得到栈顶

  //这段代码有古怪，一动就会出问题，莫动
  //这个问题确实已经被修正了，TMD，真cao dan
  // 2021/12/16
  
  map(as, as->area.end - 8 * PAGESIZE, alloced_page - 8 * PAGESIZE, 1); 
  map(as, as->area.end - 7 * PAGESIZE, alloced_page - 7 * PAGESIZE, 1);
  map(as, as->area.end - 6 * PAGESIZE, alloced_page - 6 * PAGESIZE, 1); 
  map(as, as->area.end - 5 * PAGESIZE, alloced_page - 5 * PAGESIZE, 1);
  map(as, as->area.end - 4 * PAGESIZE, alloced_page - 4 * PAGESIZE, 1); 
  map(as, as->area.end - 3 * PAGESIZE, alloced_page - 3 * PAGESIZE, 1);
  map(as, as->area.end - 2 * PAGESIZE, alloced_page - 2 * PAGESIZE, 1); 
  map(as, as->area.end - 1 * PAGESIZE, alloced_page - 1 * PAGESIZE, 1); 
  
  char *brk = (char *)(alloced_page - 4);
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
  ptr_brk = ptr_brk - argc;
  
  printf("%p\n", ptr_brk);
  printf("%p\t%p\n", alloced_page, ptr_brk);
  //printf("%x\n", ptr_brk);
  //assert((intptr_t)ptr_brk == 0xDD5FDC);
  for (int i = 0; i < argc; ++i){
    ptr_brk[i] = (intptr_t)(argv_ustack[i]);
  }

  ptr_brk -= 1;
  *ptr_brk = argc;
  
  //printf("AAAAAA: %d\n", (intptr_t)ptr_brk - (intptr_t)alloced_page + (intptr_t)as->area.end);

  //这条操作会把参数的内存空间扬了，要放在最后
  uintptr_t entry = loader(pcb, filename);
  Area karea;
  karea.start = &pcb->cp;
  karea.end = &pcb->cp + STACK_SIZE;

  Context* context = ucontext(as, karea, (void *)entry);
  pcb->cp = context;


  //context->GPRx = (intptr_t)ptr_brk - (intptr_t)alloced_page + (intptr_t)as->area.end;
  context->GPRx = (intptr_t)(ptr_brk);
}
