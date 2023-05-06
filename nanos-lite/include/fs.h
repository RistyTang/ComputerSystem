#ifndef __FS_H__
#define __FS_H__

#include "common.h"
enum {SEEK_SET, SEEK_CUR, SEEK_END};


ssize_t fs_write(int fd,uint8_t *buf,size_t len);
off_t fs_lseek(int fd,off_t offset,int whence);
size_t fs_filesz(int fd);
int fs_open(const char* filename,int flags,int mode);
ssize_t fs_read(int fd,uint8_t *buf,size_t len);
int fs_close(int fd);

#endif
