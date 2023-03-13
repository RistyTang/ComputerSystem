#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

//here 调用cpuexec
static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

//新增调试器实现，cmd_si继续执行一些指令
static int cmd_si(char *args) {
  int steps;//继续执行的步数
  if(args==NULL)
  {
    steps=1;
  }
  else
  {
    sscanf(args,"%d",&steps);
  }
  if(steps<=0)
  {
    printf("wrong input,check your steps\n");
    return 1;//区分开
  }
  cpu_exec(steps);
  return 0;
}

static int cmd_info(char *args) {//打印寄存器
  if(args[0]=='r')//registers
  {
    printf("-----32bit regs------\n");
    printf("eax : %x \n",cpu.eax);
    printf("ecx : %x \n",cpu.ecx);
    printf("edx : %x \n",cpu.edx);
    printf("ebx : %x \n",cpu.ebx);
    printf("esp : %x \n",cpu.esp);
    printf("ebp : %x \n",cpu.ebp);
    printf("esi : %x \n",cpu.esi);
    printf("edi : %x \n",cpu.edi);
    printf("eip : %x \n",cpu.eip);
    printf("-----16bit regs-----\n");
    for(int i=0;i<8;i++)
    {
      printf("%s : %x \n",regsw[i],reg_w(i));
    }
    printf("-----8bit regs-----\n");
    for(int i=0;i<8;i++)
    {
      printf("%s : %x \n",regsb[i],reg_b(i));
    }

  }
  else if(args[0]=='w')//monitors
  {

    //return 0;
  }
  return 0;
}

static int cmd_p(char * args) {//表达式求值

}

static int cmd_x(char * args) {//扫描内存

}

static int cmd_w(char * args) {//设置监视点

}

static int cmd_d(char * args) {//删除监视点

}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  {"si","Continue several executions next of the program",cmd_si },
  {"info","print information of regs or moniors",cmd_info },
  {"p","calculate the results of exprs",cmd_p},
  {"x","scan the memory",cmd_x},
  {"w","set a monitor",cmd_w},
  {"d","delete a monitor",cmd_d}

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
