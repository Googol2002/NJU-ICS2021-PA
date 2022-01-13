#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)

#if __riscv_xlen == 32
#define LOAD  lw
#define STORE sw
#define XLEN  4
#else
#define LOAD  ld
#define STORE sd
#define XLEN  8
#endif

#define REGS(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15) f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)

#define PUSH(n) STORE concat(x, n), (n * XLEN)(sp);
#define POP(n)  LOAD  concat(x, n), (n * XLEN)(sp);

#define CONTEXT_SIZE  ((32 + 3 + 1 + 1) * XLEN)
#define OFFSET_SP     ( 2 * XLEN)
#define OFFSET_CAUSE  (32 * XLEN)
#define OFFSET_STATUS (33 * XLEN)
#define OFFSET_EPC    (34 * XLEN)
//中间有一个pdir没有地址
#define OFFSET_NP     (36 * XLEN)

.align 3
.globl __am_asm_trap
__am_asm_trap:
//还在用户栈底
  addi sp, sp, -XLEN
  STORE t4, (sp)
  mv t4, sp

  csrrw sp, mscratch, sp   // (1) atomically exchange sp and mscratch
  bnez sp, save_context    // (2) take the branch if we trapped from user
  csrrw sp, mscratch, sp   // (3) if we trapped from kernel, 恢复之前的状态

//进入内核栈
  save_context:
  addi sp, sp, -CONTEXT_SIZE

  STORE t4, OFFSET_SP(sp)

  MAP(REGS, PUSH)

  csrr t0, mcause
  csrr t1, mstatus
  csrr t2, mepc

  STORE t0, OFFSET_CAUSE(sp)
  STORE t1, OFFSET_STATUS(sp)
  STORE t2, OFFSET_EPC(sp)

  # set mstatus.MPRV to pass difftest
  li a0, (1 << 17)
  or t1, t1, a0
  csrw mstatus, t1

  mv a0, sp
  jal __am_irq_handle
  mv sp, a0
//切换到新的内核栈空间

//判断c->np == USER
  LOAD t4, OFFSET_NP(sp)
  bnez t4, restore_contexts
//c->np == USER
  addi t4, sp, CONTEXT_SIZE
  csrw mscratch, t4

restore_contexts:

  LOAD t1, OFFSET_STATUS(sp)
  LOAD t2, OFFSET_EPC(sp)
  csrw mstatus, t1
  csrw mepc, t2

  MAP(REGS, POP)

  LOAD t4, OFFSET_SP(sp)
  addi sp, sp, CONTEXT_SIZE
//回到了用户栈
  mv sp, t4
  LOAD t4, (sp)
  addi sp, sp, XLEN
  
  mret