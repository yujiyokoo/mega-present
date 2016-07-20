/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_C_STRING_H_
#define MRUBYC_SRC_C_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void mrb_init_class_string(void);

char* mrb_string_dup(const char *str);

#ifdef __cplusplus
}
#endif
#endif

