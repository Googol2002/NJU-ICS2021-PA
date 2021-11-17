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
  char *str = (char *)buf;
  for (int i = 0; i < len; ++i){
    putch(str[i]);
  }
  return len;
}

//offset被忽视
size_t events_read(void *buf, size_t offset, size_t len) {
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
  if (real_length <= len){
    strcat(buf, keyname[ev.keycode]);
  }else {
    assert(0);
    return 0;
  }
  Log("Got  (kbd): %s (%d) %s\n", keyname[ev.keycode], ev.keycode, ev.keydown ? "DOWN" : "UP");
  
  return real_length;
}

static int dispinfo_state = 0;
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  //Log("I'm reading");
  if (dispinfo_state){
    dispinfo_state = !dispinfo_state;
    return 0;
  }

  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;

  int ret = snprintf(buf, len, "WIDTH:%d\nHEIGHT : %d", w, h);

  if (ret >= len){
    assert(0);
  }

  dispinfo_state = !dispinfo_state;
  return ret;
}

#define MMIO_BASE 0xa0000000
#define FB_ADDR (MMIO_BASE   + 0x1000000)

size_t fb_write(const void *buf, size_t offset, size_t len) {
  //TODO: 奇怪的操作
  Log("%d\n", offset);
  const uint8_t *src = (uint8_t *)buf;
  uint8_t *fb = (uint8_t *)(uintptr_t)FB_ADDR; //字节编址

  for (int i = 0; i < len; ++i){
    fb[offset + i] = src[i];
  }

  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
  
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
