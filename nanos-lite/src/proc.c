#include <proc.h>
#include <fs.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

int program_index = 3;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", (char *)arg, j);
    j ++;
    yield();
  }
}

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);

#define PROG_PATH1 "/bin/menu"
#define PROG_PATH2 "/bin/timer-test"
#define PROG_PATH3 "/bin/hello"
void init_proc() {
  context_kload(&pcb[0], hello_fun, "ONE");
  char *argv1[] = {PROG_PATH1, NULL};
  char *argv2[] = {PROG_PATH2, NULL};
  char *argv3[] = {PROG_PATH3, NULL};
  context_uload(&pcb[1], PROG_PATH1, argv1, NULL);
  context_uload(&pcb[2], PROG_PATH2, argv2, NULL);
  context_uload(&pcb[3], PROG_PATH3, argv3, NULL);
  // context_uload(&pcb[1], "/bin/pal", argv, NULL);

  switch_boot_pcb();
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  current = (current == &pcb[0] ? &pcb[program_index] : &pcb[0]);
  //printf("schedule c->pdir内容地址修改后 页表项:%p\t页表项地址%p\n", current->cp->pdir, &current->cp->pdir);
  // then return the new context
  return current->cp;
}

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

int execve(const char *filename, char *const argv[], char *const envp[]){
  if (fs_open(filename, 0, 0) == -1){// 文件不存在
    return -1;
  }
  printf("Loading from %s ...\n", filename);
  context_uload(&pcb[program_index], filename, argv, envp);
  switch_boot_pcb();  
  
  pcb[0].cp->pdir = NULL;
  //TODO: 这是一种trade-off
  //set_satp(pcb[1].cp->pdir);
  printf("PCB[0] pdir: %p cp: %p\n", pcb[0].cp->pdir, pcb[0].cp);

  yield();
  return 0;
}