/*! @file
  @brief
  Manage global objects.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRUBYC_SRC_GLOBAL_H_
#define MRUBYC_SRC_GLOBAL_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct GLOBAL_OBJECT {
  mrb_sym sym_id;
  mrb_object obj;
} mrb_globalobject;

void global_object_add(mrb_sym sym_id, mrb_object *obj);
mrb_object global_object_get(mrb_sym sym_id);


#ifdef __cplusplus
}
#endif
#endif
