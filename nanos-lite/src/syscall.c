#include <common.h>
#include "syscall.h"
#include "fs.h"

#define STRACE 1

void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

void sys_exit(Context *c){
  halt(c->GPR1);
  c->GPRx = 0;
}

void sys_write(Context *c){
  if (c->GPR2 == 1 || c->GPR2 == 2){
    for (int i = 0; i < c->GPR4; ++i){
      putch(*(((char *)c->GPR3) + i));
    }
    c->GPRx = c->GPR4;
  }else {  
    int ret = fs_write(c->GPR1, (void *)c->GPR2, c->GPR3);
    c->GPRx = ret;
  }
}

void sys_brk(Context *c){
  c->GPRx = 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  #ifdef STRACE
    Log("System call trace\nmcause\t\tGPR1\t\tGPR2\t\tGPR3\t\tGPR4 \n%x\t%d\t\t%x\t\t%x\t\t%x",
      c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);
  #endif

  switch (a[0]) {
    case SYS_yield:
      sys_yield(c);
      break;

    case SYS_exit:
      sys_exit(c);
      break;

    case SYS_write:
      sys_write(c);
      break;

    case SYS_brk:
      sys_brk(c);
      break;
    
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
