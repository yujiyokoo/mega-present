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

#define MRBC_STRING_C_STR(p) ((p)->handle->str)


void mrbc_init_class_string(mrb_vm *vm);

mrb_value * mrbc_string_constructor(mrb_vm *vm, const char *src);
mrb_value * mrbc_string_constructor_w_len(mrb_vm *vm, const char *src, int len);
void mrbc_string_destructor(mrb_value *target);

#ifdef __cplusplus
}
#endif
#endif
