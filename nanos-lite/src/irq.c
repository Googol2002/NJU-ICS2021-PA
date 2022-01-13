#include <common.h>
#include <proc.h>


void do_syscall(Context *c);


static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_IRQ_TIMER:
    case EVENT_YIELD:
      // printf("EVENT_YIELD\n");
      return schedule(c); //存起来了现在的，返回了另一个

    case EVENT_SYSCALL:
      do_syscall(c);
      break;

    // case EVENT_IRQ_TIMER:
    //   //Log("EVENT_IRQ_TIMER happening...");
    //   break;

    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
