#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>
#include <stddef.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  //panic("Not implemented");
  size_t len = 0;
  while(s[len] != '\0'){
    len++;
  }

  return len;
}

char *strcpy(char *dst, const char *src) {
  int i;
  for (i = 0; src[i] != '\0'; ++i){
    dst[i] = src[i];
  }
  dst[i] = '\0';

  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  int i;
  for (i = 0; i < n && src[i] != '\0'; ++i){
    dst[i] = src[i];
  }
  for (; i < n; ++i){
    dst[i] = '\0';
  }

  return dst;
}

char *strcat(char *dst, const char *src) {
  int i, j;
  for (i = 0; dst[i] != '\0'; ++i){}
  for (j = 0; src[j] != '\0'; ++j, ++i){
    dst[i] = src[j];//追加
  }
  dst[i] = '\0';

  return dst;
}

int strcmp(const char *s1, const char *s2) {
  int i;
  for (i = 0; s1[i] != '\0' && s2[i] != '\0'; ++i){
    if (s1[i] != s2[i])
      return (int)(s1[i]) - (int)(s2[i]);
  }
  //case 同时结束：为0
  //case s1先结束，0-x < 0
  //case s2先结束，x-0 > 0
  return (int)(s1[i]) - (int)(s2[i]);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  int i;
  for (i = 0; i < n && s1[i] != '\0' && s2[i] != '\0'; ++i){
    if (s1[i] != s2[i])
      return (int)(s1[i]) - (int)(s2[i]);
  }
  //case 同时结束：为0
  //case s1先结束，0-x < 0
  //case s2先结束，x-0 > 0
  //case 到达n，返回0
  if (i == n)
    return 0;
  return (int)(s1[i]) - (int)(s2[i]);
}

void *memset(void *s, int c, size_t n) {
  unsigned char* target = (unsigned char*)s;
  for (int i = 0; i < n; ++i){
    target[i] = (unsigned char)c;
  }
  return s;
}

// 如果目标区域和源区域有重叠的话，
// memmove() 能够保证源串在被覆盖之前将重叠区域的字节拷贝到目标区域中，
// 复制后源区域的内容会被更改。
void *memmove(void *dst, const void *src, size_t n) {
  char *char_dst = (char *)dst;
  char *char_src = (char *)src;

  if (dst < src){
    for (int i = 0; i < n; ++i)
      char_dst[i] = char_src[i];
  }else if(dst > src){
    for (int i = n-1; i >= 0; --i)
      char_dst[i] = char_src[i];
  }//else 不需要复制

  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  char *char_out = (char *)out;
  const char *char_in = (const char *)in;
  for (int i = 0; i < n; ++i){
    char_out[i] = char_in[i];
  }

  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  char *char_s1 = (char *)s1;
  char *char_s2 = (char *)s2;

  int i;
  for (i = 0; i < n; ++i){
    if (char_s1[i] != char_s2[i])
      return (int)(char_s1[i]) - (int)(char_s2[i]);
  }
  if (i == n)
    return 0;
  return (int)(char_s1[i]) - (int)(char_s2[i]);
}

#endif
