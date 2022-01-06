#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
  int *array = malloc(100 * sizeof(int));
  for (int i = 0; i < 100; ++i){
    printf("%p\n", &(array[i]));
  }
  printf("Hi, I'm dummy!\n");
  return _syscall_(SYS_yield, 0, 0, 0);
}
