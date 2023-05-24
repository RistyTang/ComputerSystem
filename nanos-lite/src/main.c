#include "common.h"

/* Uncomment these macros to enable corresponding functionality. */
#define HAS_ASYE
#define HAS_PTE

void init_mm(void);
void init_ramdisk(void);
void init_device(void);
void init_irq(void);
void init_fs(void);
uint32_t loader(_Protect *, const char *);
extern void load_prog(const char *filename);

int main() {
#ifdef HAS_PTE
  init_mm();
#endif
  //nanos-lite和nemu是两个独立的项目
  Log("'Hello World!' from Nanos-lite");//hello message此处的log宏不同于nemu中的log宏
  //通过调用klib的printk输出，最终调用TRM的putc
  Log("Build time: %s, %s", __TIME__, __DATE__);//compiling time

  init_ramdisk();//将nanos-lite中的一段内存作为磁盘ramdisk使用，进行初始化

  init_device();//设备初始化，目前它会调用_ioe_init

#ifdef HAS_ASYE
  Log("Initializing interrupt/exception handler...");
  init_irq();
#endif

  init_fs();

  //uint32_t entry = loader(NULL, "/bin/dummy");//加载用户程序，返回程序入口地址
  //((void (*)(void))entry)();
  load_prog("/bin/dummy");
  //printf("reached here\n");

  panic("Should not reach here");
}
