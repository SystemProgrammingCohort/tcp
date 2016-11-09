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
  printf("\n length: %d",length);
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
    printf("\n vurrent counter: %d",counter);
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
    a->object[i].size = 0;
    a->object[i].type = -1;
    free(a->object[i].data);
    a->object[i].data = NULL;
  }
  a->used = 0;
  return TLV_SUCCESS;
}
