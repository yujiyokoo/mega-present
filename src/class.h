/*! @file
  @brief

  <pre>
  Copyright (C) 2015-2017 Kyushu Institute of Technology.
  Copyright (C) 2015-2017 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRBC_SRC_CLASS_H_
#define MRBC_SRC_CLASS_H_

#include <stdint.h>
#include "vm.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

mrb_class *find_class_by_object(struct VM *vm, mrb_object *obj);
mrb_proc *find_method(struct VM *vm, mrb_value recv, mrb_sym sym_id);

void mrbc_init_class(void);
void mrbc_define_method(struct VM *vm, mrb_class *cls, const char *name, mrb_func_t func);


static inline mrb_class * mrbc_define_class( mrb_vm *vm, const char *name, mrb_class *super )
{
  return mrbc_class_alloc( vm, name, super );
}


#ifdef __cplusplus
}
#endif
#endif
