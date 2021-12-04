#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include <arch/riscv32-nemu.h>

#define Machine_Software_Interrupt (11)


static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    
    switch (c->mcause) {
      case Machine_Software_Interrupt:
        // printf("c->GPR1 = %d \n", c->GPR1);
        if (c->GPR1 == -1){ // 特指-1
          ev.event = EVENT_YIELD;
        }else {
          ev.event = EVENT_SYSCALL;
        }
        break;

      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
    c->mepc += 4;
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *context = kstack.end - sizeof(Context);
  context->mepc = (uintptr_t)entry;
  context->mcause = 0x1800;
  //TODO: 还需要添加一些
  return context;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
