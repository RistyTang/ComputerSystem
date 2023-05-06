#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

extern void getScreen(int* p_width,int* p_height);

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
}

void fb_write(const void *buf, off_t offset, size_t len) {
  /*
  assert(offset % 4 == 0 && len % 4 == 0);
  int index, screen_x,screen_y;
  int width = 0;
  int height = 0;
  getSceen(&width,&height);
  for(int i = 0;i < len / 4;i++)
  {
    index = offset / 4 + i;
    screen_y = index / width;
    screen_x = index % width;
    _draw_rect(buf + i * 4,screen_x,screen_y,1,1);
  }
  */
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
