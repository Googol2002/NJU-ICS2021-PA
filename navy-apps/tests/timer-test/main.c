#include <stdio.h>
#include <assert.h>
#include <NDL.h>

int main(){
    int sec = 1;
  printf("Hello.\n");
  while (1) {
    while(NDL_GetTicks() / 1000000 < sec) {};
    //rtc = io_read(AM_TIMER_RTC);
    //printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    
    if (sec == 1) {
      printf("%d second).\n", sec);
    } else {
      printf("%d seconds).\n", sec);
    }
    sec ++;
  }
}
