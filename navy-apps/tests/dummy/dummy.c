#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>
#include <string.h>

struct BitmapHeader {
  uint16_t type;
  uint32_t filesize;
  uint32_t resv_1;
  uint32_t offset;
  uint32_t ih_size;
  uint32_t width;
  uint32_t height;
  uint16_t planes;
  uint16_t bitcount; // 1, 4, 8, or 24
  uint32_t compression;
  uint32_t sizeimg;
  uint32_t xres, yres;
  uint32_t clrused, clrimportant;
} __attribute__((packed));

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

extern char end;

int main() {
  NDL_Init(0);
  int w1, h1;
  const char *filename = "/share/pictures/test.bmp";
  int *width = &w1; 
  int *height = &h1;

  FILE *fp = fopen(filename, "r");
  if (!fp) return NULL;

  struct BitmapHeader hdr;
  assert(sizeof(hdr) == 54);
  assert(1 == fread(&hdr, sizeof(struct BitmapHeader), 1, fp));

  if (hdr.bitcount != 24) return NULL;
  if (hdr.compression != 0) return NULL;
  int w = hdr.width;
  int h = hdr.height;
  uint32_t *pixels = malloc(w * h * sizeof(uint32_t));

  printf("Hi, I'm dummy!\n");
  return _syscall_(SYS_yield, 0, 0, 0);
}
