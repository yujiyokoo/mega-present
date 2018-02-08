/*! @file
  @brief
  mruby/c Range object

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_C_RANGE_H_
#define MRBC_SRC_C_RANGE_H_

#include <stdint.h>
#include "vm.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

//================================================================
/*!@brief
  Define Range object (same the handles of other objects)
*/
typedef struct MrbcHandleRange {
  uint16_t ref_count;	// TODO: not use yet.
  mrb_vtype tt : 8;	// TODO: for debug use.

  uint8_t flag_exclude;	// true: exclude the end object, otherwise include.
  mrb_value first;
  mrb_value last;

} MrbcHandleRange;


mrb_value mrbc_range_new(mrb_vm *vm, mrb_value *v_first, mrb_value *v_last, int flag_exclude);
void mrbc_range_delete(mrb_value *v);
void mrbc_range_clear_vm_id(mrb_value *v);

void mrbc_init_class_range(mrb_vm *vm);


static inline int mrbc_range_compare(const mrb_value *v1, const mrb_value *v2)
{
  return( mrbc_eq( &v1->h_range->first, &v2->h_range->first ) &&
	  mrbc_eq( &v1->h_range->last, &v2->h_range->last ) &&
	  v1->h_range->flag_exclude == v2->h_range->flag_exclude );
}

#ifdef __cplusplus
}
#endif
#endif
