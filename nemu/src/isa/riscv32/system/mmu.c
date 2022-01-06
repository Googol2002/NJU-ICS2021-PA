#include <isa.h>
#include <isa-def.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

#define VA_VPN_0(x) (((vaddr_t)x & 0x003FF000u) >> 12)
#define VA_VPN_1(x) (((vaddr_t)x & 0xFFC00000u) >> 22)
#define VA_OFFSET(x) ((vaddr_t)x & 0x00000FFFu)

#define PTE_PPN_MASK (0xFFFFFC00u)
#define PTE_PPN(x) (((vaddr_t)x & PTE_PPN_MASK) >> 10)

#define PTE_V 0x01
#define PTE_A 0x40
#define PTE_D 0x80

typedef uint32_t PTE;
extern riscv32_CSR_state csr;

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  paddr_t page_table_entry_addr = (csr.satp << 12) + VA_VPN_1(vaddr) * 4;
  PTE page_table_entry = paddr_read(page_table_entry_addr, 4);
  // if ((page_table_entry & PTE_V) == 0){
  //   Log("page_table_entry not valid, vaddr: %#x", vaddr);
  // }
  Assert(page_table_entry & PTE_V, "page_table_entry not valid, vaddr: %#x", vaddr);

  paddr_t leaf_page_table_entry_addr = PTE_PPN(page_table_entry) * 4096 + VA_VPN_0(vaddr) * 4;
  PTE leaf_page_table_entry = paddr_read(leaf_page_table_entry_addr, 4);
  // if ((leaf_page_table_entry & PTE_V) == 0){
  //   Log("leaf_page_table_entry not valid, vaddr: %#x, pte_entry: %#x", vaddr, leaf_page_table_entry_addr);
  // }
  Assert(leaf_page_table_entry & PTE_V, "leaf_page_table_entry not valid, vaddr: %#x, pte_entry: %#x", vaddr, leaf_page_table_entry_addr);
  if (type == 0){//读
    paddr_write(leaf_page_table_entry_addr, 4, leaf_page_table_entry | PTE_A);
  }else if (type == 1){//写
    paddr_write(leaf_page_table_entry_addr, 4, leaf_page_table_entry | PTE_D);
  }
  paddr_t pa = PTE_PPN(leaf_page_table_entry) * 4096 + VA_OFFSET(vaddr);
  // assert(pa == vaddr);

  return pa;
}
