/*! @file
  @brief

  <pre>
  Copyright (C) 2015-2016 Kyushu Institute of Technology.
  Copyright (C) 2015-2016 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#include "value.h"
#include "class.h"
#include "static.h"
#include "console.h"

#include "c_array.h"
#include "c_hash.h"
#include "c_numeric.h"
#include "c_string.h"
#include "c_range.h"



//================================================================
/*!@brief
  find class from a object

  @param  vm
  @param  obj
  @return pointer to mrb_class
*/
mrb_class *find_class_by_object(mrb_vm *vm, mrb_object *obj)
{
  mrb_class *cls = mrbc_class_object;
  switch( obj->tt ){
    case MRB_TT_ARRAY:
      cls = mrbc_class_array;
      break;
    case MRB_TT_HASH:
      cls = mrbc_class_hash;
      break;
    case MRB_TT_FIXNUM:
      cls = mrbc_class_fixnum;
      break;
    case MRB_TT_FALSE:
      cls = mrbc_class_false;
      break;
    case MRB_TT_TRUE:
      cls = mrbc_class_true;
      break;
#if MRBC_USE_FLOAT
    case MRB_TT_FLOAT:
      cls = mrbc_class_float;
      break;
#endif
#if MRBC_USE_STRING
    case MRB_TT_STRING:
      cls = mrbc_class_string;
      break;
#endif
    default:
      break;
  }
  return cls;
}



//================================================================
/*!@brief
  find method from

  @param  vm
  @param  recv
  @param  sym_id
  @return
*/
mrb_proc *find_method(mrb_vm *vm, mrb_value recv, mrb_sym sym_id)
{
  mrb_class *cls = find_class_by_object(vm, &recv);

  while( cls != 0 ) {
    mrb_proc *proc = cls->procs;
    while( proc != 0 ) {
      if( proc->sym_id == sym_id ) {
        return proc;
      }
      proc = proc->next;
    }
    cls = cls->super;
  }
  return 0;
}



void mrbc_define_method(mrb_vm *vm, mrb_class *cls, const char *name, mrb_func_t cfunc)
{
  mrb_proc *rproc = mrbc_rproc_alloc(vm, name);
  rproc->c_func = 1;  // c-func
  rproc->next = cls->procs;
  cls->procs = rproc;
  rproc->func.func = cfunc;
}




void mrbc_define_method_proc(mrb_vm *vm, mrb_class *cls, mrb_sym sym_id, mrb_proc *rproc)
{
  rproc->c_func = 0;
  rproc->sym_id = sym_id;
  rproc->next = cls->procs;
  cls->procs = rproc;
}



// Object - puts
void c_puts(mrb_vm *vm, mrb_value *v)
{
  mrb_value *arg0 = v+1;
  switch( arg0->tt ){
  case MRB_TT_FIXNUM:
    console_printf("%d", arg0->value.i);
    break;
  case MRB_TT_NIL:
    console_printf("");
    break;
  case MRB_TT_TRUE:
    console_printf("true");
    break;
  case MRB_TT_FALSE:
    console_printf("false");
    break;
#if MRBC_USE_FLOAT
  case MRB_TT_FLOAT:
    console_printf("%f", arg0->value.d);
    break;
#endif
#if MRBC_USE_STRING
  case MRB_TT_STRING:
    console_printf("%s", arg0->value.str);
    break;
#endif
  case MRB_TT_RANGE:{
    mrb_value *ptr = arg0->value.range;
    if( ptr[0].tt == MRB_TT_TRUE ){
      console_printf("%d...%d", ptr[1].value.i, ptr[2].value.i);
    } else {
      console_printf("%d..%d", ptr[1].value.i, ptr[2].value.i);
    }
  } break;
  default:
    console_printf("Not supported: MRB_TT_XX(%d)", arg0->tt);
    break;
  }
  console_printf("\n");
}


static void mrbc_init_class_object(mrb_vm *vm)
{
  // Class
  mrbc_class_object = mrbc_class_alloc(vm, "Object", 0);
  // Methods
  mrbc_define_method(vm, mrbc_class_object, "puts", c_puts);

}


// =============== FalseClass

void c_false_ne(mrb_vm *vm, mrb_value *v)
{
  mrb_object *arg0 = v+1;
  if( arg0->tt == MRB_TT_FALSE ){
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}

void c_false_not(mrb_vm *vm, mrb_value *v)
{
  SET_TRUE_RETURN();
}

static void mrbc_init_class_false(mrb_vm *vm)
{
  // Class
  mrbc_class_false = mrbc_class_alloc(vm, "FalseClass", mrbc_class_object);
  // Methods
  mrbc_define_method(vm, mrbc_class_false, "!=", c_false_ne);
  mrbc_define_method(vm, mrbc_class_false, "!", c_false_not);
}


// =============== TrueClass

void c_true_ne(mrb_vm *vm, mrb_value *v)
{
  mrb_object *arg0 = v+1;
  if( arg0->tt == MRB_TT_TRUE ){
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}

void c_true_not(mrb_vm *vm, mrb_value *v)
{
  SET_FALSE_RETURN();
}



static void mrbc_init_class_true(mrb_vm *vm)
{
  // Class
  mrbc_class_true = mrbc_class_alloc(vm, "TrueClass", mrbc_class_object);
  // Methods
  mrbc_define_method(vm, mrbc_class_true, "!=", c_true_ne);
  mrbc_define_method(vm, mrbc_class_true, "!", c_true_not);

}


void mrbc_init_class(void)
{
  mrbc_init_class_object(0);
  mrbc_init_class_false(0);
  mrbc_init_class_true(0);

  mrbc_init_class_fixnum(0);
#if MRBC_USE_FLOAT
  mrbc_init_class_float(0);
#endif
#if MRBC_USE_STRING
  mrbc_init_class_string(0);
#endif
  mrb_init_class_array(0);
  mrb_init_class_range(0);
  mrb_init_class_hash(0);
}
