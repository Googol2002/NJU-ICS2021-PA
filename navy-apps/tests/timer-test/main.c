#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

int main(){
  //NDL_Init(0);
  
  int sec = 1;
  printf("Hello.\n");
  struct timeval tv;
  while (1) {
    gettimeofday(&tv, NULL);
    while(tv.tv_usec / 1000000 < sec) {
      printf("%d \n", tv.tv_usec);
      gettimeofday(&tv, NULL);
    };
    //rtc = io_read(AM_TIMER_RTC);
    //printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    
    if (sec == 1) {
      printf("%d second).\n", sec);
    } else {
      printf("%d seconds).\n", sec);
    }
    sec ++;
  }

  
  //NDL_Quit();
}
