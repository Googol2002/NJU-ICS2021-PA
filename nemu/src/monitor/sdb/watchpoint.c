#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char condation[32];
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

static int number = 1;

bool check_watchpoint(){
  return false;
}


WP* new_wp(char *condation){
  if (free_->next == NULL){
    assert(0);
  }
  
  WP* result = free_->next;
  result->NO = number ++;
  result->next = NULL;
  strcpy(result->condation, condation);

  free_->next = result->next;
  
  if (head == NULL){
    head = result;
  }else{
    result->next = head->next;
    head->next = result;
  }

  return result;
}

static void insert_free(WP *wp){
  wp->next = free_->next;
  free_->next = wp;
}

void free_wp(int NO){
  if (head->NO == NO){
    insert_free(head);
    head = head->next;
    return ;
  }

  WP* prev = head;
  while (prev->next){
    if (prev->next->NO == NO){
      insert_free(prev->next);
      prev->next = prev->next->next;
      return ;
    }
    prev = prev->next;
  }

  printf("未找到NO: %d", NO);
}
