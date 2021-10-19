#include <../include/rtl/rtl.h>
#include "../../../monitor/ftrace/ftracer.h"
#include "../local-include/reg.h"

def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sll) {
  rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(add) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mul) {
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh) {//TODO
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu) {
  rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
  rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div) {
  rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem) {
  rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
  rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra) {
  rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srai) {
  rtl_srai(s, ddest, dsrc1, id_src2->imm & 0b111111u);
}

def_EHelper(srl) {
  rtl_srl(s, ddest, dsrc1, dsrc2);
}

def_EHelper(srli) {
  rtl_srli(s, ddest, dsrc1, id_src2->imm & 0b111111u);
}

def_EHelper(slli) {
  rtl_slli(s, ddest, dsrc1, id_src2->imm & 0b111111u);
}

def_EHelper(beq) {
  if (*dsrc1 == *ddest){
    rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
  }
}

def_EHelper(bne) {
  if (*dsrc1 != *ddest){
    rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
  }
}

def_EHelper(bge) {
  if ((sword_t)(*dsrc1) >= (sword_t)(*ddest)){
    rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
  }
}

def_EHelper(blt) {
  if ((sword_t)(*dsrc1) < (sword_t)(*ddest)){
    rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
  }
}

def_EHelper(bltu) {
  if ((*dsrc1) < (*ddest)){
    rtl_addi(s, &s->dnpc, &s->pc, id_src2->imm);
  }
}

def_EHelper(sltiu) {
  rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}

def_EHelper(auipc) {
  rtl_addi(s, ddest, &s->pc, id_src1->imm);
}

def_EHelper(jal) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_addi(s, &s->dnpc, &s->pc, id_src1->imm);
  stack_call(s->dnpc);
}

def_EHelper(jalr) {
  rtl_addi(s, s0, &s->pc, 4);
  rtl_addi(s, &s->dnpc, dsrc1, id_src2->imm);
  rtl_andi(s, &s->dnpc, &s->dnpc, ~1);
  rtl_addi(s, ddest, s0, 0);
  if (ddest == &gpr(8)){
    stack_return();
  }else{
    stack_call(s->dnpc);
  }
}