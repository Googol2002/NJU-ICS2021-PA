#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

extern const char *regs[];

void error_finfo();

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < 32; ++i){
    if (ref_r->gpr[i]._32 != cpu.gpr[i]._32){
      error_finfo();
      Log("对于[%s]寄存器, expceted %x, but got %x.", regs[i], ref_r->gpr[i]._32, cpu.gpr[i]._32);
      Assert(ref_r->pc == pc, "PC expected %x but got %x", ref_r->pc, pc);
      return false;
    }
  }

  if (ref_r->pc == pc){
    return true;
  }else {
    error_finfo();
    Log("PC expected %x but got %x", ref_r->pc, pc);
    return false;
  }
}

void isa_difftest_attach() {
}
