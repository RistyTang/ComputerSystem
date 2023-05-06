#include "common.h"
#include "syscall.h"
#include "fs.h"

//static inline防篡改
static inline uintptr_t sys_write_handle(int fd,uintptr_t buf,size_t len)
{

  return fs_write(fd,(uint8_t *)buf,len);
  /*
  if(fd == 1 || fd == 2)
  {
    char c;
    for(int i=0;i<len;i++)
    {
      memcpy(&c,(void *)buf+i,1);
      _putc(c);
    }
    return len;
  }
  else
  {
    return -1;
  }
  */
  
}

//static inline防篡改
static inline int sys_brk_handle(int addr)
{
  return 0;
}

//static inline防篡改
static inline uintptr_t sys_open_handle(const char* pathname)
{
  return fs_open(pathname,0,0);
}

//static inline防篡改
static inline int sys_lseek_handle(int fd,off_t offset,int whence)
{
  return fs_lseek(fd,offset,whence);
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);


  switch (a[0]) 
  {
    case SYS_none:
      r->eax = 1;
      break;
    case SYS_exit:
      _halt(a[1]);
      break;
    case SYS_write:
      SYSCALL_ARG1(r) = sys_write_handle(a[1],(uintptr_t)a[2],a[3]);
      break;
    case SYS_brk:
      SYSCALL_ARG1(r) = sys_brk_handle(a[1]);
      break;
    case SYS_lseek:
      SYSCALL_ARG1(r) = sys_lseek_handle(a[1],a[2],a[3]);
      break;
    case SYS_open:
      SYSCALL_ARG1(r) = sys_open_handle((char*)a[1]);
      break;
    case SYS_read:
      SYSCALL_ARG1(r) = fs_read(a[1],(uint8_t*)a[2],a[3]);
      break;
    case SYS_close:
      SYSCALL_ARG1(r) = fs_close(a[1]);
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
