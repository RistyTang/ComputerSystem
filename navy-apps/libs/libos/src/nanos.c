#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <time.h>
#include "syscall.h"

// TODO: discuss with syscall interface
#ifndef __ISA_NATIVE__

// FIXME: this is temporary

int _syscall_(int type, uintptr_t a0, uintptr_t a1, uintptr_t a2){
  int ret = -1;
  //将系统调用参数依次放入eax-edx中，之后执行int 0x80手动触发一个特殊的异常
  //操作系统捕捉此异常并且进行处理，处理结束后设置好返回值，返回到上述的内联汇编中
  //内联汇编最终从eax取出返回值
  asm volatile("int $0x80": "=a"(ret): "a"(type), "b"(a0), "c"(a1), "d"(a2));
  return ret;
}

void _exit(int status) {
  _syscall_(SYS_exit, status, 0, 0);
}

int _open(const char *path, int flags, mode_t mode) {
  //_exit(SYS_open);
  _syscall_(SYS_open,(uintptr_t)path,flags,mode);
}

int _write(int fd, void *buf, size_t count){
  return _syscall_(SYS_write,fd,(uintptr_t)buf,count);
}

void *_sbrk(intptr_t increment){
  //return (void *)-1;
  extern int end;
  static uintptr_t program_break = (uintptr_t)&end;
  uintptr_t new_program_break = program_break + increment;
  int res = _syscall_(SYS_brk,new_program_break,0,0);
  //success
  if(res == 0)
  {
    uintptr_t temp = program_break;
    program_break = new_program_break;
    return (void *)temp;
  }
  //fail
  return (void *)-1;
}

int _read(int fd, void *buf, size_t count) {
  //_exit(SYS_read);
  return _syscall_(SYS_read,fd,(uintptr_t)buf,count);
}

int _close(int fd) {
  //_exit(SYS_close);
  return _syscall_(SYS_close,fd,0,0);
}

off_t _lseek(int fd, off_t offset, int whence) {
  //_exit(SYS_lseek);
  return _syscall_(SYS_lseek,fd,offset,whence);
}

// The code below is not used by Nanos-lite.
// But to pass linking, they are defined as dummy functions

// not implement but used
int _fstat(int fd, struct stat *buf) {
  return 0;
}

int execve(const char *fname, char * const argv[], char *const envp[]) {
  assert(0);
  return -1;
}

int _execve(const char *fname, char * const argv[], char *const envp[]) {
  return execve(fname, argv, envp);
}

int _kill(int pid, int sig) {
  _exit(-SYS_kill);
  return -1;
}

pid_t _getpid() {
  _exit(-SYS_getpid);
  return 1;
}

char **environ;

time_t time(time_t *tloc) {
  assert(0);
  return 0;
}

int signal(int num, void *handler) {
  assert(0);
  return -1;
}

pid_t _fork() {
  assert(0);
  return -1;
}

int _link(const char *d, const char *n) {
  assert(0);
  return -1;
}

int _unlink(const char *n) {
  assert(0);
  return -1;
}

pid_t _wait(int *status) {
  assert(0);
  return -1;
}

clock_t _times(void *buf) {
  assert(0);
  return 0;
}

int _gettimeofday(struct timeval *tv) {
  assert(0);
  tv->tv_sec = 0;
  tv->tv_usec = 0;
  return 0;
}

int _fcntl(int fd, int cmd, ... ) {
  assert(0);
  return 0;
}

int pipe(int pipefd[2]) {
  assert(0);
  return 0;
}

int dup(int oldfd) {
  assert(0);
  return 0;
}

int dup2(int oldfd, int newfd) {
  assert(0);
  return 0;
}

pid_t vfork(void) {
  assert(0);
  return 0;
}

#endif
