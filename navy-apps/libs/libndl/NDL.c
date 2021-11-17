#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);

  return tv.tv_usec / 1000 + tv.tv_sec * 1000;
}

int NDL_PollEvent(char *buf, int len) {
  FILE *fp = fopen("/dev/events", "r");

  return fread(buf, sizeof(char), len, fp);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }

  char info[128], key[64];
  int value;

  FILE *dispinfo = fopen("/proc/dispinfo", "r");
  fread(info, sizeof(char), sizeof(info) / sizeof(char), dispinfo);

  printf("%s\n", info);
  /* 获取第一个子字符串 */
  char *token = strtok(info, "\n");
   
   /* 继续获取其他的子字符串 */
   while( token != NULL ) {
      printf("%s\n", token);
      sscanf(token, "%s : %d", key, &value);

      if(strcmp(key, "WIDTH") == 0){
        screen_w = value;
      }else if(strcmp(key, "HEIGHT") == 0) {
        screen_h = value;
      }

      token = strtok(NULL, "\n");
  }

  printf("With width = %d, height = %d.\n", screen_w, screen_h);

  return 0;
}

void NDL_Quit() {
}
