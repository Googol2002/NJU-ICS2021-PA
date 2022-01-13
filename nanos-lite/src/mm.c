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
  uintptr_t max_page_end = current->max_brk; 
  uintptr_t max_page_pn = (max_page_end >> 12) - 1;
  uintptr_t brk_pn = brk >> 12;//12

  //[page_start, page_end) 所以，应该是大于等于就重新分配
  //printf("申请内存 max_page_end: %p\t to brk: %p\n", max_page_end, brk);//分配从[max_page_pn, brk_pn]的所有页
  if (brk >= max_page_end){
    void *allocted_page =  new_page(brk_pn - max_page_pn + 1);//需要申请的新的页数
    for (int i = 0; i < brk_pn - max_page_pn + 1; ++i){
      //TODO: prot 有问题
      map(&current->as, (void *)(max_page_end + i * 0xfff),
       (void *)(allocted_page + i * 0xfff), 1);
    }

    current->max_brk = (brk_pn + 1) << 12;
    assert(current->max_brk > brk);
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
