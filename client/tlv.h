#ifndef __TLV_H__
#define __TLV_H__
//#include <stdio.h>
#include <stdlib.h>
//#include <string.h>
#include <stdint.h>

#define MAX_TLV_OBJECT 2

typedef struct tlv
{
  uint8_t type;
  uint8_t *data;
  uint16_t size;
}tlv;

typedef struct tlv_chain
{
  tlv object[MAX_TLV_OBJECT];
  uint8_t used;
}tlv_chain;

typedef enum tlv_result
{
  TLV_SUCCESS,
  TLV_ERROR_UNINITIALISED,
  TLV_ERROR_OUT_OF_MEMORY,
  TLV_SERVER_REJECT
}tlv_result;

typedef enum signal
{
  NONE,
  JOIN_REQUEST,
  JOIN_ACCEPT,
  JOIN_REJECT,
  DISJOIN,
  QUES_SOLVE,
  QUES_ANSWER,
  QUES_ERROR_INVALID,
  QUES_ERROR_LIMITED_RESOURCE,
  QUES_ERROR_ANSWER_INVALID,
  QUES_ERROR_OTHER,
  MAX_NONE
}signal;

tlv_result init_tlv_chain(tlv_chain *a);
tlv_result tlv_chain_add_raw(tlv_chain *a, unsigned char type, uint16_t size, const unsigned char *raw_data);
tlv_result tlv_chain_serialise(tlv_chain *a, unsigned char *dest, int32_t *count);
tlv_result tlv_chain_deserialise(const unsigned char *source, tlv_chain *dest, int32_t length);
int32_t tlv_chain_print(tlv_chain *a);
int32_t tlv_chain_free(tlv_chain *a);
tlv_chain* encode_join_request(uint16_t num_groups, uint16_t *group_list);

#endif /* __TLV_H__ */
