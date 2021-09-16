#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

word_t expr(const char *e, bool *success);

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char condation[32];
} WP;

bool check_watchpoint(WP **point);

WP* new_wp(const char *condation, bool *success);

void free_wp(int NO);

void watchpoint_display();

#endif
