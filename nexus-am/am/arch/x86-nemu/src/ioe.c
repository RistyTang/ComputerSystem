#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() //相关初始化
{
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {//返回程序启动后经历的毫秒数
  return 0;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {//指示屏幕大小
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) //绘制矩形
{
  int i;
  for (i = 0; i < _screen.width * _screen.height; i++) {
    fb[i] = i;
  }
}

void _draw_sync() //绘制内容同步
{
}

int _read_key() {//返回按键的键盘码，无按键则返回keynone
  return _KEY_NONE;
}
