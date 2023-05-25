#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,



static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) 
{
  char buffer[40];
  int key = _read_key();
  bool down = false;
  if(key & 0x8000 )
  {
    key ^= 0x8000;
    down = true;
  }
  if(down && (key == _KEY_F12))
  {
    extern void switch_current_game();
    switch_current_game();
    Log("F12 key down : change current game");
  }
  if(key != _KEY_NONE)
  {
    sprintf(buffer,"%s  %s \n",down ? "kd" : "ku", keyname[key]);
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
  Log("the length of event > len.\n");
  
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) 
{
  strncpy(buf,dispinfo + offset,len);
}


void fb_write(const void *buf, off_t offset, size_t len) 
{
  //Log("fb_write()");
  assert(offset % 4 == 0 && len % 4 == 0);
  int index, screen_x,screen_y1,screen_y2;
  //get screen size
  int width = _screen.width;
  index = offset / 4;
  screen_y1 = index / width;
  screen_x = index % width;

  index = (offset + len) / 4;
  screen_y2 = index / width;

  assert(screen_y2 >= screen_y1);
  if(screen_y2 == screen_y1)
  {
    _draw_rect(buf,screen_x,screen_y1,len / 4,1);
    return;
  }

  int tempw = width - screen_x;
  if(screen_y2 - screen_y1 == 1)
  {
    _draw_rect(buf,screen_x,screen_y1,tempw,1);
    _draw_rect(buf + tempw * 4,0,screen_y2,len / 4 - tempw,1);
    return;
  }
  _draw_rect(buf,screen_x,screen_y1,tempw,1);
  int temph = screen_y2 - screen_y1 - 1;
  _draw_rect(buf + tempw * 4,0,screen_y1 + 1,width,temph);
  _draw_rect(buf + tempw * 4 + temph * width * 4,0,screen_y2,len / 4 - tempw - temph * width,1);
  
 /*
  assert(offset % 4 == 0 && len % 4 == 0);
  int index,screen_x,screen_y;
  int w = _screen.width;
  //int h = _screen.height;
  for(int i = 0;i < len / 4;i++)
  {
    index = offset / 4 + i;
    screen_y = index / w;
    screen_x = index % w;
    _draw_rect(buf + i * 4,screen_x,screen_y,1,1);
  }
  */
}

void init_device() {
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  //Log("init device begin\n");
  sprintf(dispinfo,"WIDTH : %d\nHEIGHT : %d\n",_screen.width,_screen.height);

}
