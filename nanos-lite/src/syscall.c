#include <common.h>
#include "syscall.h"

#define STRACE 1

void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

void sys_exit(Context *c){
  halt(0);
  c->GPRx = 0;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  #ifdef STRACE
    Log("System call trace\n mcause\tGPR1\tGPR2\tGPR3 \n%x %d %x %x", c->mcause, c->GPR1, c->GPR2, c->GPR3);
  #endif

  switch (a[0]) {
    case SYS_yield:
      sys_yield(c);
      break;

    case SYS_exit:
      sys_exit(c);
      break;
    
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
