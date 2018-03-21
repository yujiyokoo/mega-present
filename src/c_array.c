/*! @file
  @brief
  mruby/c Array class

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include <string.h>
#include <assert.h>
#include <stdio.h>   // for debug

#include "vm_config.h"
#include "alloc.h"
#include "static.h"
#include "class.h"
#include "value.h"
#include "console.h"
#include "c_array.h"


#ifndef ENOMEM
#define ENOMEM 12
#endif


//================================================================
/*! constructor

  @param  vm	pointer to VM.
  @param  size	initial size
  @return 	array object
*/
mrb_value mrbc_array_new(mrb_vm *vm, int size)
{
  mrb_value value = {.tt = MRB_TT_ARRAY};

  /*
    Allocate handle and data buffer.
  */
  MrbcHandleArray *h = mrbc_alloc(vm, sizeof(MrbcHandleArray));
  if( !h ) return value;	// ENOMEM

  mrb_value *data = mrbc_alloc(vm, sizeof(mrb_value) * size);
  if( !data ) {			// ENOMEM
    mrbc_raw_free( h );
    return value;
  }

  h->ref_count = 1;
  h->tt = MRB_TT_ARRAY;
  h->data_size = size;
  h->n_stored = 0;
  h->data = data;

  value.h_array = h;
  return value;
}


//================================================================
/*! destructor

  @param  ary	pointer to target value
*/
void mrbc_array_delete(mrb_value *ary)
{
  MrbcHandleArray *h = ary->h_array;

  mrb_value *p1 = h->data;
  const mrb_value *p2 = p1 + h->n_stored;
  while( p1 < p2 ) {
    mrbc_dec_ref_counter(p1++);
  }

  mrbc_raw_free(h->data);
  mrbc_raw_free(h);
}



//================================================================
/*! clear vm_id
*/
void mrbc_array_clear_vm_id(mrb_value *ary)
{
  MrbcHandleArray *h = ary->h_array;

  mrbc_set_vm_id( h, 0 );

  mrb_value *p1 = h->data;
  const mrb_value *p2 = p1 + h->n_stored;
  while( p1 < p2 ) {
    mrbc_clear_vm_id(p1++);
  }
}


//================================================================
/*! resize buffer
*/
int mrbc_array_resize(mrb_value *ary, int size)
{
  MrbcHandleArray *h = ary->h_array;

  mrb_value *data2 = mrbc_raw_realloc(h->data, sizeof(mrb_value) * size);
  if( !data2 ) return ENOMEM;	// ENOMEM

  h->data = data2;
  h->data_size = size;

  return 0;
}


//================================================================
/*! setter
*/
void mrbc_array_set(mrb_value *ary, int idx, mrb_value *set_val)
{
  MrbcHandleArray *h = ary->h_array;

  if( idx < 0 ) {
    idx = h->n_stored + idx;
    if( idx < 0 ) {
      console_print("IndexError\n");	// raise?
      return;
    }
  }

  // need resize?
  if( idx >= h->data_size ) {
    if( mrbc_array_resize(ary, idx + 1) != 0 ) return;	// ENOMEM
  }

  h->data[idx] = *set_val;

  // clear empty cells if need.
  if( idx >= h->n_stored ) {
    int i;
    for( i = h->n_stored; i < idx; i++ ) {
      h->data[i] = mrb_nil_value();
    }
    h->n_stored = idx + 1;
  }
}


//================================================================
/*! getter
*/
mrb_value mrbc_array_get(mrb_value *ary, int idx)
{
  MrbcHandleArray *h = ary->h_array;

  if( idx < 0 ) idx = h->n_stored + idx;
  if( 0 <= idx && idx < h->n_stored ) return h->data[idx];
  return mrb_nil_value();
}


//================================================================
/*! push a data to tail
*/
void mrbc_array_push(mrb_value *ary, mrb_value *set_val)
{
  MrbcHandleArray *h = ary->h_array;

  if( h->n_stored >= h->data_size ) {
    int size = h->data_size + 5;	// TODO 5 is better value?
    if( mrbc_array_resize(ary, size) != 0 ) return;	// ENOMEM
  }

  h->data[h->n_stored++] = *set_val;
}


