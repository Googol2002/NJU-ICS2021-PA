#include <../include/rtl/rtl.h>
#include "../local-include/reg.h"

extern CSR_state csr;

rtlreg_t* decode_csr_no(int csr_no){
    switch(csr_no){
        case 0x300:
            return &(csr.mstatus);

        case 0x301:
            return &(csr.mtvec);

        case 0x341:
            return &(csr.mepc);

        case 0x342:
            return &(csr.mcause);

        default:
            assert(0);
    }
}

def_EHelper(csrrw) {
  rtl_mv(s, s0, decode_csr_no(id_src2->imm));
  rtl_mv(s, decode_csr_no(id_src2->imm), dsrc1);
  rtl_mv(s, ddest, s0);
}