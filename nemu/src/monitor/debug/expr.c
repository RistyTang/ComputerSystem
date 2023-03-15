#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_HEX,TK_OCTAL,TK_DECIMIAL,
  TK_NEQ,TK_REG

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"!=", TK_NEQ},        // not equal
  {"0[X|x][0-9A-Fa-f]*",TK_HEX}, //hexadecimial
  {"0[0-7]*",TK_OCTAL},  //8
  {"0|[1-9][0-9]*",TK_DECIMIAL}, //10
  {"\\-", '-'},          // sub
  {"\\*", '*'},          // mul
  {"\\/", '/'},          // div
  {"\\(", '('},          // (
  {"\\)", ')'},          // )
  {"\\$(eax|ebx|ecx|edx|esp|ebp|esi|edi|eip|ax|bx|cx|dx|sp|bp|si|di|al|bl|cl|dl|ah|bh|ch|dh)", TK_REG}

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];//溢出怎么做
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;//第几个token

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        //记录到token数组中
        const char * endzero='\0';
        switch (rules[i].token_type) 
        {
          case TK_DECIMIAL://10
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            strcat(tokens[nr_token].str,endzero);//末尾添加\0
            break;
          case TK_HEX://16
            strncpy(tokens[nr_token].str,substr_start+2,substr_len-2);//去除0x
            strcat(tokens[nr_token].str,endzero);
            break;
          case TK_OCTAL://8
            strncpy(tokens[nr_token].str,substr_start+1,substr_len-1);//去除0
            strcat(tokens[nr_token].str,endzero);
            break;
          case TK_REG://registers
            strncpy(tokens[nr_token].str,substr_start+1,substr_len-1);//去除$
            strcat(tokens[nr_token].str,endzero);
            break;
          case  TK_NOTYPE://空格
            break;
          default: 
            //TODO();
            break;
        }
        printf("nr_token = %d , str = %s , token_type = %d \n",nr_token,tokens[nr_token].str,tokens[nr_token].type);
        nr_token+=1;//新token
        break;
      }
    }

    if (i == NR_REGEX) {//无对应token
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
