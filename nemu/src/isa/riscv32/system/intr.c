#include <isa.h>

#define IRQ_TIMER 0x80000007

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
  csr.mstatus.m.MPIE = csr.mstatus.m.MIE;
  csr.mstatus.m.MIE  = 0;
  IFDEF(CONFIG_ETRACE, Log("etrace: MIE = 0"));


  return csr.mtvec;
}


word_t isa_query_intr() {
  if (csr.mstatus.m.MIE == 1 && cpu.INTR) {
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
