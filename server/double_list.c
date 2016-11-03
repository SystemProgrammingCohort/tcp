#include<stdio.h>
#include<stdlib.h>
#include "double_list.h"

struct dnode* add_dnode(struct double_list *list,int group_number,void *val)
{
  if(val==NULL || list==NULL)
    return NULL;
  struct dnode *t;
  t=(struct dnode *)malloc(sizeof(struct dnode));
  t->group_number = group_number;
  t->val = val;
  t->prev = NULL;
  t->next = NULL;

  if(list->first == NULL)
  {
    list->first = t;
    list->last = t;
    return t;
  }
  list->last->next = t;
  t->prev = list->last;
  list->last = t;
  return t;
}

void init_list(struct double_list *list)
{
  list->first = NULL;
  list->last = NULL;
}

int delete_dnode(struct double_list *list, struct dnode **target_node)
{
  struct dnode *t;
  t= *target_node;
  if(target_node == NULL)
    return 0;
  if(*target_node == NULL)
    return 0;
  t= *target_node;
  if(t->prev == NULL && t->next == NULL)
  {
    list->first = list->last = NULL;
    free(t);
    *target_node = NULL;
    return 1;
  }
  if(t->prev == NULL)
  {
    list->first = t->next;
    t->next->prev = NULL;
    free(t);
    *target_node = NULL;
    return 1;
  }
  if(t->next == NULL)
  {
    list->last = t->prev;
    t->prev->next = NULL;
    free(t);
    *target_node = NULL;
    return 1;
  }
  t->prev->next = t->next;
  t->next->prev = t->prev;
  return 1;
}

void display_list(struct double_list *list)
{
  printf("\n List:\n");
  if(list == NULL)
    return;
  struct dnode *t = list->first;
  while(t)
  {
    printf("%x:%d ",t->val,*((int *)(t->val)));
    t=t->next;
  }
  printf("\n");
}
