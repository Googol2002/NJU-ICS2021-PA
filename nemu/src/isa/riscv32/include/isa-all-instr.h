#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(addi) f(auipc) f(jal) f(jalr) f(nemu_trap) \
f(add) f(beq) f(sub) f(mul) f(sltiu) f(bne) f(sltu) f(xor) f(or) f(and) f(sb) f(sra) f(srl) \
f(srai) f(srli) f(lbu) f(andi) f(sll) f(xori) f(bge) f(div) f(rem) f(blt) f(slt) f(lh) f(lhu)

def_all_EXEC_ID();
