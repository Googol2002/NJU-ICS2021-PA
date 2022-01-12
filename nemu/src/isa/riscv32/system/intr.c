#include <isa.h>

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  csr.mepc    = epc;
  csr.mcause  = NO;
  // 11b-M 01b-S 00b-U
  // 真的需要吗？
  // csr.mstatus.m.MPP = 3;
  //TODO: 缺少一些操作
  return csr.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
