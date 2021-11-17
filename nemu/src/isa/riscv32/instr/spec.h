#include <../include/rtl/rtl.h>
#include "../local-include/reg.h"

extern CSR_state csr;

rtlreg_t* decode_csr_no(int csr_no){
    switch(csr_no){
        case 0x300://不支持被软件显示改变
            return &(csr.mstatus.value);

        case 0x305:
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
  IFDEF(CONFIG_ETRACE, Log("etrace: cssrw"));
  rtl_mv(s, s0, decode_csr_no(id_src2->imm));
  rtl_mv(s, decode_csr_no(id_src2->imm), dsrc1);
  rtl_mv(s, ddest, s0);
}

#define Machine_Software_Interrupt (11)

def_EHelper(ecall) {
  IFDEF(CONFIG_ETRACE, Log("etrace: ecall"));
  isa_raise_intr(Machine_Software_Interrupt, s->pc);
  rtl_mv(s, &s->dnpc, &(csr.mtvec));
}

def_EHelper(mret) {
  IFDEF(CONFIG_ETRACE, Log("etrace: mret"));
  //在__am_irq_handle中处理
  //rtl_addi(s, s0, &(csr.mepc), 4);
  rtl_mv(s, &s->dnpc, &(csr.mepc));
  // csr.mstatus.m.MIE = csr.mstatus.m.MPIE;
  // csr.mstatus.m.MPIE = 1;

}

def_EHelper(csrrs) {
  IFDEF(CONFIG_ETRACE, Log("etrace: csrrs"));
  rtl_mv(s, s0, decode_csr_no(id_src2->imm));
  rtl_or(s, decode_csr_no(id_src2->imm), dsrc1, s0);
  rtl_mv(s, ddest, s0);
}