#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < 32; ++i){
    if (ref_r->gpr[i]._32 != cpu.gpr[i]._32)
      Log("对于[%s]寄存器, expcet %x, but got %x.", regs[i], ref_r->gpr[i]._32, cpu.gpr[i]._32);
      return false;
  }

  if (ref_r->pc == pc){
    return true;
  }else {
    Log("%x\t%x", ref_r->pc, pc);
    return false;
  }
}

void isa_difftest_attach() {
}
