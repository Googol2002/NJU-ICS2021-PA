#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

struct __event_element{
  uint8_t type;
  uint8_t sym;
  struct __event_element* next;
};
typedef struct __event_element event_element;

static event_element event_queue = {.type = 0, .sym = 0, .next = NULL};
static event_element *end = &event_queue;

static void append(uint8_t type, uint8_t sym){
  event_element *new_element = malloc(sizeof(event_element));
  new_element->type = type;
  new_element->sym = sym;
  new_element->next = NULL;
  end->next = new_element;
  end = new_element;
}

static int pop(uint8_t *type, uint8_t *sym){
  if (event_queue.next == NULL){
    return 0;
  }else {
    event_element *buf = event_queue.next;
    *type = buf->type;
    *sym = buf->sym;
    event_queue.next = buf->next;
    free (buf);
  }
  return 1;
}

static uint8_t key_state[sizeof(keyname) / sizeof(keyname[0])] = {0};

void SDL_PumpEvents(void){
  char buf[64], action[8], key[32];
  
  int ret;
  uint8_t type = 0, sym = 0;
  while ((ret = NDL_PollEvent(buf, sizeof(buf)), ret)){
    sscanf(buf, "%s %s", action, key);

    for (int i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
      if (strcmp(key, keyname[i]) == 0){
        sym = i;
        break ;
      }
    }

    if (strcmp("kd", action) == 0){
      type = SDL_KEYDOWN;
      key_state[sym] = 1;
    }else if(strcmp("ku", action) == 0){
      type = SDL_KEYUP;
      key_state[sym] = 0;
    }else {
      assert(0);
    }

    append(type, sym);
  }
}


int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  uint8_t type = 0, sym = 0;
  SDL_PumpEvents();

  if (pop(&type, &sym)){
    ev->type = type;
    ev->key.keysym.sym = sym;
  }else {
    return 0;
  }


  // char buf[64], action[8], key[32];
  
  // int ret = NDL_PollEvent(buf, sizeof(buf));
  
  // if (ret == 0){
  //   return 0;
  // }

  // //注意：这里代码和Wait中重复
  // sscanf(buf, "%s %s", action, key);
  
  // if (strcmp("kd", action) == 0){
  //   ev->type = SDL_KEYDOWN;
  // }else if(strcmp("ku", action) == 0){
  //   ev->type = SDL_KEYUP;
  // }else {
  //   assert(0);
  // }

  // for (int i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
  //   if (strcmp(key, keyname[i]) == 0){
  //     ev->key.keysym.sym = i;
  //     break ;
  //   }
  // }
  // return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  uint8_t type = 0, sym = 0;
  SDL_PumpEvents();

  while (!pop(&type, &sym)){}
  
  event->type = type;
  event->key.keysym.sym = sym;


  // char buf[64], action[8], key[32];
  
  // while (!NDL_PollEvent(buf, sizeof(buf))) {}
  
  // //注意：这里代码和Poll中重复
  // sscanf(buf, "%s %s", action, key);
  
  // if (strcmp("kd", action) == 0){
  //   event->type = SDL_KEYDOWN;
  // }else if(strcmp("ku", action) == 0){
  //   event->type = SDL_KEYUP;
  // }else {
  //   assert(0);
  // }

  // for (int i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
  //   if (strcmp(key, keyname[i]) == 0){
  //     event->key.keysym.sym = i;
  //     break ;
  //   }
  // }

  // return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  if (numkeys)
    *numkeys = sizeof(key_state) / sizeof(key_state[0]);
  SDL_PumpEvents();
  return key_state;
}