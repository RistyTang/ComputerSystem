#include "nemu.h"
#include "memory/mmu.h"
#include "device/mmio.h"


#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

#define PGSHFT    12      // log2(PGSIZE)
#define PTXSHFT   12      // Offset of PTX in a linear address
#define PDXSHFT   22      // Offset of PDX in a linear address
#define PDX(va)     (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)     (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)     ((uint32_t)(va) & 0xfff)
#define PTE_ADDR(pte)   ((uint32_t)(pte) & ~0xfff)

uint8_t pmem[PMEM_SIZE];//模拟内存实现

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int r = is_mmio(addr);
  if(r == -1)
  {
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
  else
  {
    return mmio_read(addr,len,r);
  }
  
  
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  int r = is_mmio(addr);
  if(r == -1)
  {
    memcpy(guest_to_host(addr), &data, len);
  }
  else
  {
    mmio_write(addr,len,data,r);
  }
  
}

paddr_t page_translate(vaddr_t addr,bool iswrite)
{
  CR0 cr0 = (CR0)cpu.CR0;
  //开启分页机制
  if(cr0.paging && cr0.protect_enable)
  {
    //pde
    CR3 cr3 = (CR3)cpu.CR3;
    PDE *pgdirbase = (PDE*)PTE_ADDR(cr3.val);
    PDE pde = (PDE)paddr_read((uint32_t)(pgdirbase + PDX(addr)),4);
    //pte
    PTE *ptdirbase = (PTE*)PTE_ADDR(pde.val);
    PTE pte = (PTE)paddr_read((uint32_t)(ptdirbase + PTX(addr)),4);
    //present = 1
    Assert(pte.present,"addr = 0x%x",addr);

    pde.accessed = 1;
    pte.accessed = 1;
    if(iswrite)
    {
      pte.dirty = 1;
    }
    // + offset
    paddr_t paddr = PTE_ADDR(pte.val) | OFF(addr);
    return paddr;
  }
  return addr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  //return paddr_read(addr, len);
  if(PTE_ADDR(addr) != PTE_ADDR(addr + len - 1))
  {
    Log("data is out of bound");
    //assert(0);
    int num1 = 0x1000 - OFF(addr);
    int num2 = len - num1;
    paddr_t paddr1 = page_translate(addr,false);
    paddr_t paddr2 = page_translate(addr + num1,false);
    uint32_t bytes1 = paddr_read(paddr1,num1);
    uint32_t bytes2 = paddr_read(paddr2,num2);
    uint32_t result = bytes2 << (8 * num1) | bytes1;
    return result;
  }
  else
  {
    paddr_t paddr = page_translate(addr,false);
    return paddr_read(paddr, len);
  }
  
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  //paddr_write(addr, len, data);
  if(PTE_ADDR(addr) != PTE_ADDR(addr + len - 1))
  {
    assert(0);
  }
  else
  {
    paddr_t paddr = page_translate(addr,false);
    paddr_write(paddr, len, data);
  }
}
