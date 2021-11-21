#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <stdint.h>
#include <NDL.h>

int main(){
  printf("Hello. %d\n", sizeof(struct timeval));
  NDL_Init(0);

  uint32_t time;
  uint32_t msec = 500;
  while (1) {
    time = NDL_GetTicks();
    while(time < msec) {
      time = NDL_GetTicks();
    };
    
    printf("Oh, 0.5s have been lost...\n");
    msec += 500;
  }

  NDL_Quit();
}
