/*! @file
  @brief
  mruby/c String object

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_C_STRING_H_
#define MRBC_SRC_C_STRING_H_

#include <stdint.h>
#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

//================================================================
/*!@brief
  Define String handle.
*/
typedef struct MrbcHandleString {
  uint16_t ref_count;	// TODO: not use yet.
  mrb_vtype tt : 8;	// TODO: for debug use.

  uint16_t size;	//!< string length.
  uint8_t *str;		//!< pointer to allocated buffer.

} MrbcHandleString;


#define MRBC_STRING_CSTR(p) ((char*)(p)->h_str->str)


mrb_value mrbc_string_new(mrb_vm *vm, const void *src, int len);
mrb_value mrbc_string_new_cstr(mrb_vm *vm, const char *src);
mrb_value mrbc_string_new_alloc(mrb_vm *vm, void *buf, int len);
void mrbc_string_delete(mrb_vm *vm, mrb_value *v);
void mrbc_init_class_string(mrb_vm *vm);

#ifdef __cplusplus
}
#endif
#endif
