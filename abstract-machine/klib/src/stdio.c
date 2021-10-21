#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int vsprintf(char *out, const char *fmt, va_list ap);

int printf(const char *fmt, ...) {
  char buffer[1024];
  va_list arg;
  va_start (arg, fmt);
  
  int done = vsprintf(buffer, fmt, arg);

  putstr(buffer);

  return done;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char buffer[128];
  char *txt;
  int num, len;
  
  int state = 0, i, j;//模仿一个状态机
  for (i = 0, j = 0; fmt[i] != '\0'; ++i){
    switch (state)
    {
    case 0:
      if (fmt[i] != '%')
        out[j++]=fmt[i];
      else
        state = 1;
      break;
    
    case 1:
      switch (fmt[i])
      {
      case 's':
        txt = va_arg(ap, char*);
        for (int k = 0; txt[k] !='\0'; ++k)
          out[j++] = txt[k];
        break;
      
      case 'd':
        num = va_arg(ap, int);
        for (len = 0; num ; num /= 10, ++len)
          buffer[len] = num % 10 + '0';//逆序的
        for (int k = len - 1; k >= 0; --k)
          out[j++] = buffer[k];
        break;
      }
      state = 0;
      break;
    }
  }

  out[j] = '\0';
  return j;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list valist;
  va_start(valist, fmt);

  int res = vsprintf(out ,fmt, valist);
  va_end(valist);
  return res;
}

// int snprintf(char *out, size_t n, const char *fmt, ...) {
//   panic("Not implemented");
// }

// int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
//   panic("Not implemented");
// }

#endif
