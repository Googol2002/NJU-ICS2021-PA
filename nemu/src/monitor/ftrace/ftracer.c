#include "ftracer.h"

#include <utils.h>
#include <stdlib.h>


STACK_ENTRY header;
STACK_ENTRY *end = &header;

void init_ftracer(const char* elf_file){
    init_elf(elf_file);
    header.des_info = NULL;
    header.cur_info = NULL;
    header.next = NULL;
    header.addr = 0;
}

#ifdef CONFIG_FTRACE
static void append(paddr_t cur, paddr_t des, int type){
    STACK_ENTRY *node = malloc(sizeof(STACK_ENTRY));
    end->next = node;
    node->next = NULL;
    node->addr = cur;
    node->cur_info = check_func(cur);
    node->des_info = check_func(des);
    node->type = type;

    end = node;
}
#endif

void stack_call(paddr_t cur, paddr_t des){
  #ifdef CONFIG_FTRACE
    append(cur, des, FT_CALL);
  #endif
}

void stack_return(paddr_t cur, paddr_t des){
  #ifdef CONFIG_FTRACE
    append(cur, des, FT_RET);
  #endif
}

#ifdef CONFIG_FTRACE
    static char *action_name[] = {"Call", "Ret"};
#endif

// static void travel(STACK_ENTRY *r, int depth){
//     if (r != NULL){
//         travel(r->next, depth + 1);
//         // for (int i = 0; i < depth; i++)
//         //     printf("  ");
//         printf("At " ASNI_FMT("<%#x>", ASNI_FG_YELLOW) ASNI_FMT("\t%s  \n", ASNI_FG_BLUE),  r->info->start, r->info->func_name);
//     }
// }

void print_stack_trace(){
  #ifdef CONFIG_FTRACE
    printf("====== " ASNI_FMT("Call Stack", ASNI_FG_BLUE) " ======\n");
    for (STACK_ENTRY* cur = &header; cur != end; cur = cur->next){
        STACK_ENTRY* r = cur->next;
        printf("<%#x>" ASNI_FMT(" %-12s ", ASNI_FG_BLUE) ASNI_FMT("%s", ASNI_FG_WHITE)  
            ASNI_FMT("\t<%#x> ", ASNI_FG_YELLOW) ASNI_FMT("%-12s \n", ASNI_FG_BLUE),  
            r->addr, r->cur_info ? r->cur_info->func_name : "", action_name[r->type], 
            r->des_info ? r->des_info->start : 0, r->des_info ? r->des_info->func_name : "");
    }
  #endif
}