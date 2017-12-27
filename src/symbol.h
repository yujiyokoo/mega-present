/*! @file
  @brief
  Symbol

  <pre>
  Copyright (C) 2015-2016 Kyushu Institute of Technology.
  Copyright (C) 2015-2016 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_SYMBOL_H_
#define MRBC_SRC_SYMBOL_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

mrb_sym add_sym(const char *str);
mrb_sym str_to_symid(const char *str);
const char* symid_to_str(mrb_sym sym_id);

void debug_all_symbols(void);

#ifdef __cplusplus
}
#endif
#endif
