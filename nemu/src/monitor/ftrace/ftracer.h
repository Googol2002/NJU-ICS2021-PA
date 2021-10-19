#ifndef _FTRACER_H_
#define _FTRACER_H_

#include "elfloader.h"

typedef struct __STACK_ENTRY_{
    FUNC_INFO* info;
    struct __STACK_ENTRY_ *next;
}STACK_ENTRY;

void init_ftracer();



#endif