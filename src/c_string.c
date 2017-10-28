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
#include "console.h"



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
/*! (method) +
*/
static void c_string_add(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *s1 = &GET_ARG(0);
  mrb_value *s2 = &GET_ARG(1);

  if( s2->tt != MRB_TT_STRING ) {
    console_print( "Not support STRING + Other\n" );
    return;
  }

  int len1 = strlen(MRBC_STRING_C_STR(s1));
  int len2 = strlen(MRBC_STRING_C_STR(s2));

  mrb_value *handle = mrbc_string_constructor_w_len(vm, NULL, len1+len2);
  if( !handle ) return;

  memcpy( handle->str, MRBC_STRING_C_STR(s1), len1 );
  memcpy( handle->str+len1, MRBC_STRING_C_STR(s2), len2+1 );

  mrbc_release(vm, v);
  v->tt = MRB_TT_STRING;
  v->handle = handle;
}



//================================================================
/*! (method) size, length
*/
static void c_string_size(mrb_vm *vm, mrb_value *v, int argc)
{
  int i = strlen(MRBC_STRING_C_STR(v));

  mrbc_release(vm, v);
  SET_INT_RETURN( i );
}



//================================================================
/*! (method) !=
*/
static void c_string_neq(mrb_vm *vm, mrb_value *v, int argc)
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
static void c_string_to_i(mrb_vm *vm, mrb_value *v, int argc)
{
  int i = atoi(MRBC_STRING_C_STR(v));

  mrbc_release(vm, v);
  SET_INT_RETURN( i );
}



//================================================================
/*! (method) <<
*/
static void c_string_append(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *v2 = &GET_ARG(1);
  int len1 = strlen(MRBC_STRING_C_STR(v));
  int len2 = (v2->tt == MRB_TT_STRING) ? strlen(MRBC_STRING_C_STR(v2)) : 1;

  uint8_t *str = mrbc_realloc(vm, MRBC_STRING_C_STR(v), len1+len2+1);
  if( !str ) return;

  if( v2->tt == MRB_TT_STRING ) {
    memcpy(str + len1, MRBC_STRING_C_STR(v2), len2 + 1);
  } else if( v2->tt == MRB_TT_FIXNUM ) {
    str[len1] = v2->i;
    str[len1+1] = '\0';
  }

  v->handle->str = (char *)str;
}



//================================================================
/*! (method) []
*/
static void c_string_slice(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *v1 = &GET_ARG(1);
  mrb_value *v2 = &GET_ARG(2);

  /*
    in case of slice(nth) -> String | nil
  */
  if( argc == 1 && v1->tt == MRB_TT_FIXNUM ) {
    int len = strlen(MRBC_STRING_C_STR(v));
    int idx = v1->i;
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
      v->handle = s;
    } else {
      mrbc_release(vm, v);
      SET_NIL_RETURN();
    }
    return;
  }

  /*
    in case of slice(nth, len) -> String | nil
  */
  if( argc == 2 && v1->tt == MRB_TT_FIXNUM && v2->tt == MRB_TT_FIXNUM ) {
    int len = strlen(MRBC_STRING_C_STR(v));
    int idx = v1->i;
    if( idx < 0 ) idx += len;

    if( idx >= 0 ) {
      int rlen = (v2->i < (len - idx)) ? v2->i : (len - idx);
                                              // min( v2->i, (len-idx) )
      if( rlen >= 0 ) {
        mrb_value *s = mrbc_string_constructor_w_len(vm,
                         MRBC_STRING_C_STR(v) + idx, rlen);
        if( !s ) return;

        mrbc_release(vm, v);
        v->tt = MRB_TT_STRING;
        v->handle = s;
        return;
      }
    }
    mrbc_release(vm, v);
    SET_NIL_RETURN();
    return;
  }

  console_print( "Not support such case in String#[].\n" );
}


//================================================================
/*! initialize
*/
void mrbc_init_class_string(mrb_vm *vm)
{
  mrbc_class_string = mrbc_class_alloc(vm, "String", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_string, "+", c_string_add);
  mrbc_define_method(vm, mrbc_class_string, "size", c_string_size);
  mrbc_define_method(vm, mrbc_class_string, "length", c_string_size);
  mrbc_define_method(vm, mrbc_class_string, "!=", c_string_neq);
  mrbc_define_method(vm, mrbc_class_string, "to_i", c_string_to_i);
  mrbc_define_method(vm, mrbc_class_string, "<<", c_string_append);
  mrbc_define_method(vm, mrbc_class_string, "[]", c_string_slice);
}
