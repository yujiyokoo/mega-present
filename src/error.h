/*! @file
  @brief
  exception 

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRBC_SRC_ERROR_H_
#define MRBC_SRC_ERROR_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif


void mrbc_init_class_exception(struct VM *vm);


#define mrbc_israised(v) (!((v)->exc.tt == MRBC_TT_NIL))

#ifdef __cplusplus
}
#endif
#endif
