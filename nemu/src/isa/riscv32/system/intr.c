#include <isa.h>

#define Machine_Software_Interrupt (1 << 31 | 3)

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  csr.mepc    = epc;
  csr.mcause  = Machine_Software_Interrupt;
  //TODO: 缺少一些操作
  return 0;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
