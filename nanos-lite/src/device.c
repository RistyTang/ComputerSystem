#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,



static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  char buffer[40];
  int key = _read_key();
  int down = 0;
  if(key & 0x8000 )
  {
    key ^= 0x8000;
    down = 1;
  }
  if(key != _KEY_NONE)
  {
    sprintf(buffer,"%s  %s \n",down ? "keydown" : "keyup", keyname[key]);
  }
  else
  {
    sprintf(buffer,"t %d\n",_uptime());
  }
  if(strlen(buffer) <= len)
  {
    strncpy((char*)buf,buffer,strlen(buffer));
    return strlen(buffer);
  }
  Log("strlen(event) > len, return 0 \n");

  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
  strncpy(buf,dispinfo + offset,len);
}


void fb_write(const void *buf, off_t offset, size_t len) {
  
  assert(offset % 4 == 0 && len % 4 == 0);
  extern _screen;
  int index, screen_x,screen_y;
  int width = _screen.width;
  int height = _screen.height;
  
  
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
