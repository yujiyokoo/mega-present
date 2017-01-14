/*! @file
  @brief
  mrubyc memory management.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Memory management for objects in mruby/c.

  </pre>
*/

#ifndef MRBC_SRC_ALLOC_H_
#define MRBC_SRC_ALLOC_H_

#include <stdint.h>
#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

void mrbc_init_alloc(void);
uint8_t *mrbc_raw_alloc(uint32_t size);
void mrbc_raw_free(void *ptr);

void mrbc_alloc_debug(void);

uint8_t *mrbc_alloc(mrb_vm *vm, int size);
void mrbc_free(mrb_vm *vm, void *ptr);


#ifdef __cplusplus
}
#endif
#endif
