#include <am.h>
#include <nemu.h>
#include <klib.h>
#include <riscv/riscv.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

#define VA_VPN_0(x) (((uintptr_t)x & 0x003FF000u) >> 12)
#define VA_VPN_1(x) (((uintptr_t)x & 0xFFC00000u) >> 22)
#define VA_OFFSET(x) ((uintptr_t)x & 0x00000FFFu)

#define PTE_PPN_MASK (0xFFFFFC00u)
#define PTE_PPN(x) (((uintptr_t)x & PTE_PPN_MASK) >> 10)

void map(AddrSpace *as, void *va, void *pa, int prot) {
  PTE *page_table_entry = as->ptr + VA_VPN_1(va) * 4;

  if (!(*page_table_entry & PTE_V)){ // 说明二级表未分配
    *page_table_entry = (*page_table_entry & ~PTE_PPN_MASK) | (PTE_PPN_MASK & ((uintptr_t)pgalloc_usr(PGSIZE) >> 2));
    *page_table_entry = *page_table_entry | PTE_V;
    printf("二级表未分配\t二级表项地址:%p\t虚拟地址:%p\n", page_table_entry, va);
  }
  
  printf("设置二级表项\t虚拟地址:%p\n", va);
  // 找到二级表中的表项
  PTE *leaf_page_table_entry = (PTE *)(PTE_PPN(*page_table_entry) * 4096 + VA_VPN_0(va) * 4);
  // 设置PPN
  *leaf_page_table_entry = (*leaf_page_table_entry & ~PTE_PPN_MASK) | (PTE_PPN_MASK & ((uintptr_t)pa >> 2));
  *leaf_page_table_entry = *leaf_page_table_entry | PTE_V;
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *context = kstack.end - sizeof(Context);
  context->mstatus = 0x1800;
  context->mepc    = (uintptr_t)entry;
  // context->pdir    = as->ptr;
  
  return context;
}
