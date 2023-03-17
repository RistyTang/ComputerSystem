#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) 
  {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;//正在使用的监视点
  free_ = wp_pool;//空闲的
}

/* TODO: Implement the functionality of watchpoint */
WP* get_wp(int num)//根据传入的序号找到该监视点
{
  if(head==NULL)
  {
    //printf("there is no watchpoint\n");
    return NULL;
  }
  WP* tempwp=head;
  while(tempwp->next!=NULL)
  {
    if(tempwp->NO==num)
    {
      break;
    }
    tempwp=tempwp->next;
  }
  if(tempwp->NO==num)
  {
    return tempwp;
  }
  return NULL;
}

WP* new_wp(char * args)//分配一个监视点
{
  WP* choosepoint=free_;
  if(free_==NULL)//没有空闲的监视点
  {
    printf("there's no empty watchpoint left\n");
    assert(0);
  }
  else
  {
    //从free链表中取出
    //choosepoint=free_;
    free_=free_->next;
    choosepoint->next=NULL;
    WP* temp=head;
    //链接到head链表上
    //printf("wrong here or not\n");
    if(head==NULL)
    {
      head=choosepoint;
    }
    else
    {
      while(temp->next!=NULL)
      {
        temp=temp->next;
      }
      temp->next=choosepoint;
    }
  }
  //设置控制点相关的值
  strcpy(choosepoint->expr,args);
  bool flags=false;
  choosepoint->value=expr(args,&flags);
  return choosepoint;
}

void free_wp(WP* wp)//将wp归还到free表中
{
  //从head表中取出
  WP* tempinhead=head;
  if(head==wp)
  {
    head=head->next;
  }
  else
  {
    while(tempinhead->next!=wp)
    {
      tempinhead=tempinhead->next;
    }
    tempinhead->next=wp->next;
  }
  //放入free表
  tempinhead->next=free_;
  free_=tempinhead;
  printf("you have delete watchpoint %d successfully\n",wp->NO);
}

void print_wp()//打印所有监视点信息
{
  WP* temp=head;
  if(temp==NULL)
  {
    printf("there is no watchpoint now\n");
    return;
  }
  while(temp)
  {
    printf("No.%d watchpoint : expr->%s  value->%d\n",temp->NO,temp->expr,temp->value);
  }
}

