#include "common.h"
#include"fs.h"

#define DEFAULT_ENTRY ((void *)0x08048000)
extern void ramdisk_read(void * buf,off_t offset,size_t len);
extern size_t get_ramdisk_size();
extern void* new_page(void);

uintptr_t loader(_Protect *as, const char *filename) 
{
  //TODO();
  
  int fd = fs_open(filename,0,0);
  Log("filename = %s,fd = %d",filename,fd);
  //fs_read(fd,DEFAULT_ENTRY,fs_filesz(fd));
  int totalsize = fs_filesz(fd);
  int pagenum = totalsize / PGSIZE;
  if(totalsize % PGSIZE)
  {
    pagenum += 1;
  }
  void *pa = NULL;
  void *va = DEFAULT_ENTRY;
  for(int i = 0;i<pagenum;i++)
  {
    pa = new_page();
    _map(as,va,pa);
    Log("mapped");
    fs_read(fd,pa,PGSIZE);
    va += PGSIZE;
  }
  fs_close(fd);
  //Log("file closed successfully\n");
  //ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());
  

  return (uintptr_t)DEFAULT_ENTRY;
}
