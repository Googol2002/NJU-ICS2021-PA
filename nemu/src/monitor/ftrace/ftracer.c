#include "ftracer.h"

#include <utils.h>
#include <stdlib.h>


STACK_ENTRY header;

void init_ftracer(const char* elf_file){
    init_elf(elf_file);
    header.info = NULL;
    header.next = NULL;
}

void stack_call(paddr_t addr){
    STACK_ENTRY *node = malloc(sizeof(STACK_ENTRY));
    node->next = header.next;
    node->info = check_func(addr);
    header.next = node;
}

void stack_return(){
    STACK_ENTRY *node = header.next;
    header.next = node->next;
    free(node);
}

static void travel(STACK_ENTRY *r, int depth){
    if (r != NULL){
        travel(r->next, depth + 1);
        printf("At " ASNI_FMT("<%#x>", ASNI_FG_YELLOW) ASNI_FMT("\t%s  \n", ASNI_FG_BLUE),  r->info->start, r->info->func_name);
    }
}

void print_stack_trace(){
    printf(ASNI_FMT("====== Call Stack ======\n", ASNI_FG_BLUE));
    travel(header.next, 0);
}