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
  TK_NEQ,TK_REG,
  TK_MINUS,TK_NOT,TK_ASTERISK,//'-'  '!' '*'
  TK_AND,TK_OR//&& ||
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
  {"0x[0-9A-Fa-f]*|0X[0-9A-Fa-f]*",TK_HEX}, //hexadecimial
  {"[1-9][0-9]*|0",TK_DECIMIAL}, //10
  
  {"0[0-7]*",TK_OCTAL},  //8
  {"\\-", '-'},          // sub
  {"\\*", '*'},          // mul
  {"\\/", '/'},          // div
  {"\\(", '('},          // (
  {"\\)", ')'},          // )
  {"\\$(eax|ebx|ecx|edx|esp|ebp|esi|edi|eip|ax|bx|cx|dx|sp|bp|si|di|al|bl|cl|dl|ah|bh|ch|dh)", TK_REG},
  {"!", TK_NOT},
  {"&&", TK_AND},
  {"\\|\\|", TK_OR}
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
        switch (rules[i].token_type) 
        {
          case TK_DECIMIAL://10
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case TK_HEX://16
            strncpy(tokens[nr_token].str,substr_start+2,substr_len-2);//去除0x
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case TK_OCTAL://8
            strncpy(tokens[nr_token].str,substr_start+1,substr_len-1);//去除0
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case TK_REG://registers
            strncpy(tokens[nr_token].str,substr_start+1,substr_len-1);//去除$
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case  TK_NOTYPE://空格
            break;
          case '+':
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case '-':
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case '*':
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case '/':
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case TK_AND:
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case TK_OR:
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case TK_EQ:
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case TK_NEQ:
            strncpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          default: 
            //TODO();
            break;
        }
        tokens[nr_token].type=rules[i].token_type;//一定要加上这句话
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
bool check_parentheses(int p,int q) 
{ //根据指导手册，表达式必须被一对匹配的括号包括在内
  if(p>=q)//wrong input0
  {
    //printf("there's no expression \n");
    return false;
  }
  //wrong input1
  else if((tokens[p].type!='(')||(tokens[q].type!=')'))
  {
    //printf("not surrounded by parenthesis \n");
    return false;
  }
  int leftcnt=0;
  int rightcnt=0;
  for(int i=p;i<q;i++)
  {
    if(tokens[i].type=='(')
    {
      leftcnt+=1;
    }
    else if(tokens[i].type==')')
    {
      rightcnt+=1;
    }
    if(leftcnt==rightcnt)//wrong input 3
    {
      printf("the left most ( does not match the right most ) \n");
      return false;
    }
  }
  rightcnt+=1;
  //wrong input 2
  if(leftcnt!=rightcnt)
  {
    printf("bad expression, check if your '('s and ')'s are the same \n");
    return false;
  }
  return true;
}

int find_dominant_oprator(int p, int q)//找到求值时最后一个计算的运算符
{
  //此表达式一定不是（expr)形式
  int level=0;//辨别括号优先级
  //表达式优先级：非负15 乘除14 加减13
  int dominant_operator=100;//记录选中的运算符是什么
  int revalue=p-1;//返回的是选中的token下标
  //printf("p=%d,q=%d\n",p,q);
  for(int i=p;i<=q;i++)
  {
    if(tokens[i].type=='(')
    {
      level+=1;
    }
    else if(tokens[i].type==')')
    {
      level-=1;
    }
    if(level!=0)//凡是括号内内容都不考虑，
    {
      continue;
    }
    //非，负，取值
    if(dominant_operator>=15)
    {
      if(tokens[i].type==TK_MINUS||tokens[i].type==TK_NOT||tokens[i].type==TK_ASTERISK)
      {
        dominant_operator=15;
        revalue=i;
      }
    }
    //乘除法
    if(dominant_operator>=14)
    {
      if(tokens[i].type == '*'||tokens[i].type == '/')
      {
        dominant_operator=14;
        revalue=i;
      }
    }
    //加减法
    if(dominant_operator >= 13)
    {
      if(tokens[i].type == '+'||tokens[i].type == '-')
      {
        dominant_operator=13;
        revalue=i;
      }
    }
    // &&
    if((dominant_operator>=12)&&(tokens[i].type==TK_AND))
    {
      dominant_operator=12;
      revalue=i;
    }
    // ||
    if((dominant_operator>=11)&&(tokens[i].type == TK_OR))
    {
      dominant_operator=11;
      revalue=i;
    }
  }
  if(revalue == p-1)
  {
    printf("can't find a legal operator\n");
    assert(0);
    return 0;
  }
  printf("dominant operator is %s\n",tokens[revalue].str);
  return revalue;
}

