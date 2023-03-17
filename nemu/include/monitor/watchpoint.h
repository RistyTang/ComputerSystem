#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;//监视点序号
  struct watchpoint *next;//下一个监视点

  /* TODO: Add more members if necessary */
  char expr[32];//记录表达式
  uint32_t value;//记录值

} WP;
WP* new_wp(char * args);
WP* get_wp(int num);
void free_wp(WP* wp);
#endif
