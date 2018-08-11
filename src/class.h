/*! @file
  @brief

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

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


mrbc_class *find_class_by_object(struct VM *vm, mrbc_object *obj);
mrbc_proc *find_method(struct VM *vm, mrbc_value recv, mrbc_sym sym_id);
mrbc_class *mrbc_define_class(struct VM *vm, const char *name, mrbc_class *super);
mrbc_class *mrbc_get_class_by_name(const char *name);
void mrbc_define_method(struct VM *vm, mrbc_class *cls, const char *name, mrbc_func_t cfunc);
void mrbc_funcall(struct VM *vm, const char *name, mrbc_value *v, int argc);
mrbc_value mrbc_send(struct VM *vm, mrbc_value *v, int reg_ofs, mrbc_value *recv, const char *method, int argc, ...);
void c_proc_call(struct VM *vm, mrbc_value v[], int argc);
void c_ineffect(struct VM *vm, mrbc_value v[], int argc);
void mrbc_init_class(void);


#ifdef __cplusplus
}
#endif
#endif
