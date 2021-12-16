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
  void *page_end = new_page(page_n);
  void *page_start = page_end - page_n * PAGESIZE;

  printf("Page_start %x page_end %x\n", page_start, page_end);
  
  for (int i = 0; i < page_n; ++i){
    // TODO: 这里prot参数不规范
    printf("%x, %x\n", vaddr + i * PAGESIZE, page_start + i * PAGESIZE);
    map(as, (void *)(vaddr + i * PAGESIZE), (void *)(page_start + i * PAGESIZE), 0);
  }

  return page_start;
}

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  if (fd == -1){ 
    assert(0); //filename指向文件不存在
  }
  
  AddrSpace *as = &pcb->as;
  // TODO: 这里prot参数不规范
  // as->area.start 0x40000000
  // void *alloced_page = new_page(8);
  // void *alloced_page_start = alloced_page - PAGESIZE * 8;
  // map(as, as->area.start, alloced_page_start, 0);
  // map(as, as->area.start + 1 * PAGESIZE, alloced_page_start + 1 * PAGESIZE, 0);
  // map(as, as->area.start + 2 * PAGESIZE, alloced_page_start + 2 * PAGESIZE, 0);
  // map(as, as->area.start + 3 * PAGESIZE, alloced_page_start + 3 * PAGESIZE, 0);
  // map(as, as->area.start + 4 * PAGESIZE, alloced_page_start + 4 * PAGESIZE, 0);
  // map(as, as->area.start + 5 * PAGESIZE, alloced_page_start + 5 * PAGESIZE, 0);
  // map(as, as->area.start + 6 * PAGESIZE, alloced_page_start + 6 * PAGESIZE, 0);
  // map(as, as->area.start + 7 * PAGESIZE, alloced_page_start + 7 * PAGESIZE, 0);
  
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

      printf("Load to %x with offset %x\n", phys_addr, section_entry.p_offset);
      read(fd, phys_addr, section_entry.p_offset, section_entry.p_filesz);
      memset(phys_addr + section_entry.p_filesz, 0, 
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

  void *alloced_page = new_page(NR_PAGE);

  //这段代码有古怪，一动就会出问题，莫动

  // printf("%x \n", alloced_page);
  // int counter = 1;
  // // 给用户栈做了分配和映射
  // for (void *page = alloced_page - PAGESIZE; page >= alloced_page - PAGESIZE * NR_PAGE; page -= PAGESIZE, ++counter){
  //   // TODO: 这里prot参数不规范
  //   printf("va: %x pa: %x\n", as->area.end - counter * PAGESIZE, page);
  //   map(as, as->area.end - counter * PAGESIZE, page, 0); 
  // }
  // printf("va: %x pa: %x\n", as->area.end - 8 * PAGESIZE, alloced_page - 8 * PAGESIZE);

  // printf("va: %x pa: %x\n", as->area.end - 1 * PAGESIZE, alloced_page - 1 * PAGESIZE);
  map(as, as->area.end - 8 * PAGESIZE, alloced_page - 8 * PAGESIZE, 0); 
  map(as, as->area.end - 7 * PAGESIZE, alloced_page - 7 * PAGESIZE, 0);
  map(as, as->area.end - 6 * PAGESIZE, alloced_page - 6 * PAGESIZE, 0); 
  map(as, as->area.end - 5 * PAGESIZE, alloced_page - 5 * PAGESIZE, 0);
  map(as, as->area.end - 4 * PAGESIZE, alloced_page - 4 * PAGESIZE, 0); 
  map(as, as->area.end - 3 * PAGESIZE, alloced_page - 3 * PAGESIZE, 0);
  map(as, as->area.end - 2 * PAGESIZE, alloced_page - 2 * PAGESIZE, 0); 
  map(as, as->area.end - 1 * PAGESIZE, alloced_page - 1 * PAGESIZE, 0); 
  
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
  //halt(10001);
  //assert((uint32_t)ptr_brk != 0xDD1FE0);

  // 分配argv空间
  ptr_brk -= 1;
  *ptr_brk = 0;
  ptr_brk = ptr_brk - argc;
 // assert((uint32_t)ptr_brk != 0xDD1FDC);
  //halt(10001);
  //ptr_brk = (intptr_t *)((int32_t)ptr_brk - sizeof(intptr_t *) * argc);
  
  for (int i = 0; i < argc; ++i){
    ptr_brk[i] = (intptr_t)(argv_ustack[i]);
  }

  ptr_brk -= 1;
  *ptr_brk = argc;
  
  //printf("Loading... %x with %d\n", ptr_brk, *ptr_brk);
  //这条操作会把参数的内存空间扬了，要放在最后
  uintptr_t entry = loader(pcb, filename);
  Area karea;
  karea.start = &pcb->cp;
  karea.end = &pcb->cp + STACK_SIZE;

  Context* context = ucontext(as, karea, (void *)entry);
  pcb->cp = context;
  
  context->GPRx = (intptr_t)ptr_brk;
}
