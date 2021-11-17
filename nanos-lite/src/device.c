#include <common.h>
#include <string.h>

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
    return 0;
  }
  real_length += strlen(keyname[ev.keycode]);
  if (real_length <= len){
    strcat(buf, keyname[ev.keycode]);
  }else {
    return 0;
  }
  Log("Got  (kbd): %s (%d) %s\n", keyname[ev.keycode], ev.keycode, ev.keydown ? "DOWN" : "UP");
  
  return real_length;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  
  
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
