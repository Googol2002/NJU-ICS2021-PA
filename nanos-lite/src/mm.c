#include <memory.h>
#include <stdint.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  //nr_page * 4KB
  pf += nr_page * 4 * 1024;
  return pf - nr_page * 4 * 1024;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert(n % (4096) == 0);
  void *ret = new_page(n / (4096));
  memset(ret, 0, n);
  return ret;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

//#define align4k_page_number(x) (x % 4096 == 0 ? x / 4096 : (x / 4096 + 1))

extern PCB *current;
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  uintptr_t max_page_start = (current->max_brk & ~0xfff); 
  uintptr_t max_page_pn = max_page_start >> 12;
  uintptr_t brk_pn = brk >> 12;

  if (brk_pn > max_page_pn){
    void *allocted_page =  new_page(brk_pn - max_page_pn);
    for (int i = 0; i < brk_pn - max_page_pn; ++i){
      //TODO: prot 有问题
      map(&current->as, (void *)(max_page_start + (i + 1) * 4096),
       (void *)(allocted_page + i * 4096), 0);
    }
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
