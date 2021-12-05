#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  uintptr_t argc = *args;
  char **argv = (char **)(args + 1);

  printf("argv: %s", *argv);
  for (args += 1; args; ++args){}
  char **envp = (char **)(args + 1);

  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
