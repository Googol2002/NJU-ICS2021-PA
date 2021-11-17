#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <BMP.h>
#include <string.h>

int main() {
  printf("Doing bmp-test...\n");
  NDL_Init(0);
  int w, h;
  //void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  w = 128;
  h = 128;
  uint32_t *bmp = malloc(w * h * sizeof(uint32_t));
  for (int i = 0; i < w * h; ++i){
    bmp[i] = 0x00FF00FF;
  }
  assert(bmp);
  NDL_OpenCanvas(&w, &h);
  NDL_DrawRect(bmp, 0, 0, w, h);
  free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
