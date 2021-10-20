#include "ftracer.h"

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
        printf("%s at %#x \n", r->info->func_name, r->info->start);
    }
}

void print_stack_trace(){
    travel(header.next, 0);
}