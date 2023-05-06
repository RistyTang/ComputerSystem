#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;//
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))
extern void ramdisk_write(const void* buf,off_t offset,size_t len);
extern void fb_write(const void* buf,off_t offset,size_t len);

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

ssize_t fs_write(int fd,uint8_t *buf,size_t len)
{
  //得到要操作的file指针
  Finfo *fp = &file_table[fd];
  ssize_t datalen = fp->size - fp->open_offset;
  ssize_t writelen = datalen < len ? datalen : len;
  //为1或者2时将buf首地址，len长度字节输出
  if(fd == FD_STDOUT || fd == FD_STDERR)//1 2
  {
    char c;
    for(int i=0;i<len;i++)
    {
      memcpy(&c,(void *)buf+i,1);
      _putc(c);
    }
    return len;
  }
  else if(fd == FD_FB)//3
  {
    fb_write(buf,fp->open_offset,len);
  }
  else
  {
    if(fd < 6 || fd >=NR_FILES)
    {
      printf("wrong in sys_write for wrong fd\n");
      return -1;
    }
    ramdisk_write(buf,fp->disk_offset + fp->open_offset,len);
  }
  fp->open_offset += writelen;
  return writelen;
  
  
}
