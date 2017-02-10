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
#include "vm.h"    // for mruby/c

#ifdef __cplusplus
extern "C" {
#endif

void mrbc_init_alloc(uint8_t *ptr, unsigned int size );
uint8_t *mrbc_raw_alloc(uint32_t size);
uint8_t *mrbc_raw_realloc(uint8_t *ptr, uint32_t size);
void mrbc_raw_free(void *ptr);

void mrbc_alloc_debug(void);

// for mruby/c
uint8_t *mrbc_alloc(mrb_vm *vm, int size);
uint8_t *mrbc_realloc(mrb_vm *vm, void *ptr, int size);
void mrbc_free(mrb_vm *vm, void *ptr);
void mrbc_free_all(mrb_vm *vm);

#ifdef __cplusplus
}
#endif
#endif
