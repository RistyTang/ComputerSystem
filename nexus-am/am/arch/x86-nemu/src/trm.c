#include <am.h>
#include <x86.h>

// Define this macro after serial has been implemented
#define HAS_SERIAL

#define SERIAL_PORT 0x3f8

extern char _heap_start;
extern char _heap_end;
extern int main();

_Area _heap = {//指示堆区的起始和末尾
  .start = &_heap_start,
  .end = &_heap_end,
};

static void serial_init() {
#ifdef HAS_SERIAL
  outb(SERIAL_PORT + 1, 0x00);
  outb(SERIAL_PORT + 3, 0x80);
  outb(SERIAL_PORT + 0, 0x01);
  outb(SERIAL_PORT + 1, 0x00);
  outb(SERIAL_PORT + 3, 0x03);
  outb(SERIAL_PORT + 2, 0xC7);
  outb(SERIAL_PORT + 4, 0x0B);
#endif
}

void _putc(char ch) {//输出一个字符
#ifdef HAS_SERIAL
  while ((inb(SERIAL_PORT + 5) & 0x20) == 0);
  outb(SERIAL_PORT, ch);
#endif
}

void _halt(int code) {//结束程序运行
  asm volatile(".byte 0xd6" : :"a"(code));//内联汇编语句，0xd6就是nemu-trap

  // should not reach here
  while (1);
}

void _trm_init() {//TRM相关的初始化
  serial_init();
  int ret = main();
  _halt(ret);
}
