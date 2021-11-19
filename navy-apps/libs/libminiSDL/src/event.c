#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64], action[8], key[32];
  
  printf("SDL_WAITING...\n");
  while (!NDL_PollEvent(buf, sizeof(buf))) {
    printf("waiting...\n");
  }
  
  sscanf(buf, "%s %s", action, key);
  
  if (strcmp("kd", action) == 0){
    event->type = SDL_KEYDOWN;
  }else if(strcmp("ku", action) == 0){
    event->type = SDL_KEYUP;
  }else {
    assert(0);
  }

  for (int i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
    if (strcmp(key, keyname[i]) == 0){
      event->key.keysym.sym = i;
      break ;
    }
  }

  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
