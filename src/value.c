/*! @file
  @brief
  mruby/c value definitions

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "value.h"
#include "class.h"
#include "static.h"
#include "symbol.h"
#include "alloc.h"
#include "c_string.h"
#include "c_range.h"
#include "c_array.h"
#include "vm.h"


mrb_object *mrbc_obj_alloc(mrb_vm *vm, mrb_vtype tt)
{
  mrb_object *ptr = (mrb_object *)mrbc_alloc(vm, sizeof(mrb_object));
  if( ptr ){
    ptr->tt = tt;
  }
  return ptr;
}


mrb_proc *mrbc_rproc_alloc(mrb_vm *vm, const char *name)
{
  mrb_proc *ptr = (mrb_proc *)mrbc_alloc(vm, sizeof(mrb_proc));
  if( ptr ) {
    ptr->sym_id = add_sym(name);
#ifdef MRBC_DEBUG
    ptr->names = name;	// for debug; delete soon.
#endif
    ptr->next = 0;
  }
  return ptr;
}

mrb_proc *mrbc_rproc_alloc_to_class(mrb_vm *vm, const char *name, mrb_class *cls)
{
  mrb_proc *rproc = mrbc_rproc_alloc(vm, name);
  if( rproc != 0 ){
    rproc->next = cls->procs;
    cls->procs = rproc;
  }
  return rproc;
}


// EQ? two objects
// EQ: return true
// NEQ: return false
int mrbc_eq(const mrb_value *v1, const mrb_value *v2)
{
  // TT_XXX is different
  if( v1->tt != v2->tt ) return 0;

  // check value
  switch( v1->tt ){
  case MRB_TT_TRUE:
  case MRB_TT_FALSE:
  case MRB_TT_NIL:
    return 1;

  case MRB_TT_FIXNUM:
  case MRB_TT_SYMBOL:
    return v1->i == v2->i;

  case MRB_TT_FLOAT:
    return v1->d == v2->d;

  case MRB_TT_STRING:
    return mrbc_string_compare( v1, v2 );

  case MRB_TT_ARRAY:
    return mrbc_array_compare( v1, v2 );

  case MRB_TT_RANGE:
    return mrbc_range_compare( v1, v2 );

  default:
    return 0;
  }
}


//================================================================
/*!@brief
  Duplicate mrb_value

  @param   v     Pointer to mrb_value
*/
void mrbc_dup(mrb_value *v)
{
  switch( v->tt ){
  case MRB_TT_PROC:
  case MRB_TT_RANGE:
    mrbc_inc_ref_count(v->handle);
    break;

  case MRB_TT_STRING:
    mrbc_inc_ref_count(v->handle);
    v->h_str->ref_count++;	// no effect, yet.
    assert( v->h_str->ref_count < 255 );
    break;

  case MRB_TT_OBJECT:
    mrbc_inc_ref_count(v->instance);
    v->instance->ref_count++;	// no effect, yet.
    break;

  case MRB_TT_ARRAY:
    v->h_array->ref_count++;
    assert( v->h_array->ref_count < 255 );
    break;

  default:
    // Nothing
    break;
  }
}


//================================================================
/*!@brief
  Release object related memory

  @param   v     Pointer to target mrb_value
*/
void mrbc_release(mrb_value *v)
{
  mrbc_dec_ref_counter(v);
  v->tt = MRB_TT_EMPTY;
}


//================================================================
/*!@brief
  Decrement reference counter

  @param   v     Pointer to target mrb_value
*/
void mrbc_dec_ref_counter(mrb_value *v)
{
  switch( v->tt ) {
  case MRB_TT_PROC:
    if( mrbc_dec_ref_count(v->handle) == 0 ) {
      mrbc_raw_free(v->handle);
    }
    break;

#if MRBC_USE_STRING
  case MRB_TT_STRING:
    v->h_str->ref_count--;	// no effect, yet.
    if( mrbc_dec_ref_count(v->h_str) == 0 ) {
      mrbc_string_delete(v);
    }
    break;
#endif

  case MRB_TT_RANGE:
    if( mrbc_dec_ref_count(v->handle) == 0 ) {
      mrbc_range_delete(v);
    }
    break;

  case MRB_TT_OBJECT:
    v->instance->ref_count--;	// no effect, yet.
    if( mrbc_dec_ref_count(v->instance) == 0 ) {
      mrbc_instance_delete(v);
    }
    break;

  case MRB_TT_ARRAY:
    if( --v->h_array->ref_count == 0 ) {
      mrbc_array_delete(v);
    }
    break;

  default:
    // Nothing
    break;
  }
}


//================================================================
/*!@brief

  convert ASCII string to integer mruby/c version

  @param  s	source string.
  @param  base	n base.
  @return	result.
*/
int32_t mrbc_atoi( const char *s, int base )
{
  int ret = 0;
  int sign = 0;

 REDO:
  switch( *s ) {
  case '-':
    sign = 1;
    // fall through.
  case '+':
    s++;
    break;

  case ' ':
    s++;
    goto REDO;
  }

  int ch;
  while( (ch = *s++) != '\0' ) {
    int n;

    if( 'a' <= ch ) {
      n = ch - 'a' + 10;
    } else
    if( 'A' <= ch ) {
      n = ch - 'A' + 10;
    } else
    if( '0' <= ch && ch <= '9' ) {
      n = ch - '0';
    } else {
      break;
    }
    if( n >= base ) break;

    ret = ret * base + n;
  }

  if( sign ) ret = -ret;

  return ret;
}



//================================================================
/*!@brief

  mrb_instance constructor

  @param  vm    Pointer to VM.
  @param  cls	Pointer to Class (mrb_class).
  @param  size	size of additional data.
  @return       mrb_instance object.
*/
mrb_value mrbc_instance_new(struct VM *vm, mrb_class *cls, int size)
{
  mrb_value v = {.tt = MRB_TT_OBJECT};
  v.instance = (mrb_instance *)mrbc_alloc(vm, sizeof(mrb_instance) + size);
  if( v.instance == NULL ) return v;	// ENOMEM
  v.instance->cls = cls;

  return v;
}



//================================================================
/*!@brief

  mrb_instance destructor

  @param  v	pointer to target value
*/
void mrbc_instance_delete(mrb_value *v)
{
  mrbc_raw_free( v->instance );
}
