#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(auipc) f(ld) f(sd) f(inv) f(addi) f(lui) f(nemu_trap)

def_all_EXEC_ID();
