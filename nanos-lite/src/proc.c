#include <proc.h>
#include <fs.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

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

#define PROG_PATH "/bin/dummy"
void init_proc() {
  context_kload(&pcb[0], hello_fun, "ONE");
  char *argv[] = {PROG_PATH, NULL};
  context_uload(&pcb[1], PROG_PATH, argv, NULL);
  // context_uload(&pcb[1], "/bin/pal", argv, NULL);

  switch_boot_pcb();
  // Log("Initializing processes...");

  // // load program here
  // naive_uload(NULL, "/bin/pal");

}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  printf("I'm here\n");
  // then return the new context
  return current->cp;
}

int execve(const char *filename, char *const argv[], char *const envp[]){
  if (fs_open(filename, 0, 0) == -1){// 文件不存在
    return -1;
  }
  printf("Loading from %s ...\n", filename);
  context_uload(&pcb[1], filename, argv, envp);
  switch_boot_pcb();
  yield();
  return 0;
}