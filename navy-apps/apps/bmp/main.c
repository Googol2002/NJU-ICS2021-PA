#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <NDL.h>
#include <SDL.h>
#include <BMP.h>
#include <string.h>

int main() {
  //printf("Doing bmp-test...\n");
  SDL_Init(0);
  SDL_Surface *screen = SDL_SetVideoMode(400, 300, 32, SDL_HWSURFACE);
  int w, h;
  SDL_Surface *slide = SDL_LoadBMP("/share/pictures/test.bmp");

  assert(slide);
  SDL_UpdateRect(slide, 0, 0, 0, 0);

  SDL_Quit();
  printf("Test ends! Spinning...\n");
  while (1);
  return 0;
}
