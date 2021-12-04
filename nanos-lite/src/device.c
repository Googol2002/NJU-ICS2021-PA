#include <common.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  yield();
  char *str = (char *)buf;
  for (int i = 0; i < len; ++i){
    putch(str[i]);
  }
  return len;
}

//offset被忽视
size_t events_read(void *buf, size_t offset, size_t len) {
  yield();
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;
  
  int real_length = 4;
  char *tag = ev.keydown ? "kd " : "ku ";
  if (real_length <= len){
    strcpy(buf, tag);
  }else {
    assert(0);
    return 0;
  }
  real_length += strlen(keyname[ev.keycode]);
  
  if (real_length<= len){
    strcat(buf, keyname[ev.keycode]);
  }else {
    Log("Need %d for %s%s but got %d", strlen(keyname[ev.keycode]), (char *)buf, keyname[ev.keycode], len);
    assert(0);
    return 0;
  }
  Log("Got  (kbd): %s (%d) %s\n", keyname[ev.keycode], ev.keycode, ev.keydown ? "DOWN" : "UP");
  
  return real_length;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  //Log("I'm reading");

  if (offset > 0){
    return 0;
  }

  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;

  int ret = snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d", w, h);
  Log("%s", (char *)buf);
  if (ret >= len){
    assert(0);
  }

  return ret + 1;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {

  // const uint32_t *src = (uint32_t *)buf;
  // uint32_t *fb = (uint32_t *)(uintptr_t)(FB_ADDR + offset); //字节编址

  // for (int i = 0; i < len / 4; ++i){
  //   fb[i] = src[i];
  // }
  yield();
  uintptr_t *ptr;
  ptr = (uintptr_t *)(&buf);

  io_write(AM_GPU_MEMCPY, offset, (void *)*ptr, len);
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
  
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
