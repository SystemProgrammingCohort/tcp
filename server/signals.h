#ifndef __SIGNALS_H__
#define __SIGNALS_H__

#include "tlv.h"

typedef enum Signal
{
  NONE,
  JOIN_REQUEST,
  JOIN_ACCEPT,
  QUES_SOLVE,
  QUES_ANSWER,
  QUES_ERROR_INVALID,
  QUES_ERROR_LIMITED_RESOURCE,
  QUES_ERROR_ANSWER_INVALID,
  QUES_ERROR_OTHER,
  INVALID_CLIENT,
  MAX_NONE
}Signal;

tlv_chain* encode_join(Signal type,uint16_t num_groups, uint16_t *group_list);
void decode_join_request(tlv *tlv,uint16_t *num_groups,uint16_t **group_list);

#endif /*__SIGNALS_H__*/
