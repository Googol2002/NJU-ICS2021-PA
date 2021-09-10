#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

#define REGISTERS_PER_LINE 4

void isa_reg_display() {
  int length = ARRLEN(regs);
  int i = 0;
  printf("=========寄存器信息=========\n");
  for (i = 0; i + REGISTERS_PER_LINE < length; i+= REGISTERS_PER_LINE){
    for (int j = i; j < i + REGISTERS_PER_LINE; ++j){
      printf("%s: 0x%12x\t", regs[j], cpu.gpr[j]._32);
    }
    printf("\n");
  }
  for (int j = i; j < length; ++j){
      printf("%s: 0x%12x\t", regs[j], cpu.gpr[j]._32);
  }
  printf("\n");
}

word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
