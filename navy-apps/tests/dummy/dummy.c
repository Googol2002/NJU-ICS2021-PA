#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>
#include <string.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

extern char end;

int main() {
  NDL_Init(0);
  int w, h;
  
  FILE *fp = fopen("/share/pictures/test.bmp", "r");
  if (!fp) return NULL;

  printf("Hi, I'm dummy!\n");
  return _syscall_(SYS_yield, 0, 0, 0);
}
