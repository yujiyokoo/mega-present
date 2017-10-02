/*! @file
  @brief
  If you want to add your own extension,
  add your code in c_ext.c and c_ext.h. 

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRBC_SRC_C_EXT_H_
#define MRBC_SRC_C_EXT_H_

#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif


void mrbc_init_class_extension(mrb_vm *vm);

#ifdef __cplusplus
}
#endif
#endif
