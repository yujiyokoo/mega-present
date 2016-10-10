/*! @file
  @brief
  mrubyc memory management.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Memory management for objects in mruby/c.
  All objects are managed by reference counter.

  </pre>
*/


#ifndef MRUBYC_SRC_ALLOC_H_
#define MRUBYC_SRC_ALLOC_H_

#include <stdint.h>
#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

void mrbc_init_alloc(void);
uint8_t *mrbc_alloc(mrb_vm *vm, int size);
void mrbc_free(mrb_vm *vm, void *ptr);
void mrbc_compact(int vm_id);
void mrbc_inc_ref(uint8_t *ptr);
void mrbc_dec_ref(uint8_t *ptr);

// assignment
// dst = src
// dst != NULL
#define mrbc_copy(dst,src) {mrbc_inc_ref(src);mrbc_dec_ref(dst);dst=src;}

// assignment
// dst = src
// dst is not initialized
#define mrbc_overwrite(dst,src) {mrbc_inc_ref(src);dst=src;}

// debug
void disp_page(int page);
void disp_memory(void);

#ifdef __cplusplus
}
#endif
#endif

