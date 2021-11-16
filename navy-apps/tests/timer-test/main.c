#include <stdio.h>
#include <assert.h>
#include <sys/time.h>

// int gettimeofday(struct timeval *__restrict __p, void *__restrict __tz){
//   return _gettimeofday(__p, __tz);
// }

int main(){
  //NDL_Init(0);
  __uint64_t time;
  __uint64_t usec = 500000;
  struct timeval tv;
  printf("Hello. %d\n", sizeof(struct timeval));
  while (1) {
    gettimeofday(&tv, NULL);
    time = tv.tv_sec * 1000000 + tv.tv_usec;
    while(time < usec) {
      gettimeofday(&tv, NULL);
      time = tv.tv_sec * 1000000 + tv.tv_usec;
    };
    //rtc = io_read(AM_TIMER_RTC);
    //printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    
    printf("又流失了0.5s的时间。");
    usec += 500000;
  }

  
  //NDL_Quit();
}