//================================================================
/*! pop a data from tail.
*/
mrb_value mrbc_array_pop(mrb_value *ary)
{
  MrbcHandleArray *h = ary->h_array;

  if( h->n_stored <= 0 ) return mrb_nil_value();
  return h->data[--h->n_stored];
}


//================================================================
/*! insert a data to the first.
*/
void mrbc_array_unshift(mrb_value *ary, mrb_value *set_val)
{
  mrbc_array_insert(ary, 0, set_val);
}


//================================================================
/*! removes the first data and returns it.
*/
mrb_value mrbc_array_shift(mrb_value *ary)
{
  MrbcHandleArray *h = ary->h_array;

  if( h->n_stored <= 0 ) return mrb_nil_value();

  mrb_value ret = h->data[0];
  memmove(h->data, h->data+1, sizeof(mrb_value) * --h->n_stored);

  return ret;
}


//================================================================
/*! insert a data
*/
void mrbc_array_insert(mrb_value *ary, int idx, mrb_value *set_val)
{
  MrbcHandleArray *h = ary->h_array;

  if( idx < 0 ) {
    idx = h->n_stored + idx + 1;
    if( idx < 0 ) {
      console_print("IndexError\n");	// raise?
      return;
    }
  }

  // need resize?
  int size = 0;
  if( idx >= h->data_size ) {
    size = idx + 1;
  } else if( h->n_stored >= h->data_size ) {
    size = h->data_size + 1;
  }
  if( size ) {
    if( mrbc_array_resize(ary, size) != 0 ) return;	// ENOMEM
  }

  // move datas.
  if( idx < h->n_stored ) {
    memmove(h->data + idx + 1, h->data + idx,
	    sizeof(mrb_value) * (h->n_stored - idx));
  }

  // set data
  h->data[idx] = *set_val;
  h->n_stored++;

  // clear empty cells if need.
  if( idx >= h->n_stored ) {
    int i;
    for( i = h->n_stored-1; i < idx; i++ ) {
      h->data[i] = mrb_nil_value();
    }
    h->n_stored = idx + 1;
  }
}


//================================================================
/*! remove a data
*/
mrb_value mrbc_array_remove(mrb_value *ary, int idx)
{
  MrbcHandleArray *h = ary->h_array;

  if( idx < 0 ) {
    idx = h->n_stored + idx;
  }
  if( idx < 0 || idx >= h->n_stored ) return mrb_nil_value();

  mrb_value val = h->data[idx];

  h->n_stored--;

  if( idx < h->n_stored ) {
    memmove(h->data + idx, h->data + idx + 1,
	    sizeof(mrb_value) * (h->n_stored - idx));
  }

  return val;
}


//================================================================
/*! clear all
*/
void mrbc_array_clear(mrb_value *ary)
{
  MrbcHandleArray *h = ary->h_array;

  h->n_stored = 0;
}


//================================================================
/*! compare
*/
int mrbc_array_compare(const mrb_value *v1, const mrb_value *v2)
{
  if( v1->h_array->n_stored != v2->h_array->n_stored ) return 0;

  int i;
  for( i = 0; i < v1->h_array->n_stored; i++ ) {
    if( !mrbc_eq( &v1->h_array->data[i], &v2->h_array->data[i] ) ) return 0;
  }
  return 1;
}



//================================================================
/*! (operator) +
*/
static void c_array_add(mrb_vm *vm, mrb_value *v, int argc)
{
  if( GET_TT_ARG(1) != MRB_TT_ARRAY ) {
    console_print( "TypeError\n" );	// raise?
    return;
  }

  MrbcHandleArray *h1 = v[0].h_array;
  MrbcHandleArray *h2 = v[1].h_array;

  mrb_value value = mrbc_array_new(vm, h1->n_stored + h2->n_stored);
  if( value.h_array == NULL ) return;		// ENOMEM

  memcpy( value.h_array->data,                h1->data,
	  sizeof(mrb_value) * h1->n_stored );
  memcpy( value.h_array->data + h1->n_stored, h2->data,
	  sizeof(mrb_value) * h2->n_stored );
  value.h_array->n_stored = h1->n_stored + h2->n_stored;

  mrb_value *p1 = value.h_array->data;
  const mrb_value *p2 = p1 + value.h_array->n_stored;
  while( p1 < p2 ) {
    mrbc_dup(p1++);
  }

  if( --h1->ref_count == 0 ) mrbc_array_delete(v);
  if( --h2->ref_count == 0 ) mrbc_array_delete(v+1);

  v[1].tt = MRB_TT_EMPTY;
  SET_RETURN(value);
}


