/*! @file
  @brief
  Declare static data.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_STATIC_H_
#define MRUBYC_SRC_STATIC_H_

#include "vm.h"
#include "global.h"
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif


/* VM */
extern mrb_vm *static_pool_vm;

/* IREP */
extern mrb_irep *static_pool_irep;

/* Object */
extern mrb_object *static_pool_object;

/* Class */
extern mrb_class *static_pool_class;

/* Proc */
extern mrb_proc *static_pool_proc;

/* Class Tree */
extern mrb_class *static_class_object;

extern mrb_class *static_class_false;
extern mrb_class *static_class_true;
extern mrb_class *static_class_nil;
extern mrb_class *static_class_array;
extern mrb_class *static_class_fixnum;
#if MRUBYC_USE_FLOAT
extern mrb_class *static_class_float;
#endif

extern mrb_constobject static_const[];
/* Global Objects */
extern mrb_globalobject static_global[];

void init_static(void);


#ifdef __cplusplus
}
#endif
#endif
