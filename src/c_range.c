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
#include "alloc.h"
#include "value.h"
#include "vm.h"
#include "class.h"
#include "static.h"
#include "console.h"
#include "c_range.h"


//================================================================
/*! constructor

  @param  vm		pointer to VM.
  @param  v_first	pointer to first value.
  @param  v_last	pointer to last value.
  @param  flag_exclude	true: exclude the end object, otherwise include.
  @return		range object.
*/
mrb_value mrbc_range_new(mrb_vm *vm, mrb_value *v_first, mrb_value *v_last, int flag_exclude)
{
  mrb_value value = {.tt = MRB_TT_RANGE};

  value.h_range = mrbc_alloc(vm, sizeof(MrbcHandleRange));
  if( !value.h_range ) return value;		// ENOMEM

  value.h_range->ref_count = 1;		// TODO: not use yet.
  value.h_range->tt = MRB_TT_STRING;	// TODO: for DEBUG
  value.h_range->flag_exclude = flag_exclude;
  value.h_range->first = *v_first;
  value.h_range->last = *v_last;

  return value;
}


//================================================================
/*! destructor

  @param  target 	pointer to range object.
*/
void mrbc_range_delete(mrb_value *v)
{
  mrbc_release( &v->h_range->first );
  mrbc_release( &v->h_range->last );

  mrbc_raw_free( v->h_range );
}


//================================================================
/*! clear vm_id
*/
void mrbc_range_clear_vm_id(mrb_value *v)
{
  // TODO: set first and last member's vm_id to zero.

  mrbc_set_vm_id( v->h_range, 0 );
}


//================================================================
/*! (method) ===
*/
static void c_range_equal3(mrb_vm *vm, mrb_value *v, int argc)
{
  int result = 0;

  mrb_value *v_first = &v->h_range->first;
  mrb_value *v_last =&v->h_range->last;
  mrb_value *v1 = v+1;

  if( v_first->tt == MRB_TT_FIXNUM && v1->tt == MRB_TT_FIXNUM ) {
    if( v->h_range->flag_exclude ) {
      result = (v_first->i <= v1->i) && (v1->i < v_last->i);
    } else {
      result = (v_first->i <= v1->i) && (v1->i <= v_last->i);
    }
    goto DONE;
  }
  console_printf( "Not supported\n" );
  return;

 DONE:
  mrbc_release(v);
  if( result ) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}



//================================================================
/*! initialize
*/
void mrbc_init_class_range(mrb_vm *vm)
{
  mrbc_class_range = mrbc_class_alloc(vm, "Range", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_range, "===", c_range_equal3);
}
