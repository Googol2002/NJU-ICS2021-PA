#include <isa.h>

extern CPU_state cpu;
void dev_raise_intr() {
    cpu.INTR = true;
}
