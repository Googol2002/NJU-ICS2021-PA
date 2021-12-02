#include <common.h>
#include "syscall.h"
#include "fs.h"
#include <proc.h>
#include <sys/time.h>

#define STRACE 1
#undef STRACE

void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

void naive_uload(PCB *pcb, const char *filename);

void sys_exit(Context *c){
  naive_uload(NULL, "/bin/nterm");
  c->GPRx = 0;
}

void sys_execve(Context *c){
  const char *fname = (const char *)c->GPR2;
  naive_uload(NULL, fname);
  c->GPRx = 0;
}

void sys_write(Context *c){
  // if (c->GPR2 == 1 || c->GPR2 == 2){
  //   for (int i = 0; i < c->GPR4; ++i){
  //     putch(*(((char *)c->GPR3) + i));
  //   }
  //   c->GPRx = c->GPR4;
  // }else {  
  int ret = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
  // }
}

void sys_read(Context *c){
  int ret = fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_brk(Context *c){
  c->GPRx = 0;
}

void sys_open(Context *c){
  int ret = fs_open((char *)c->GPR2, c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_close(Context *c){
  int ret = fs_close(c->GPR2);
  c->GPRx = ret;
}

void sys_lseek(Context *c){
  int ret = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
  c->GPRx = ret;
}

struct __temp{
 #if __SIZEOF_POINTER__ == 8
  __uint64_t sec;
  __uint64_t usec;
 #elif __SIZEOF_POINTER__ == 4
  __uint32_t sec;
  __uint32_t usec;
#endif
};

void sys_gettimeofday(Context *c){
  struct timeval *tv = (struct timeval *)c->GPR2;
  __uint64_t time = io_read(AM_TIMER_UPTIME).us;
  tv->tv_usec = (time % 1000000);
  tv->tv_sec = (time / 1000000);
  c->GPRx = 0;
}

static void strace(Context *c){
  #ifdef STRACE
    //TODO: 缺一个trace
    Log("System call trace\nmcause\t\tGPR1\t\tGPR2\t\tGPR3\t\tGPR4 \n0x%x\t%d\t\t0x%x\t\t0x%x\t\t0x%x",
      c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);
  #endif
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  strace(c);

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

    case SYS_open:
      sys_open(c);
      break;

    case SYS_close:
      sys_close(c);
      break;

    case SYS_read:
      sys_read(c);
      break;

    case SYS_lseek:
      sys_lseek(c);
      break;

    case SYS_gettimeofday:
      sys_gettimeofday(c);
      break;

    case SYS_execve:
      sys_execve(c);
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
