#include <../include/rtl/rtl.h>
#include "../local-include/reg.h"

extern CSR_state csr;

rtlreg_t* decode_csr_no(int csr_no){
    switch(csr_no){
        case 0x300://不支持被软件显示改变
            return &(csr.mstatus.value);

        case 0x305:
            return &(csr.mtvec);

        case 0x340:
            return &(csr.mscratch);

        case 0x341:
            return &(csr.mepc);

        case 0x342: 
            return &(csr.mcause);

        case 0x180:
            return &(csr.satp);

        default:
            assert(0);
    }
}

def_EHelper(csrrw) {
  rtl_mv(s, s0, decode_csr_no(id_src2->imm));
  if (id_src2->imm == 0x340){
    IFDEF(CONFIG_ETRACE, Log("etrace: csrrw mscratch rs:%x csr:%x", *dsrc1, *s0));
  }
  rtl_mv(s, decode_csr_no(id_src2->imm), dsrc1);
  rtl_mv(s, ddest, s0);
}

#define Machine_Software_Interrupt (11)
#define User_Software_Interrupt (8)

def_EHelper(ecall) {
  IFDEF(CONFIG_ETRACE, Log("etrace: ecall"));
  word_t dnpc = 0;
  //Log("PRIV: %d", s->isa.instr.i.funct3);
  if (csr.mstatus.m.MPP == 3){
    dnpc = isa_raise_intr(Machine_Software_Interrupt, s->pc);
  }else if (csr.mstatus.m.MPP == 0){
    dnpc = isa_raise_intr(User_Software_Interrupt, s->pc);
  }else {
    assert(0);
  }
  rtl_mv(s, &s->dnpc, &(dnpc));
}

def_EHelper(mret) {
  IFDEF(CONFIG_ETRACE, Log("etrace: mret"));
  //在__am_irq_handle中处理
  //rtl_addi(s, s0, &(csr.mepc), 4);
  rtl_mv(s, &s->dnpc, &(csr.mepc));

  csr.mstatus.m.MIE = csr.mstatus.m.MPIE;
  csr.mstatus.m.MPIE= 1;
  // if (csr.mstatus.m.MIE){
  //   IFDEF(CONFIG_ETRACE, Log("etrace: MIE = 1"));
  // }else {
  //   IFDEF(CONFIG_ETRACE, Log("etrace: MIE = 0"));
  // }
}

def_EHelper(csrrs) {
  rtl_mv(s, s0, decode_csr_no(id_src2->imm));
  if (id_src2->imm == 0x340){
    IFDEF(CONFIG_ETRACE, Log("etrace: csrrs mscratch rs:%x csr:%x", *dsrc1, *s0));
  }
  rtl_or(s, decode_csr_no(id_src2->imm), dsrc1, s0);
  rtl_mv(s, ddest, s0);
}