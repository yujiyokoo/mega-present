/*! @file
  @brief
  mruby/c Range object

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include "vm_config.h"
#include "value.h"
#include "alloc.h"
#include "static.h"
#include "class.h"
#include "c_range.h"
#include "c_string.h"
#include "console.h"
#include "opcode.h"


//================================================================
/*! constructor

  @param  vm		pointer to VM.
  @param  first		pointer to first value.
  @param  last		pointer to last value.
  @param  flag_exclude	true: exclude the end object, otherwise include.
  @return		range object.
*/
mrb_value mrbc_range_new( struct VM *vm, mrb_value *first, mrb_value *last, int flag_exclude)
{
  mrb_value value = {.tt = MRB_TT_RANGE};

  value.range = mrbc_alloc(vm, sizeof(mrb_range));
  if( !value.range ) return value;		// ENOMEM

  value.range->ref_count = 1;
  value.range->tt = MRB_TT_STRING;	// TODO: for DEBUG
  value.range->flag_exclude = flag_exclude;
  value.range->first = *first;
  value.range->last = *last;

  return value;
}


//================================================================
/*! destructor

  @param  target 	pointer to range object.
*/
void mrbc_range_delete(mrb_value *v)
{
  mrbc_release( &v->range->first );
  mrbc_release( &v->range->last );

  mrbc_raw_free( v->range );
}


//================================================================
/*! clear vm_id
*/
void mrbc_range_clear_vm_id(mrb_value *v)
{
  mrbc_set_vm_id( v->range, 0 );
  mrbc_clear_vm_id( &v->range->first );
  mrbc_clear_vm_id( &v->range->last );
}


//================================================================
/*! compare
*/
int mrbc_range_compare(const mrb_value *v1, const mrb_value *v2)
{
  int res;

  res = mrbc_compare( &v1->range->first, &v2->range->first );
  if( res != 0 ) return res;

  res = mrbc_compare( &v1->range->last, &v2->range->last );
  if( res != 0 ) return res;

  return (int)v2->range->flag_exclude - (int)v1->range->flag_exclude;
}



//================================================================
/*! (method) ===
*/
static void c_range_equal3(mrb_vm *vm, mrb_value v[], int argc)
{
  if( v[0].tt == MRB_TT_CLASS ) {
	mrb_value result = mrbc_send( vm, v, argc, &v[1], "kind_of?", 1, &v[0] );
	SET_RETURN( result );
	return;
  }

  int cmp_first = mrbc_compare( &v[0].range->first, &v[1] );
  int result = (cmp_first <= 0);
  if( !result ) goto DONE;

  int cmp_last  = mrbc_compare( &v[1], &v[0].range->last );
  result = (v->range->flag_exclude) ? (cmp_last < 0) : (cmp_last <= 0);

 DONE:
  SET_BOOL_RETURN( result );
}


//================================================================
/*! (method) first
*/
static void c_range_first(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_range_first(v);
  SET_RETURN(ret);
}


//================================================================
/*! (method) last
*/
static void c_range_last(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_range_last(v);
  SET_RETURN(ret);
}



//================================================================
/*! (method) exclude_end?
*/
static void c_range_exclude_end(mrb_vm *vm, mrb_value v[], int argc)
{
  int result = v->range->flag_exclude;
  SET_BOOL_RETURN( result );
}



#if MRBC_USE_STRING
//================================================================
/*! (method) inspect
*/
static void c_range_inspect(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value ret = mrbc_string_new(vm, NULL, 0);
  if( !ret.string ) goto RETURN_NIL;		// ENOMEM

  int i;
  for( i = 0; i < 2; i++ ) {
    if( i != 0 ) mrbc_string_append_cstr( &ret, ".." );
    mrb_value v1 = (i == 0) ? mrbc_range_first(v) : mrbc_range_last(v);
    mrb_value s1 = mrbc_send( vm, v, argc, &v1, "inspect", 0 );
    mrbc_string_append( &ret, &s1 );
    mrbc_string_delete( &s1 );
  }

  SET_RETURN(ret);
  return;

 RETURN_NIL:
  SET_NIL_RETURN();
}
#endif



//================================================================
/*! initialize
*/
void mrbc_init_class_range(mrb_vm *vm)
{
  mrbc_class_range = mrbc_define_class(vm, "Range", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_range, "===", c_range_equal3);
  mrbc_define_method(vm, mrbc_class_range, "first", c_range_first);
  mrbc_define_method(vm, mrbc_class_range, "last", c_range_last);
  mrbc_define_method(vm, mrbc_class_range, "exclude_end?", c_range_exclude_end);

#if MRBC_USE_STRING
  mrbc_define_method(vm, mrbc_class_range, "inspect", c_range_inspect);
  mrbc_define_method(vm, mrbc_class_range, "to_s", c_range_inspect);
#endif
}
