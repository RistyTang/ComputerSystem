#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;//文件在ramdisk中的偏移
  off_t open_offset;//文件被打开之后的读写指针
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
extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern void ramdisk_write(const void* buf,off_t offset,size_t len);
extern void fb_write(const void* buf,off_t offset,size_t len);
extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern size_t events_read(void *buf, size_t len);
extern size_t get_ramdisk_size();

void init_fs() {
  // TODO: initialize the size of /dev/fb
  //extern _screen;
  file_table[FD_FB].size = _screen.width * _screen.height *sizeof(u_int32_t);
  Log("/dev/fb initialized with size %d\n",fs_filesz(FD_FB));
}

void set_open_offset(int fd,off_t n)
{
  assert(fd >= 0 && fd < NR_FILES);
  assert(n >= 0);
  if(n > file_table[fd].size)
  {
    n = file_table[fd].size;
  }
  file_table[fd].open_offset = n;
}

off_t fs_lseek(int fd,off_t offset,int whence)
{
  switch (whence)
  {
  case SEEK_SET:
    set_open_offset(fd,offset);
    break;
  case SEEK_CUR:
    set_open_offset(fd,offset + file_table[fd].open_offset);
    break;
  case SEEK_END:
    set_open_offset(fd,file_table[fd].size + offset);
    break;
  default:
    printf("wrong whence case %d\n",whence);
    return -1;
  }
  return file_table[fd].open_offset;
}

size_t fs_filesz(int fd)//返回fd指向的文件大小
{
  assert(fd >= 0 && fd < NR_FILES);
  return file_table[fd].size;
}

int fs_open(const char* filename,int flags,int mode)
{
  for(int i=0;i<NR_FILES;i++)
  {
    if(strcmp(filename,file_table[i].name) == 0)
    {
      Log("open No.%d file \"%s \" \n",i,filename);
      return i;
    }
  }
  //can't find
  printf("can't find file %s\n",filename);
  assert(0);
  return -1;
}


ssize_t fs_read(int fd,void *buf,size_t len)
{
  assert(fd >= 0 && fd < NR_FILES);
  //可读字节数
  int n = fs_filesz(fd) - file_table[fd].open_offset;
  if(n > len)
  {
    n = len;
  }
  switch (fd)
  {
  case FD_STDIN:
    Log("this is not the actual stdin\n");
    return 0;
  case FD_STDOUT:
    Log("this is not the actual stdout\n");
    return 0;
  case FD_STDERR:
    Log("this is not the actual stderr\n");
    return 0;
  case FD_FB:
    //should be fb_read()
    return 0;
  case FD_EVENTS:
    return events_read(buf,len);
  case FD_DISPINFO:
    dispinfo_read(buf,file_table[fd].open_offset,n);
    break;
  default:
    ramdisk_read(buf,file_table[fd].disk_offset + file_table[fd].open_offset,n);
    break;
  }
  set_open_offset(fd,file_table[fd].open_offset + n);
  return n;
}

int fs_close(int fd)
{
  assert(fd >= 0 && fd < NR_FILES);
  return 0;
}

ssize_t fs_write(int fd,void *buf,size_t len)
{
  assert(fd >= 0 && fd < NR_FILES);
  int n = fs_filesz(fd) - file_table[fd].open_offset;
  if(n > len)
  {
    n = len;
  }
  char c;
  switch (fd)
  {
  case FD_STDIN:
    Log("this is not the actual stdin\n");
    return 0;
  case FD_STDOUT://为1或者2时将buf首地址，len长度字节输出
  case FD_STDERR:
    for(int i=0;i<len;i++)
    {
      memcpy(&c,(void *)buf+i,1);
      _putc(c);
    }
    return len;
  case FD_FB://write to frame buffer
    fb_write(buf,file_table[fd].open_offset,n);
    break;
  default:
    ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,n);
    break;
  }
  set_open_offset(fd,file_table[fd].open_offset + n);
  return n;
  
}

