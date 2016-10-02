/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_CLASS_H_
#define MRUBYC_SRC_CLASS_H_

#include <stdint.h>
#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif


mrb_class *find_class_by_object(struct VM *vm, mrb_object *obj);
mrb_proc *find_method(struct VM *vm, mrb_value recv, mrb_sym sym_id);


void mrb_init_class(void);
  void mrb_define_method(struct VM *vm, mrb_class *cls, const char *name, mrb_func_t func);
  void mrb_define_method_proc(struct VM *vm, mrb_class *cls, mrb_sym sym_id, mrb_proc *rproc);


#ifdef __cplusplus
}
#endif
#endif
