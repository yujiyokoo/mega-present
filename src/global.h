/*! @file
  @brief
  Manage global objects.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_GLOBAL_H_
#define MRBC_SRC_GLOBAL_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OBJECT_WITH_SYMBOL\
  mrb_sym sym_id;\
  mrb_object obj

typedef struct GLOBAL_OBJECT {
  OBJECT_WITH_SYMBOL;
} mrb_globalobject;

typedef struct CONST_OBJECT {
  OBJECT_WITH_SYMBOL;
} mrb_constobject;

void global_object_add(mrb_sym sym_id, mrb_object *obj);
mrb_object global_object_get(mrb_sym sym_id);

void const_add(mrb_sym sym_id, mrb_object *obj);
mrb_object const_get(mrb_sym sym_id);

#ifdef __cplusplus
}
#endif
#endif
