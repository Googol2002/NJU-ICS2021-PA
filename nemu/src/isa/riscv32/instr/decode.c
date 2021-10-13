#include "../local-include/reg.h"
#include <cpu/ifetch.h>
#include <isa-all-instr.h>

def_all_THelper();

static uint32_t get_instr(Decode *s) {
  return s->isa.instr.val;
}

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (Decode *s, Operand *op, word_t val, bool flag)

static def_DopHelper(i) {
  op->imm = val;
}

static def_DopHelper(j) {
  op->imm = 0;
  op->imm |= s->isa.instr.j.simm20 << 20;
  op->imm |= s->isa.instr.j.imm19_12 << 12;
  op->imm |= s->isa.instr.j.imm11 << 11;
  op->imm |= s->isa.instr.j.imm10_1 << 1;
}

static def_DopHelper(r) {
  bool is_write = flag;
  static word_t zero_null = 0;
  op->preg = (is_write && val == 0) ? &zero_null : &gpr(val);
}

static def_DHelper(I) {
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, false);
  decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, false);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}

static def_DHelper(R) {
  decode_op_r(s, id_src1, s->isa.instr.r.rs1, false);
  decode_op_r(s, id_src2, s->isa.instr.r.rs2, false);
  decode_op_r(s, id_dest, s->isa.instr.r.rd, true);
}

static def_DHelper(J) {
  decode_op_j(s, id_src1, 0, false);
  decode_op_r(s, id_dest, s->isa.instr.j.rd, true);
}

static def_DHelper(U) {
  decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

static def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
  sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
  decode_op_i(s, id_src2, simm, false);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, false);
}

static def_DHelper(B) {
  decode_op_r(s, id_src1, s->isa.instr.b.rs1, false);
  sword_t simm = (s->isa.instr.b.simm12 << 12) | s->isa.instr.b.imm11 << 11 | 
    s->isa.instr.b.imm10_5 << 5 | s->isa.instr.b.imm4_1 << 1;
  decode_op_i(s, id_src2, simm, false);
  // 特殊情况，无处可存了。
  decode_op_r(s, id_dest, s->isa.instr.b.rs2, false);
}

def_THelper(load) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", lbu);
  return EXEC_ID_inv;
}

def_THelper(store) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", sb);
  return EXEC_ID_inv;
}

def_THelper(add_sub_mul) {
  def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", add);
  def_INSTR_TAB("0100000 ????? ????? ??? ????? ????? ??", sub);
  def_INSTR_TAB("0000001 ????? ????? ??? ????? ????? ??", mul);
  // def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", mul);
  return EXEC_ID_inv;
}

def_THelper(sltu_) {
  def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", sltu);
  return EXEC_ID_inv;
}

def_THelper(xor_div) {
  def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", xor);
  def_INSTR_TAB("0000001 ????? ????? ??? ????? ????? ??", div);
  return EXEC_ID_inv;
}

def_THelper(or_rem) {
  def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", or);
  def_INSTR_TAB("0000001 ????? ????? ??? ????? ????? ??", rem);
  return EXEC_ID_inv;
}

def_THelper(and_) {
  def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", and);
  return EXEC_ID_inv;
}

def_THelper(srl_sra) {
  def_INSTR_TAB("0100000 ????? ????? ??? ????? ????? ??", sra);
  def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", srl);
  return EXEC_ID_inv;
}

def_THelper(sll_) {
  def_INSTR_TAB("0000000 ????? ????? ??? ????? ????? ??", sll);
  return EXEC_ID_inv;
}

def_THelper(cal) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", add_sub_mul);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", sll_);
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sltu_);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", xor_div);
  def_INSTR_TAB("??????? ????? ????? 110 ????? ????? ??", or_rem);
  def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", and_);
  def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", srl_sra);
  return EXEC_ID_inv;
}

def_THelper(srli_srai) {
  def_INSTR_TAB("010000? ????? ????? ??? ????? ????? ??", srai);
  def_INSTR_TAB("000000? ????? ????? ??? ????? ????? ??", srli);
  return EXEC_ID_inv;
}

def_THelper(cal_imm) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", addi);
  def_INSTR_TAB("??????? ????? ????? 111 ????? ????? ??", andi);
  def_INSTR_TAB("??????? ????? ????? 011 ????? ????? ??", sltiu);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", xori);
  def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", srli_srai);
  return EXEC_ID_inv;
}

def_THelper(branch) {
  def_INSTR_TAB("??????? ????? ????? 000 ????? ????? ??", beq);
  def_INSTR_TAB("??????? ????? ????? 001 ????? ????? ??", bne);
  def_INSTR_TAB("??????? ????? ????? 101 ????? ????? ??", bge);
  def_INSTR_TAB("??????? ????? ????? 100 ????? ????? ??", blt);
  
  return EXEC_ID_inv;
}

def_THelper(main) {
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I     , load);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S     , store);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01101 11", U     , lui);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00100 11", I     , cal_imm);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00101 11", U     , auipc);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11011 11", J     , jal);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11001 11", I     , jalr);//该opcode唯一funct3
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01100 11", R     , cal);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 11000 11", B     , branch);
  def_INSTR_TAB  ("??????? ????? ????? ??? ????? 11010 11",         nemu_trap);
  return table_inv(s);
};

int isa_fetch_decode(Decode *s) {
  s->isa.instr.val = instr_fetch(&s->snpc, 4);
  int idx = table_main(s);
  return idx;
}
