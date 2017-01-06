/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_C_RANGE_H_
#define MRUBYC_SRC_C_RANGE_H_

#include <stdint.h>
#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

mrb_value mrbc_range_new(mrb_vm *vm, mrb_value *v_st, mrb_value *v_ed, int exclude);

void mrbc_init_class_range(mrb_vm *vm);


#ifdef __cplusplus
}
#endif
#endif