//================================================================
/*! (operator) []
*/
static void c_array_get(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *v1 = &GET_ARG(1);
  mrb_value *v2 = &GET_ARG(2);

  /*
    in case of self[nth] -> object | nil
  */
  if( argc == 1 && v1->tt == MRB_TT_FIXNUM ) {
    mrb_value val = mrbc_array_get(v, v1->i);
    mrbc_dup(&val);
    mrbc_release(v);
    SET_RETURN(val);
    return;
  }

  /*
    in case of self[start, length] -> Array | nil
  */
  if( argc == 2 && v1->tt == MRB_TT_FIXNUM && v2->tt == MRB_TT_FIXNUM ) {
    // TODO: not implement yet.
  }

  /*
    other case
  */
  console_print( "Not support such case in Array#[].\n" );
}


//================================================================
/*! (operator) []=
*/
static void c_array_set(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *v1 = &GET_ARG(1);
  mrb_value *v2 = &GET_ARG(2);

  /*
    in case of self[nth] = val
  */
  if( argc == 2 && v1->tt == MRB_TT_FIXNUM ) {
    mrb_value val = mrbc_array_get(v, v1->i);
    mrbc_release(&val);
    mrbc_array_set(v, v1->i, v2);
    v2->tt = MRB_TT_EMPTY;
    return;
  }

  /*
    in case of self[start, length] = val
  */
  if( argc == 3 && v1->tt == MRB_TT_FIXNUM && v2->tt == MRB_TT_FIXNUM ) {
    // TODO: not implement yet.
  }

  /*
    other case
  */
  console_print( "Not support such case in Array#[].\n" );
}


//================================================================
/*! (method) clear
*/
static void c_array_clear(mrb_vm *vm, mrb_value *v, int argc)
{
  int i;
  for( i = 0; i < v->h_array->n_stored; i++ ) {
    mrbc_release(&v->h_array->data[i]);
  }

  mrbc_array_clear(v);
}


//================================================================
/*! (method) delete_at
*/
static void c_array_delete_at(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value val = mrbc_array_remove(v, GET_INT_ARG(1));
  mrbc_release(v);
  SET_RETURN(val);
}


