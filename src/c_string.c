/*! @file
  @brief
  mruby/c String object

  <pre>
  Copyright (C) 2015-2017 Kyushu Institute of Technology.
  Copyright (C) 2015-2017 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include <stdlib.h>
#include <string.h>
#include "vm_config.h"
#include "alloc.h"
#include "value.h"
#include "vm.h"
#include "c_string.h"
#include "class.h"
#include "static.h"



//================================================================
/*! constructor

  @param  vm	pointer to VM.
  @param  src	source string or NULL
  @return 	pointer to allocated String object set.
*/
mrb_value * mrbc_string_constructor(mrb_vm *vm, const char *src)
{
  /*
    Allocate handle and string buffer.
    Handle have a reference count with value 1.
  */
  mrb_value *handle = (mrb_value *)mrbc_alloc(vm, sizeof(mrb_value));
  if( !handle ) return NULL;

  int size = (src == NULL)? 8 : strlen(src) + 1;        // default 8 bytes.
  char *mbuf = (char *)mrbc_alloc(vm, size);
  if( !mbuf ) {
    mrbc_free(vm, handle);
    return NULL;
  }

  /*
    Copy a source string.
  */
  if( src == NULL ) {
    mbuf[0] = '\0';
  } else {
    memcpy( mbuf, src, size );
  }

  /*
    Create String object set.
  */
  handle->tt = MRB_TT_STRING;
  handle->str = mbuf;

  return handle;
}



//================================================================
/*! constructor with string length.

  @param  vm	pointer to VM.
  @param  src	source string or NULL
  @param  len	source length
  @return 	pointer to allocated String object set.
*/
mrb_value * mrbc_string_constructor_w_len(mrb_vm *vm, const char *src, int len)
{
  /*
    Allocate handle and string buffer.
    Handle have a reference count with value 1.
  */
  mrb_value *handle = (mrb_value *)mrbc_alloc(vm, sizeof(mrb_value));
  if( !handle ) return NULL;

  char *mbuf = (char *)mrbc_alloc(vm, len+1);
  if( !mbuf ) {
    mrbc_free(vm, handle);
    return NULL;
  }

  /*
    Copy a source string.
  */
  if( src == NULL ) {
    mbuf[0] = '\0';
  } else {
    memcpy( mbuf, src, len );
    mbuf[len] = '\0';
  }

  /*
    Create String object set.
  */
  handle->tt = MRB_TT_STRING;
  handle->str = mbuf;

  return handle;
}



//================================================================
/*! destructor

  @param  target 	pointer to allocated String object set.
*/
void mrbc_string_destructor(mrb_value *target)
{
  mrbc_raw_free(target->str);
  mrbc_raw_free(target);
}



//================================================================
/*! op_add

  @param  vm	pointer to VM.
  @param  reg	pointer to R(A)
*/
void mrbc_string_op_add(mrb_vm *vm, mrb_value *reg)
{
  mrb_value *s1 = reg->obj;
  mrb_value *s2 = (reg+1)->obj;

  int len1 = strlen(s1->str);
  int len2 = strlen(s2->str);
  mrb_value *v = mrbc_string_constructor_w_len(vm, NULL, len1+len2);
  if( !v ) return;

  memcpy( v->str, s1->str, len1 );
  memcpy( v->str+len1, s2->str, len2+1 );

  mrbc_release(vm, reg);
  reg->tt = MRB_TT_STRING;
  reg->obj = v;
}



//================================================================
/*! (method) size, length
*/
static void c_string_size(mrb_vm *vm, mrb_value *v)
{
  int i = strlen(MRBC_STRING_C_STR(v));

  mrbc_release(vm, v);
  SET_INT_RETURN( i );
}



//================================================================
/*! (method) !=
*/
static void c_string_neq(mrb_vm *vm, mrb_value *v)
{
  int result = mrbc_eq(v, v+1);

  mrbc_release(vm, v);
  if( result ) {
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}



//================================================================
/*! (method) to_i
  TODO: to_i(base = 10) only. need 2 to 36.
*/
static void c_string_to_i(mrb_vm *vm, mrb_value *v)
{
  int i = atoi(MRBC_STRING_C_STR(v));

  mrbc_release(vm, v);
  SET_INT_RETURN( i );
}



//================================================================
/*! (method) <<
*/
static void c_string_append(mrb_vm *vm, mrb_value *v)
{
  mrb_value *v2 = &GET_ARG(1);
  int len1 = strlen(MRBC_STRING_C_STR(v));
  int len2 = strlen(MRBC_STRING_C_STR(v2));

  uint8_t *str = mrbc_realloc(vm, MRBC_STRING_C_STR(v), len1+len2+1);
  if( !str ) return;

  memcpy(str + len1, MRBC_STRING_C_STR(v2), len2 + 1);
  v->obj->str = (char *)str;
}



//================================================================
/*! (method) []
*/
static void c_string_slice(mrb_vm *vm, mrb_value *v)
{
  mrb_value *v2 = &GET_ARG(1);
  switch(v2->tt) {
  case MRB_TT_FIXNUM:{
    int len = strlen(MRBC_STRING_C_STR(v));
    int idx = v2->i;
    int ch = 0;
    if( idx >= 0 ) {
      if( idx < len ) {
        ch = *((uint8_t *)MRBC_STRING_C_STR(v) + idx);
      }
    } else {
      idx += len;
      if( idx >= 0 ) {
        ch = *((uint8_t *)MRBC_STRING_C_STR(v) + idx);
      }
    }

    if( ch > 0 ) {
      mrb_value *s = mrbc_string_constructor_w_len(vm, NULL, 1);
      if( !s ) return;
      s->str[0] = ch;
      s->str[1] = '\0';
      mrbc_release(vm, v);
      v->tt = MRB_TT_STRING;
      v->obj = s;
    } else {
      mrbc_release(vm, v);
      SET_NIL_RETURN();
    }
  } break;
  default:
    break;
  }
}



//================================================================
/*! initialize
*/
void mrbc_init_class_string(mrb_vm *vm)
{
  mrbc_class_string = mrbc_class_alloc(vm, "String", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_string, "size", c_string_size);
  mrbc_define_method(vm, mrbc_class_string, "length", c_string_size);
  mrbc_define_method(vm, mrbc_class_string, "!=", c_string_neq);
  mrbc_define_method(vm, mrbc_class_string, "to_i", c_string_to_i);
  mrbc_define_method(vm, mrbc_class_string, "<<", c_string_append);
  mrbc_define_method(vm, mrbc_class_string, "[]", c_string_slice);
}
