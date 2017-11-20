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
  @param  len	source length
  @return 	string object
*/
mrb_value mrbc_string_new(mrb_vm *vm, const char *src, int len)
{
  mrb_value value = {.tt = MRB_TT_STRING};

  /*
    Allocate handle and string buffer.
    Handle have a reference count with value 1.
  */
  value.handle = (mrb_value *)mrbc_alloc(vm, sizeof(mrb_value));
  if( !value.handle ) return value;		// ENOMEM

  char *buf = (char *)mrbc_alloc(vm, len+1);
  if( !buf ) {
    mrbc_raw_free(value.handle);
    value.handle = NULL;
    return value;	// ENOMEM
  }

  value.handle->tt = MRB_TT_STRING;
  value.handle->str = buf;

  /*
    Copy a source string.
  */
  if( src == NULL ) {
    buf[0] = '\0';
  } else {
    memcpy( buf, src, len );
    buf[len] = '\0';
  }

  return value;
}


//================================================================
/*! constructor by c string

  @param  vm	pointer to VM.
  @param  src	source string or NULL
  @return 	string object
*/
mrb_value mrbc_string_new_cstr(mrb_vm *vm, const char *src)
{
  return mrbc_string_new(vm, src, (src ? strlen(src) : 0));
}


//================================================================
/*! destructor

  @param  vm	pointer to VM.
  @param  v	pointer to target value
*/
void mrbc_string_delete(mrb_vm *vm, mrb_value *v)
{
  mrbc_raw_free(v->handle->str);
  mrbc_raw_free(v->handle);
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

  int len1 = strlen(MRBC_STRING_CSTR(s1));
  int len2 = strlen(MRBC_STRING_CSTR(s2));

  mrb_value value = mrbc_string_new(vm, NULL, len1+len2);
  if( value.handle == NULL ) return;		// ENOMEM

  memcpy( value.handle->str, MRBC_STRING_CSTR(s1), len1 );
  memcpy( value.handle->str+len1, MRBC_STRING_CSTR(s2), len2+1 );

  mrbc_release(vm, v);
  SET_RETURN(value);
}



//================================================================
/*! (method) ===
*/
static void c_string_eql(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *s1 = &GET_ARG(0);
  mrb_value *s2 = &GET_ARG(1);
  int result = 0;

  if( s2->tt != MRB_TT_STRING ) {
    goto DONE;
  }

  int len1 = strlen(MRBC_STRING_CSTR(s1));
  int len2 = strlen(MRBC_STRING_CSTR(s2));
  if( len1 != len2 ) {    // false
    goto DONE;
  }

  result = (memcmp( MRBC_STRING_CSTR(s1), MRBC_STRING_CSTR(s2), len1 ) == 0);

 DONE:
  mrbc_release(vm, v);
  if( result ) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}



//================================================================
/*! (method) size, length
*/
static void c_string_size(mrb_vm *vm, mrb_value *v, int argc)
{
  int i = strlen(MRBC_STRING_CSTR(v));

  mrbc_release(vm, v);
  SET_INT_RETURN( i );
}



//================================================================
/*! (method) to_i
  TODO: to_i(base = 10) only. need 2 to 36.
*/
static void c_string_to_i(mrb_vm *vm, mrb_value *v, int argc)
{
  int i = atoi(MRBC_STRING_CSTR(v));

  mrbc_release(vm, v);
  SET_INT_RETURN( i );
}


#if MRBC_USE_FLOAT
//================================================================
/*! (method) to_f
*/
static void c_string_to_f(mrb_vm *vm, mrb_value *v, int argc)
{
  double d = atof(MRBC_STRING_CSTR(v));

  mrbc_release(vm, v);
  SET_FLOAT_RETURN( d );
}
#endif


//================================================================
/*! (method) to_s
*/
static void c_string_to_s(mrb_vm *vm, mrb_value *v, int argc)
{
}


