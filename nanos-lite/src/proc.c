#include <proc.h>

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
void context_uload(PCB *pcb, const char *filename, char *const argv[], int argc, char *const envp[], int envc);
void init_proc() {
  context_kload(&pcb[0], hello_fun, "ONE");
  char *args[] = {"--skip"};
  context_uload(&pcb[1], "/bin/pal", args, 1, NULL, 0);

  switch_boot_pcb();
  // Log("Initializing processes...");

  // // load program here
  // naive_uload(NULL, "/bin/pal");

}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;

  // always select pcb[0] as the new process
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);;

  // then return the new context
  return current->cp;
}
