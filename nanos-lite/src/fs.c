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
extern size_t events_read(void *buf, size_t len);
extern void dispinfo_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();

void init_fs() {
  // TODO: initialize the size of /dev/fb
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

off_t get_disk_offset(int fd)
{
  assert(fd >= 0 && fd < NR_FILES);
  return file_table[fd].disk_offset;
}

off_t get_open_offset(int fd)
{
  assert(fd >= 0 && fd < NR_FILES);
  return file_table[fd].open_offset;
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
      Log("open No.%d file \" %s \" \n",i,filename);
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
  if(fd < 3)
  {
    Log("args invalid : fd <3\n");
    return 0;
  }
  //可读字节数
  int n = fs_filesz(fd) - get_open_offset(fd);
  if(n > len)
  {
    n = len;
  }
  ramdisk_read(buf,get_disk_offset(fd) + get_open_offset(fd),n);
  set_open_offset(fd,get_open_offset(fd) + n);
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
  /*
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
  */
 /*
  if(fd < 3 || fd == FD_DISPINFO)
  {
    printf("wrong fd in write : fd < 3\n");
    return -1;
  }
  int n = file_table[fd].size - file_table[fd].open_offset;
  if(n > len)
  {
    n = len ;
  }
  if(fd == FD_FB)
  {
    fb_write(buf,file_table[fd].open_offset,n);
  }
  else
  {
    ramdisk_write(buf,file_table[fd].disk_offset + file_table[fd].open_offset,n);
  }
  //设置新的读指针位置
  off_t cur_open_offset = file_table[fd].open_offset + n;
  if(cur_open_offset > file_table[fd].size)
  {
    cur_open_offset = file_table[fd].size;
  }
  file_table[fd].open_offset = cur_open_offset;
  return n;
  */
  if(fd < 3)
  {
    Log("args invalid : fd <3\n");
    return 0;
  }
  int n = fs_filesz(fd) - file_table[fd].open_offset;
  if(n > len)
  {
    n = len;
  }
  ramdisk_write(buf,get_disk_offset(fd)+get_open_offset(fd),n);
  set_open_offset(fd,get_open_offset(fd) + n);
  return n;
  
}

