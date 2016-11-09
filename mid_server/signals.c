#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "signals.h"
#include "tlv.h"

tlv_chain* encode_join(Signal type,uint16_t num_groups, uint16_t *group_list)
{
  if(num_groups == 0 || group_list==NULL)
    return NULL;
  char *str,*t;
  char string[20]={0};
  int i;
  tlv_chain *a;
  a= (tlv_chain*) malloc(sizeof(tlv_chain));
  init_tlv_chain(a);

  str = (char *)malloc((num_groups+1)*sizeof(uint16_t));
  t=str;
  *(uint16_t *)t = htons(num_groups);
  t+=2;
  for(i=0;i<num_groups;++i)
  {
    *(uint16_t *)t = htons(*(group_list+i));
    t+=2;
  }
  strncpy(string,str,(num_groups+1)*2);
  t=str;
/*  for(i=0;i<=num_groups;++i)
  {
    printf("%d", *((uint16_t *)t));
    t+=2;
  }*/
  tlv_chain_add_raw(a,type,(num_groups+1)*2,str);
  free(str);
  return a;
}

void decode_join_request(tlv *tlv,uint16_t *num_groups,uint16_t **group_list)
{
  if(group_list == NULL)
    return;

  if(tlv == NULL)
    return;

  uint16_t size,*group;
  int i;
  char *raw_data;
  raw_data = tlv->data;
  if(raw_data == NULL)
    return;

  size = tlv->size;
  if(size < 2)
    return;
  (*num_groups) = ntohs(*(uint16_t *)raw_data);
  raw_data+=2;
  size-=2;

  printf("\n number of grps: %d...",*(num_groups));
  if(size < (*num_groups)*2)
  {
    printf("\n different sizes... size: %d num_groups: %d",size,num_groups);
    return;
  }

  *group_list = (uint16_t *)malloc(sizeof(uint16_t)*(*num_groups));
  group = *group_list;

  for(i=0;i<(*num_groups);++i)
  {
    *group = ntohs(*(uint16_t *)raw_data);
    raw_data+=2;
    group+=1;
  }

  group = *group_list;
}
