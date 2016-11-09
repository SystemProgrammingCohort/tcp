#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tlv.h"

tlv_result init_tlv_chain(tlv_chain *a)
{
  if(a==NULL)
    return TLV_ERROR_OUT_OF_MEMORY;
  memset(a,0,sizeof(tlv_chain));
  return TLV_SUCCESS;
}

tlv_result tlv_chain_add_raw(tlv_chain *a, unsigned char type, uint16_t size, const unsigned char *raw_data)
{
  if(a==NULL || raw_data == NULL)
    return TLV_ERROR_UNINITIALISED;
  if(a->used == MAX_TLV_OBJECT)
    return TLV_ERROR_OUT_OF_MEMORY;

  int index = a->used;
  a->object[index].type = type;
  a->object[index].size = size;
  a->object[index].data = malloc(size);
  memcpy(a->object[index].data,raw_data,size);

  a->used++;
  return TLV_SUCCESS;
}

tlv_result tlv_chain_serialise(tlv_chain *a, unsigned char *dest, int32_t *count)
{
  if(a==NULL || dest == NULL)
    return TLV_ERROR_UNINITIALISED;

  int32_t counter = 0;
  uint8_t used = a->used,i;

  for(i=0;i<used;++i)
  {
    dest[counter] = a->object[i].type;
    ++counter;
    memcpy(&dest[counter],&a->object[i].size,2);
    counter+=2;
    memcpy(&dest[counter],a->object[i].data,a->object[i].size);
    counter+=a->object[i].size;
  }
  *count = counter;
  return TLV_SUCCESS;
}

tlv_result tlv_chain_deserialise(const unsigned char *source, tlv_chain *dest, int32_t length)
{
  if(source == NULL || dest == NULL)
    return TLV_ERROR_UNINITIALISED;

  if(dest->used != 0)
    return TLV_ERROR_UNINITIALISED;

  int32_t counter = 0;
  uint16_t size=0;
  while(counter < length)
  {
    if(dest->used == MAX_TLV_OBJECT)
      return TLV_ERROR_OUT_OF_MEMORY;

    dest->object[dest->used].type = source[counter];
    ++counter;

    memcpy(&(dest->object[dest->used].size), &source[counter], 2);
    counter+=2;

    size = dest->object[dest->used].size;

    if(size>0)
    {
      dest->object[dest->used].data = malloc(size);
      memcpy(dest->object[dest->used].data, &source[counter], size);
      counter+=size;
    }else {
      dest->object[dest->used].data = NULL;
    }
    ++(dest->used);
  }
  return TLV_SUCCESS;
}

int32_t tlv_chain_print(tlv_chain *a)
{
  if(a==NULL)
    return TLV_ERROR_UNINITIALISED;
  int i,used;
  used = a->used;
  for(i=0;i<used;++i)
  {
    printf("type:%d size:%d data:%s \n",a->object[i].type,a->object[i].size,a->object[i].data);
  }
  return TLV_SUCCESS;
}

int32_t tlv_chain_free(tlv_chain *a)
{
  if(a==NULL)
    return TLV_ERROR_UNINITIALISED;
  int i,used;
  used = a->used;
  for(i=0;i<used;++i)
  {
    a->object[i].type = -1;
    free(a->object[i].data);
    a->object[i].data = NULL;
  }
  a->used = 0;
  return TLV_SUCCESS;
}

tlv_chain* encode_join_request(uint16_t num_groups, uint16_t *group_list)
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
  tlv_chain_add_raw(a,JOIN_REQUEST,(num_groups+1)*2,str);
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
}
