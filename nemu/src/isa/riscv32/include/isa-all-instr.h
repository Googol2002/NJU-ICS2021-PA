#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(addi) f(auipc) f(jal) f(jalr) f(nemu_trap) \
f(add) f(beq) f(sub) f(mul) f(sltiu) f(bne) f(sltu) f(xor) f(or) f(and) f(sb) f(sra) f(srl)

def_all_EXEC_ID();
