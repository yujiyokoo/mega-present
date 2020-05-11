/*! @file
  @brief
  mrubyc memory management.

  <pre>
  Copyright (C) 2015-2020 Kyushu Institute of Technology.
  Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Memory management for objects in mruby/c.

  </pre>
*/

#ifndef MRBC_SRC_ALLOC_H_
#define MRBC_SRC_ALLOC_H_

#ifdef __cplusplus
extern "C" {
#endif

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
#if defined(MRBC_ALLOC_LIBC)
#include <stdlib.h>
#endif

/***** Local headers ********************************************************/
/***** Constant values ******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
struct VM;

/***** Global variables *****************************************************/
/***** Function prototypes **************************************************/
#if !defined(MRBC_ALLOC_LIBC)
void mrbc_init_alloc(void *ptr, unsigned int size);
void mrbc_cleanup_alloc(void);
void *mrbc_raw_alloc(unsigned int size);
void *mrbc_raw_alloc_no_free(unsigned int size);
void mrbc_raw_free(void *ptr);
void *mrbc_raw_realloc(void *ptr, unsigned int size);
int is_allocated_memory(void *tgt);
void mrbc_free_all(const struct VM *vm);
void mrbc_set_vm_id(void *ptr, int vm_id);
int mrbc_get_vm_id(void *ptr);

// for statistics or debug. (need #define MRBC_DEBUG)
void mrbc_alloc_statistics(int *total, int *used, int *free, int *fragmentation);
int mrbc_alloc_vm_used(int vm_id);
#endif


/***** Inline functions *****************************************************/

#if defined(MRBC_ALLOC_LIBC)
/*
  use the system (libc) memory allocator.
*/
#if defined(MRBC_ALLOC_VMID)
#error "Can't use MRBC_ALLOC_LIBC with MRBC_ALLOC_VMID"
#endif
static inline void mrbc_init_alloc(void *ptr, unsigned int size) {}
static inline void mrbc_cleanup_alloc(void) {}
static inline void *mrbc_raw_alloc(unsigned int size) {
  return malloc(size);
}
static inline void *mrbc_raw_alloc_no_free(unsigned int size) {
  return malloc(size);
}
static inline void mrbc_raw_free(void *ptr) {
  free(ptr);
}
static inline void *mrbc_raw_realloc(void *ptr, unsigned int size) {
  return realloc(ptr, size);
}
static inline void *mrbc_alloc(const struct VM *vm, unsigned int size) {
  return malloc(size);
}
static inline void mrbc_free_all(const struct VM *vm) {}
static inline void mrbc_set_vm_id(void *ptr, int vm_id) {}
static inline int mrbc_get_vm_id(void *ptr) {
  return 0;
}
#endif


//================================================================
/*! allocate memory
*/
#if defined(MRBC_ALLOC_VMID)
void *mrbc_alloc(const struct VM *vm, unsigned int size);
#else
static inline void * mrbc_alloc(const struct VM *vm, unsigned int size)
{
  return mrbc_raw_alloc(size);
}
#endif


//================================================================
/*! re-allocate memory
*/
static inline void * mrbc_realloc(const struct VM *vm, void *ptr, unsigned int size)
{
  return mrbc_raw_realloc(ptr, size);
}


//================================================================
/*! release memory
*/
static inline void mrbc_free(const struct VM *vm, void *ptr)
{
  mrbc_raw_free(ptr);
}


#ifdef __cplusplus
}
#endif
#endif