//================================================================
/*! (method) <<
*/
static void c_string_append(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *v2 = &GET_ARG(1);
  int len1 = strlen(MRBC_STRING_CSTR(v));
  int len2 = (v2->tt == MRB_TT_STRING) ? strlen(MRBC_STRING_CSTR(v2)) : 1;

  uint8_t *str = mrbc_realloc(vm, MRBC_STRING_CSTR(v), len1+len2+1);
  if( !str ) return;

  if( v2->tt == MRB_TT_STRING ) {
    memcpy(str + len1, MRBC_STRING_CSTR(v2), len2 + 1);
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
    int len = strlen(MRBC_STRING_CSTR(v));
    int idx = v1->i;
    int ch = 0;
    if( idx >= 0 ) {
      if( idx < len ) {
        ch = *((uint8_t *)MRBC_STRING_CSTR(v) + idx);
      }
    } else {
      idx += len;
      if( idx >= 0 ) {
        ch = *((uint8_t *)MRBC_STRING_CSTR(v) + idx);
      }
    }

    if( ch > 0 ) {
      mrb_value value = mrbc_string_new(vm, NULL, 1);
      if( !value.handle ) return;	// ENOMEM
      value.handle->str[0] = ch;
      value.handle->str[1] = '\0';
      mrbc_release(vm, v);
      SET_RETURN(value);
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
    int len = strlen(MRBC_STRING_CSTR(v));
    int idx = v1->i;
    if( idx < 0 ) idx += len;

    if( idx >= 0 ) {
      int rlen = (v2->i < (len - idx)) ? v2->i : (len - idx);
                                              // min( v2->i, (len-idx) )
      if( rlen >= 0 ) {
        mrb_value value = mrbc_string_new(vm, MRBC_STRING_CSTR(v) + idx, rlen);
	if( !value.handle ) return;	// ENOMEM

        mrbc_release(vm, v);
	SET_RETURN(value);
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
/*! (method) []=
*/
static void c_string_insert(mrb_vm *vm, mrb_value *v, int argc)
{
  int nth;
  int len;
  mrb_value *val;

  /*
    in case of self[nth] = val
  */
  if( argc == 2 &&
        GET_ARG(1).tt == MRB_TT_FIXNUM && GET_ARG(2).tt == MRB_TT_STRING ) {
    nth = GET_ARG(1).i;
    len = 1;
    val = &GET_ARG(2);
  }
  /*
    in case of self[nth, len] = val
  */
  else if( argc == 3 &&
        GET_ARG(1).tt == MRB_TT_FIXNUM && GET_ARG(2).tt == MRB_TT_FIXNUM &&
        GET_ARG(3).tt == MRB_TT_STRING ) {
    nth = GET_ARG(1).i;
    len = GET_ARG(2).i;
    val = &GET_ARG(3);
  }
  /*
    other cases
  */
  else {
    console_print( "Not support\n" );
    return;
  }

  int len1 = strlen(MRBC_STRING_CSTR(v));
  int len2 = strlen(MRBC_STRING_CSTR(val));
  if( nth < 0 ) nth = len1 + nth;               // adjust to positive number.
  if( len > len1 - nth ) len = len1 - nth;
  if( nth < 0 || nth > len1 || len < 0) {
    console_print( "IndexError\n" );  // raise?
    return;
  }

  uint8_t *str = mrbc_realloc(vm, MRBC_STRING_CSTR(v), len1 + len2 - len + 1);
  if( !str ) return;

  memmove( str + nth + len2, str + nth + len, len1 - nth - len );
  memcpy( str + nth, MRBC_STRING_CSTR(val), len2 );
  str[len1 + len2 - len] = '\0';

  v->handle->str = (char *)str;
}


//================================================================
/*! (method) ord
*/
static void c_string_ord(mrb_vm *vm, mrb_value *v, int argc)
{
  int i = MRBC_STRING_CSTR(v)[0];

  mrbc_release(vm, v);
  SET_INT_RETURN( i );
}


//================================================================
/*! initialize
*/
void mrbc_init_class_string(mrb_vm *vm)
{
  mrbc_class_string = mrbc_class_alloc(vm, "String", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_string, "+",	c_string_add);
  mrbc_define_method(vm, mrbc_class_string, "===",	c_string_eql);
  mrbc_define_method(vm, mrbc_class_string, "size",	c_string_size);
  mrbc_define_method(vm, mrbc_class_string, "length",	c_string_size);
  mrbc_define_method(vm, mrbc_class_string, "to_i",	c_string_to_i);
  mrbc_define_method(vm, mrbc_class_string, "to_s",	c_string_to_s);
  mrbc_define_method(vm, mrbc_class_string, "<<",	c_string_append);
  mrbc_define_method(vm, mrbc_class_string, "[]",	c_string_slice);
  mrbc_define_method(vm, mrbc_class_string, "[]=",	c_string_insert);
  mrbc_define_method(vm, mrbc_class_string, "ord",	c_string_ord);
#if MRBC_USE_FLOAT
  mrbc_define_method(vm, mrbc_class_string, "to_f",	c_string_to_f);
#endif
}
