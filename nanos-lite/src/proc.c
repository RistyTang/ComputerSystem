#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);
  // TODO: remove the following three lines after you have implemented _umake()
  //_switch(&pcb[i].as);
  //current = &pcb[i];
  //((void (*)(void))entry)();
  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

int current_game = 0;
void switch_current_game()
{
  current_game = 2 - current_game;
  if(current_game == 0)
  {
    Log("current game is xianjian");
  }
  else
  {
    Log("current game is video");
  }
  
}

_RegSet* schedule(_RegSet *prev) 
{
  /*
  if(current != NULL)
  {
    current->tf = prev;
  }
  current = &pcb[0];
  const int times = 100;
  static int curtime = 0;
  if(current == &pcb[0])
  {
    curtime += 1;
  }
  else
  {
    current = &pcb[0];
  }
  if(curtime == times)
  {
    current = &pcb[1];
    curtime = 0;
  }
  //current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  _switch(&current->as);
  return current->tf;
  //return NULL;
  */
  if(current != NULL)
  {
    current->tf = prev;
  }
  else
  {
    current = &pcb[current_game];
  }
  const int times = 100;
  static int curtime = 0;
  if(current == &pcb[current_game])
  {
    curtime += 1;
  }
  else
  {
    current = &pcb[current_game];
  }
  if(curtime == times)
  {
    current = &pcb[1];
    curtime = 0;
  }
  _switch(&current->as);
  return current->tf;
}