//================================================================
/*! (method) empty?
*/
static void c_array_empty(mrb_vm *vm, mrb_value *v, int argc)
{
  int n = v->h_array->n_stored;

  mrbc_release(v);
  if( n ) {
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}


//================================================================
/*! (method) size,length,count
*/
static void c_array_size(mrb_vm *vm, mrb_value *v, int argc)
{
  int n = v->h_array->n_stored;

  mrbc_release(v);
  SET_INT_RETURN(n);
}


//================================================================
/*! (method) index
*/
static void c_array_index(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *value = &GET_ARG(1);
  mrb_value *data = v->h_array->data;
  int n = v->h_array->n_stored;
  int i;

  for( i = 0; i < n; i++ ) {
    if( mrbc_eq(&data[i], value) ) break;
  }

  mrbc_release(v);
  if( i < n ) {
    SET_INT_RETURN(i);
  } else {
    SET_NIL_RETURN();
  }
}


//================================================================
/*! (method) first
*/
static void c_array_first(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value val = mrbc_array_get(v, 0);
  mrbc_release(v);
  mrbc_dup(&val);
  SET_RETURN(val);
}


//================================================================
/*! (method) last
*/
static void c_array_last(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value val = mrbc_array_get(v, -1);
  mrbc_release(v);
  mrbc_dup(&val);
  SET_RETURN(val);
}


//================================================================
/*! (method) push
*/
static void c_array_push(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value val = GET_ARG(1);
  mrbc_dup(&val);
  mrbc_array_push(v, &val);
}


//================================================================
/*! (method) pop
*/
static void c_array_pop(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *v1 = &GET_ARG(1);

  /*
    in case of pop() -> object | nil
  */
  if( argc == 0 ) {
    mrb_value val = mrbc_array_pop(v);
    mrbc_release(v);
    SET_RETURN(val);
    return;
  }

  /*
    in case of pop(n) -> Array
  */
  if( argc == 1 && v1->tt == MRB_TT_FIXNUM ) {
    // TODO: not implement yet.
  }

  /*
    other case
  */
  console_print( "Not support such case in Array#pop.\n" );
}


//================================================================
/*! (method) unshift
*/
static void c_array_unshift(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value val = GET_ARG(1);
  mrbc_dup(&val);
  mrbc_array_unshift(v, &val);
}


//================================================================
/*! (method) shift
*/
static void c_array_shift(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value *v1 = &GET_ARG(1);

  /*
    in case of pop() -> object | nil
  */
  if( argc == 0 ) {
    mrb_value val = mrbc_array_shift(v);
    mrbc_release(v);
    SET_RETURN(val);
    return;
  }

  /*
    in case of pop(n) -> Array
  */
  if( argc == 1 && v1->tt == MRB_TT_FIXNUM ) {
    // TODO: not implement yet.
  }

  /*
    other case
  */
  console_print( "Not support such case in Array#shift.\n" );
}


//================================================================
/*! (method) dup
*/
static void c_array_dup(mrb_vm *vm, mrb_value *v, int argc)
{
  MrbcHandleArray *h = GET_ARG(0).h_array;

  mrb_value value = mrbc_array_new(vm, h->n_stored);
  if( value.h_array == NULL ) return;		// ENOMEM

  memcpy( value.h_array->data, h->data, sizeof(mrb_value) * h->n_stored );
  value.h_array->n_stored = h->n_stored;

  mrb_value *p1 = value.h_array->data;
  const mrb_value *p2 = p1 + value.h_array->n_stored;
  while( p1 < p2 ) {
    mrbc_dup(p1++);
  }

  mrbc_release(v);
  SET_RETURN(value);
}



void mrbc_init_class_array(mrb_vm *vm)
{
  mrbc_class_array = mrbc_define_class(vm, "Array", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_array, "+", c_array_add);
  mrbc_define_method(vm, mrbc_class_array, "[]", c_array_get);
  mrbc_define_method(vm, mrbc_class_array, "at", c_array_get);
  mrbc_define_method(vm, mrbc_class_array, "[]=", c_array_set);
  mrbc_define_method(vm, mrbc_class_array, "<<", c_array_push);
  mrbc_define_method(vm, mrbc_class_array, "clear", c_array_clear);
  mrbc_define_method(vm, mrbc_class_array, "delete_at", c_array_delete_at);
  mrbc_define_method(vm, mrbc_class_array, "empty?", c_array_empty);
  mrbc_define_method(vm, mrbc_class_array, "size", c_array_size);
  mrbc_define_method(vm, mrbc_class_array, "length", c_array_size);
  mrbc_define_method(vm, mrbc_class_array, "count", c_array_size);
  mrbc_define_method(vm, mrbc_class_array, "index", c_array_index);
  mrbc_define_method(vm, mrbc_class_array, "first", c_array_first);
  mrbc_define_method(vm, mrbc_class_array, "last", c_array_last);
  mrbc_define_method(vm, mrbc_class_array, "push", c_array_push);
  mrbc_define_method(vm, mrbc_class_array, "pop", c_array_pop);
  mrbc_define_method(vm, mrbc_class_array, "shift", c_array_shift);
  mrbc_define_method(vm, mrbc_class_array, "unshift", c_array_unshift);
  mrbc_define_method(vm, mrbc_class_array, "dup", c_array_dup);
}
