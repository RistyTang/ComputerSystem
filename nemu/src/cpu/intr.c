#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  //step1
  memcpy(&t1,&cpu.eflags,sizeof(cpu.eflags));
  rtl_li(&t0,t1);
  rtl_push(&t0);
  memcpy(&t1,&cpu.cs,sizeof(cpu.cs));
  rtl_li(&t0,t1);
  rtl_push(&t0);
  rtl_li(&t0,ret_addr);
  rtl_push(&t0);
  //step2
  vaddr_t gate_addr = cpu.idtr.base + NO * sizeof(GateDesc);
  assert(gate_addr <= cpu.idtr.base + cpu.idtr.limit);
  //step3
  uint32_t off_15_0 = vaddr_read(gate_addr,2);
  uint32_t off_31_16 = vaddr_read(gate_addr + sizeof(GateDesc) - 2,2);
  //step 4
  uint32_t target_addr = (off_31_16 << 16) + off_15_0;
  //step 5
  decoding.is_jmp=1;
  decoding.jmp_eip = target_addr;

}

void dev_raise_intr() {
}
