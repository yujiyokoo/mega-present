/*! @file
  @brief
  mruby/c Range object

  <pre>
  Copyright (C) 2015-2017 Kyushu Institute of Technology.
  Copyright (C) 2015-2017 Shimane IT Open-Innovation Center.

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
  @param  v_st		pointer to start value.
  @param  v_ed		pointer to end value.
  @param  exclude	true: exclude the end object, otherwise include.
  @return		range object.
*/
mrb_value mrbc_range_new(mrb_vm *vm, mrb_value *v_st, mrb_value *v_ed, int exclude)
{
  mrb_value value;

  value.tt = MRB_TT_RANGE;
  value.range = (mrb_value*)mrbc_alloc(vm, sizeof(mrb_value) * 3);
  if( !value.range ) return value;	// ENOMEM

  value.range[0].tt = exclude ? MRB_TT_TRUE : MRB_TT_FALSE;
  value.range[1] = *v_st;
  value.range[2] = *v_ed;

  return value;
}


//================================================================
/*! destructor

  @param  target 	pointer to range object.
*/
void mrbc_range_delete(mrb_vm *vm, mrb_value *v)
{
  mrb_value *obj = v->range;

  mrbc_release( vm, &obj[1] );
  mrbc_release( vm, &obj[2] );
  mrbc_raw_free(obj);
}


//================================================================
/*! clear vm_id
*/
void mrbc_range_clear_vm_id(mrb_value *v)
{
  mrbc_set_vm_id( v->range, 0 );
}


//================================================================
/*! (method) ===
*/
static void c_range_equal3(mrb_vm *vm, mrb_value *v, int argc)
{

  int result = 0;
  mrb_value *flag = &v->range[0];
  mrb_value *v_st = &v->range[1];
  mrb_value *v_ed = &v->range[2];
  mrb_value *v1 = v+1;

  if( v_st->tt == MRB_TT_FIXNUM && v1->tt == MRB_TT_FIXNUM ) {

    if( flag->tt == MRB_TT_TRUE ) {
      result = (v_st->i <= v1->i) && (v1->i < v_ed->i);
    } else {
      result = (v_st->i <= v1->i) && (v1->i <= v_ed->i);
    }
    goto DONE;
  }
  console_printf( "Not supported\n" );
  return;

 DONE:
  mrbc_release(vm, v);
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
