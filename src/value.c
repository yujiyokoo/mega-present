/*! @file
  @brief
  mruby/c value definitions

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#include "vm_config.h"
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
    ptr->ref_count = 1;
    ptr->sym_id = add_sym(name);
#ifdef MRBC_DEBUG
    ptr->names = name;	// for debug; delete soon.
#endif
    ptr->next = 0;
  }
  return ptr;
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
  case MRB_TT_OBJECT:
  case MRB_TT_PROC:
  case MRB_TT_ARRAY:
  case MRB_TT_STRING:
  case MRB_TT_RANGE:
    assert( v->instance->ref_count > 0 );
    assert( v->instance->ref_count != 0xff );	// check max value.
    v->instance->ref_count++;
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
  switch( v->tt ){
  case MRB_TT_OBJECT:
  case MRB_TT_PROC:
  case MRB_TT_ARRAY:
  case MRB_TT_STRING:
  case MRB_TT_RANGE:
    assert( v->instance->ref_count != 0 );
    v->instance->ref_count--;
    break;

  default:
    // Nothing
    return;
  }

  // release memory?
  if( v->instance->ref_count != 0 ) return;

  switch( v->tt ) {
  case MRB_TT_OBJECT:	mrbc_instance_delete(v);	break;
  case MRB_TT_PROC:	mrbc_raw_free(v->handle);	break;
  case MRB_TT_ARRAY:	mrbc_array_delete(v);		break;
#if MRBC_USE_STRING
  case MRB_TT_STRING:	mrbc_string_delete(v);		break;
#endif
  case MRB_TT_RANGE:	mrbc_range_delete(v);		break;

  default:
    // Nothing
    break;
  }
}


//================================================================
/*!@brief
  clear vm id

  @param   v     Pointer to target mrb_value
*/
void mrbc_clear_vm_id(mrb_value *v)
{
  switch( v->tt ) {
  case MRB_TT_ARRAY:	mrbc_array_clear_vm_id(v);	break;
  case MRB_TT_STRING:	mrbc_string_clear_vm_id(v);	break;
  case MRB_TT_RANGE:	mrbc_range_clear_vm_id(v);	break;

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
  mrb_irep allocator

  @param  vm	Pointer of VM.
  @return	Pointer of allocated mrb_irep
*/
mrb_irep *mrbc_irep_alloc(mrb_vm *vm)
{
  mrb_irep *p = (mrb_irep *)mrbc_alloc(vm, sizeof(mrb_irep));
  if( p )
    memset(p, 0, sizeof(mrb_irep));	// caution: assume NULL is zero.
  return p;
}


//================================================================
/*!@brief
  release mrb_irep holds memory
*/
void mrbc_irep_free(mrb_irep *irep)
{
  int i;

  // release pools.
  for( i = 0; i < irep->plen; i++ ) {
    mrbc_raw_free( irep->pools[i] );
  }
  if( irep->plen ) mrbc_raw_free( irep->pools );

  // release child ireps.
  for( i = 0; i < irep->rlen; i++ ) {
    mrbc_irep_free( irep->reps[i] );
  }
  if( irep->rlen ) mrbc_raw_free( irep->reps );

  mrbc_raw_free( irep );
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

  v.instance->ref_count = 1;
  v.instance->tt = MRB_TT_OBJECT;	// for debug only.
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
