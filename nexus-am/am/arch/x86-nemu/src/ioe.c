#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
#define DATA_PORT 0x60
#define STATUS_PORT 0x64
static unsigned long boot_time;

void _ioe_init() //相关初始化
{
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {//返回程序启动后经历的毫秒数
  //return 0;
  unsigned long ms = inl(RTC_PORT) - boot_time;
  return ms;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {//指示屏幕大小
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) //绘制矩形
{
  /*
  int i;
  for (i = 0; i < _screen.width * _screen.height; i++) {
    fb[i] = i;
  }
  */
  int temp = (w > _screen.width - x) ? _screen.width - x : w ;
  int cp_bytes = sizeof(uint32_t) *temp;
  for (int i = 0; i < h && y + i <_screen.height; i++) 
  {
    memcpy(&fb[(y + i)*_screen.width + x],pixels,cp_bytes);
    pixels += w;
  }
}

void _draw_sync() //绘制内容同步
{
}

int _read_key() {//返回按键的键盘码，无按键则返回keynone
  if(inb(0x64))
  {
    return inl(0x60);
  }
  return _KEY_NONE;
}

void getScreen(int* p_width,int* p_height)
{
  *p_width =_screen.width;
  *p_height = _screen.height;
}
