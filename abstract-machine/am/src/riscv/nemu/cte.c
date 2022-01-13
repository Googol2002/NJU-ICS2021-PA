#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include <arch/riscv32-nemu.h>

#define Machine_Software_Interrupt (11)
#define User_Software_Interrupt (8)
#define IRQ_TIMER 0x80000007

#define KERNEL 3
#define USER 0

static Context* (*user_handler)(Event, Context*) = NULL;

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {
  uintptr_t mscratch;
  uintptr_t kas = 0;
  asm volatile("csrr %0, mscratch" : "=r"(mscratch));
  c->np = (mscratch == 0 ? KERNEL : USER);
  asm volatile("csrw mscratch, %0" : : "r"(kas));

  if ((uintptr_t)&c < 0x80000000){
    halt(10001);
  }
  __am_get_cur_as(c);
  //printf("__am_irq_handle c->pdir内容地址修改前 页表项:%p\t上下文地址%p\t所在栈帧:%p\n", c->pdir, c, &c);
  //printf("设置c->np为%d\n", c->np);
  if (user_handler) {
    Event ev = {0};
    
    switch (c->mcause) {
      case Machine_Software_Interrupt:
      case User_Software_Interrupt:
        // printf("c->GPR1 = %d \n", c->GPR1);
        if (c->GPR1 == -1){ // 特指-1
          ev.event = EVENT_YIELD;
        }else {
          ev.event = EVENT_SYSCALL;
        }
        c->mepc += 4;
        break;

      case IRQ_TIMER:
        ev.event = EVENT_IRQ_TIMER;
        break;

      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  //printf("__am_irq_handle c->pdir内容地址修改后 页表项:%p\t上下文地址%p\t所在栈帧:%p\n", c->pdir, c, &c);
  __am_switch(c);
  //printf("c->np为%d\n", c->np);
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
  //-4是为了缓存一个t0，为了Real VME
  uintptr_t *t0_buf = kstack.end - 4;
  *t0_buf = 0;

  Context *context = kstack.end - sizeof(Context) - 4;
  context->mstatus = 0x1800 | 0x80;
  context->mepc    = (uintptr_t)entry;
  context->gpr[10] = (uintptr_t)arg;
  context->pdir    = NULL;
  //为了Real VME
  context->np      = 3;
  context->gpr[2]  = (uintptr_t)kstack.end - 4;
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
