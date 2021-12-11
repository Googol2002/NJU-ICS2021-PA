#include <isa.h>
#include <memory/paddr.h>

word_t vaddr_ifetch(vaddr_t addr, int len) {
  if (isa_mmu_check(addr, len, 0) == MMU_TRANSLATE){
    addr = isa_mmu_translate(addr, len, 0);
  }
  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  if (isa_mmu_check(addr, len, 0) == MMU_TRANSLATE){
    addr = isa_mmu_translate(addr, len, 0);
  }
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  if (isa_mmu_check(addr, len, 0) == MMU_TRANSLATE){
    addr = isa_mmu_translate(addr, len, 0);
  }
  if (addr == 0x806BCF74){
    Log("%x", data);
  }
  paddr_write(addr, len, data);
}