uint32_t eval(int p,int q)
{
  if(p>q)
  {
    printf("wrong expression in eval\n");
    assert(0);
  }
  else if(p==q)//number
  {
    uint32_t revalue;
    switch (tokens[p].type)
    {
    case TK_DECIMIAL://10
      sscanf(tokens[p].str,"%d",&revalue);
      return revalue;
    case TK_HEX://16
      sscanf(tokens[p].str,"%x",&revalue);
      return revalue;
    case TK_OCTAL://8
      sscanf(tokens[p].str,"%o",&revalue);
      return revalue;
    case TK_REG://registers
      for(int i=0;i<8;i++)
      {
        if(strcmp(tokens[p].str,regsl[i])==0)
        {
          return reg_l(i);
        }
        if(strcmp(tokens[p].str,regsw[i])==0)
        {
          return reg_w(i);
        }
        if(strcmp(tokens[p].str,regsb[i])==0)
        {
          return reg_b(i);
        }
        if(strcmp(tokens[p].str,"eip")==0)
        {
          return cpu.eip;
        }
        printf("no matched regs\n");
        assert(0);
      }
    default:
      break;
    }
  }
  else if(check_parentheses(p,q)==true)
  {
    return eval(p+1,q-1);//去除括号
  }
  else
  {
    int op=find_dominant_oprator(p,q);
    uint32_t val2=eval(op+1,q);
    if(tokens[op].type==TK_NOT)//!
    {
      uint32_t tempres=eval(p+1,q);
      if(tempres)
      {
        return 0;
      }
      return 1;
    }
    else if(tokens[op].type==TK_MINUS)//-
    {
      return -(eval(p+1,q));
    }
    else if(tokens[op].type==TK_ASTERISK)//*
    {
      return vaddr_read(val2,4);
    }
    uint32_t val1=eval(p,op-1);
    switch (tokens[op].type)
    {
    case '+':
      return val1 + val2;
    case '-':
      return val1 - val2;
    case '*':
      return val1 * val2;
    case '/':
      return val1 / val2;
    case TK_EQ:
      return val1 == val2;
    case TK_NEQ:
      return val1 != val2;
    case TK_AND:
      return val1 && val2;
    case TK_OR:
      return val1 || val2;
    default:
      printf("no matched operator\n");
      assert(0);
    }

  }
  return -1;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  for(int i=0;i<nr_token;i++)
  {
    if(tokens[i].type=='*'&&(i==0||(tokens[i-1].type!=TK_HEX && tokens[i-1].type!=TK_OCTAL && tokens[i-1].type!= TK_DECIMIAL && tokens[i-1].type!=TK_REG && tokens[i-1].type!=')')))
    {
      tokens[i].type=TK_ASTERISK;
    }
    if(tokens[i].type=='-'&&(i==0||(tokens[i-1].type!=TK_HEX && tokens[i-1].type!=TK_OCTAL && tokens[i-1].type!= TK_DECIMIAL && tokens[i-1].type!=TK_REG && tokens[i-1].type!=')')))
    {
      tokens[i].type=TK_MINUS;
    }
  }
  *success=true;
  return eval(0,nr_token-1);
}