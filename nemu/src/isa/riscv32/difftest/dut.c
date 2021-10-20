#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for (int i = 0; i < 32; ++i){
    if (ref_r->gpr[i]._32 != cpu.gpr[i]._32)
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
