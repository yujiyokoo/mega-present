/*! @file
  @brief
  mruby/c String object

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_C_STRING_H_
#define MRBC_SRC_C_STRING_H_

#include <stdint.h>
#include <string.h>
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif

//================================================================
/*!@brief
  Define String handle.
*/
typedef struct RString {
  MRBC_OBJECT_HEADER;

  uint16_t size;	//!< string length.
  uint8_t *data;	//!< pointer to allocated buffer.

} mrb_string;


struct VM;

mrb_value mrbc_string_new(struct VM *vm, const void *src, int len);
mrb_value mrbc_string_new_cstr(struct VM *vm, const char *src);
mrb_value mrbc_string_new_alloc(struct VM *vm, void *buf, int len);
void mrbc_string_delete(mrb_value *str);
void mrbc_string_clear_vm_id(mrb_value *str);
mrb_value mrbc_string_add(struct VM *vm, mrb_value *s1, mrb_value *s2);
int mrbc_string_append(struct VM *vm, mrb_value *s1, mrb_value *s2);
void mrbc_init_class_string(mrb_vm *vm);


//================================================================
/*! compare
*/
static inline int mrbc_string_compare(const mrb_value *v1, const mrb_value *v2)
{
  if( v1->string->size != v2->string->size ) return 0;
  return !memcmp(v1->string->data, v2->string->data, v1->string->size);
}

//================================================================
/*! get size
*/
static inline int mrbc_string_size(const mrb_value *str)
{
  return str->string->size;
}

//================================================================
/*! get c-language string (char *)
*/
static inline char * mrbc_string_cstr(const mrb_value *v)
{
  return (char*)v->string->data;
}


#ifdef __cplusplus
}
#endif
#endif
