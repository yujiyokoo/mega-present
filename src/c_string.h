/*! @file
  @brief
  mruby/c String object

  <pre>
  Copyright (C) 2015-2017 Kyushu Institute of Technology.
  Copyright (C) 2015-2017 Shimane IT Open-Innovation Center.

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

#define MRBC_STRING_CSTR(p) ((p)->handle->str)


void mrbc_init_class_string(mrb_vm *vm);

mrb_value mrbc_string_new(mrb_vm *vm, const char *src, int len);
mrb_value mrbc_string_new_cstr(mrb_vm *vm, const char *src);
mrb_value mrbc_string_new_alloc(mrb_vm *vm, char *buf, int len);
void mrbc_string_delete(mrb_vm *vm, mrb_value *v);

#ifdef __cplusplus
}
#endif
#endif
