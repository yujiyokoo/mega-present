/*! @file
  @brief

  <pre>
  Copyright (C) 2015-2017 Kyushu Institute of Technology.
  Copyright (C) 2015-2017 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#include <string.h>

#include "value.h"
#include "class.h"
#include "alloc.h"
#include "static.h"
#include "console.h"

#include "c_array.h"
#include "c_hash.h"
#include "c_numeric.h"
#include "c_string.h"
#include "c_symbol.h"
#include "c_range.h"

#include "c_ext.h"


//================================================================
/*!@brief
  find class by object

  @param  vm
  @param  obj
  @return pointer to mrb_class
*/
mrb_class *find_class_by_object(struct VM *vm, mrb_object *obj)
{
  mrb_class *cls;

  switch( obj->tt ) {
  case MRB_TT_TRUE:	cls = mrbc_class_true;		break;
  case MRB_TT_FALSE:	cls = mrbc_class_false; 	break;
  case MRB_TT_NIL:	cls = mrbc_class_nil;		break;
  case MRB_TT_FIXNUM:	cls = mrbc_class_fixnum;	break;
  case MRB_TT_FLOAT:	cls = mrbc_class_float; 	break;
  case MRB_TT_SYMBOL:	cls = mrbc_class_symbol;	break;

  case MRB_TT_OBJECT:	cls = obj->instance->cls;       break;
  case MRB_TT_PROC:	cls = mrbc_class_proc;		break;
  case MRB_TT_ARRAY:	cls = mrbc_class_array; 	break;
  case MRB_TT_STRING:	cls = mrbc_class_string;	break;
  case MRB_TT_RANGE:	cls = mrbc_class_range; 	break;
  case MRB_TT_HASH:	cls = mrbc_class_hash;		break;

  case MRB_TT_USERTOP:	cls = vm->target_class; 	break;

  default:		cls = mrbc_class_object;	break;
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




//================================================================
// Object class

// Object - puts
static void c_puts(mrb_vm *vm, mrb_value *v)
{
  mrb_value *arg0 = v+1;
  switch( arg0->tt ){
  case MRB_TT_FIXNUM:
    console_printf("%d", arg0->i);
    break;
  case MRB_TT_NIL:
    console_printf("(nil)");
    break;
  case MRB_TT_TRUE:
    console_printf("true");
    break;
  case MRB_TT_FALSE:
    console_printf("false");
    break;
#if MRBC_USE_FLOAT
  case MRB_TT_FLOAT:
    console_printf("%f", arg0->d);
    break;
#endif
  case MRB_TT_STRING:
    console_printf("%s", arg0->handle->str);
    break;
  case MRB_TT_RANGE:{
    mrb_value *ptr = arg0->range;
    if( ptr[0].tt == MRB_TT_TRUE ){
      console_printf("%d...%d", ptr[1].i, ptr[2].i);
    } else {
      console_printf("%d..%d", ptr[1].i, ptr[2].i);
    }
  } break;
  case MRB_TT_ARRAY:{
    mrb_value *array = arg0->array->array;
    int i, n = array[0].i;
    console_printf("[");
    for( i=1 ; i<=n ; i++ ){
      c_puts(vm, &array[i-1]);
      if( i!=n ){
	console_printf(", ");
      }
    }
    console_printf("]");
  } break;
  default:
    console_printf("Not supported: MRB_TT_XX(%d)", arg0->tt);
    break;
  }
}

static void c_puts_nl(mrb_vm *vm, mrb_value *v)
{
  c_puts(vm, v);
  console_printf("\n");
}


static void c_object_not(mrb_vm *vm, mrb_value *v)
{
  SET_FALSE_RETURN();
}

// Object !=
static void c_object_neq(mrb_vm *vm, mrb_value *v)
{
  if( mrbc_eq(v, &GET_ARG(1)) ){
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}

// Object#class
static void c_object_class(mrb_vm *vm, mrb_value *v)
{
  // TODO: return class name
  char *name = "(class name)";
  char *str = (char *)mrbc_alloc(vm, sizeof(name)+1);
  strcpy(str, name);
  v->tt = MRB_TT_STRING;
  v->str = str;
}

// Object.new
static void c_object_new(mrb_vm *vm, mrb_value *v)
{
  mrb_instance *instance = (mrb_instance *)mrbc_alloc(vm, sizeof(mrb_instance));
  mrb_value ret;
  ret.tt = MRB_TT_OBJECT;
  ret.instance = instance;
  ret.instance->cls = mrbc_class_object;
  mrbc_release(vm, v);
  v[0] = ret;
}

static void mrbc_init_class_object(mrb_vm *vm)
{
  // Class
  mrbc_class_object = mrbc_class_alloc(vm, "Object", 0);
  // Methods
  mrbc_define_method(vm, mrbc_class_object, "puts", c_puts_nl);
  mrbc_define_method(vm, mrbc_class_object, "!", c_object_not);
  mrbc_define_method(vm, mrbc_class_object, "!=", c_object_neq);
  mrbc_define_method(vm, mrbc_class_object, "class", c_object_class);
  mrbc_define_method(vm, mrbc_class_object, "new", c_object_new);
}

// =============== ProcClass

void c_proc_call(mrb_vm *vm, mrb_value *v)
{
  // similar to OP_SEND

  // callinfo
  mrb_callinfo *callinfo = vm->callinfo + vm->callinfo_top;
  callinfo->reg_top = vm->reg_top;
  callinfo->pc_irep = vm->pc_irep;
  callinfo->pc = vm->pc;
  callinfo->n_args = 2;
  vm->callinfo_top++;



  // target irep
  vm->pc = 0;
  vm->pc_irep = v->proc->func.irep;

}


static void mrbc_init_class_proc(mrb_vm *vm)
{
  // Class
  mrbc_class_proc= mrbc_class_alloc(vm, "Proc", mrbc_class_object);
  // Methods
  mrbc_define_method(vm, mrbc_class_proc, "call", c_proc_call);
}


//================================================================
// Nil class

static void c_nil_false_not(mrb_vm *vm, mrb_value *v)
{
  SET_TRUE_RETURN();
}

static void mrbc_init_class_nil(mrb_vm *vm)
{
  // Class
  mrbc_class_nil = mrbc_class_alloc(vm, "NilClass", mrbc_class_object);
  // Methods
  mrbc_define_method(vm, mrbc_class_nil, "!", c_nil_false_not);
}



//================================================================
// False class

static void mrbc_init_class_false(mrb_vm *vm)
{
  // Class
  mrbc_class_false = mrbc_class_alloc(vm, "FalseClass", mrbc_class_object);
  // Methods
  mrbc_define_method(vm, mrbc_class_false, "!", c_nil_false_not);
}



//================================================================
// True class

static void mrbc_init_class_true(mrb_vm *vm)
{
  // Class
  mrbc_class_true = mrbc_class_alloc(vm, "TrueClass", mrbc_class_object);
  // Methods
}



//================================================================
// initialize

void mrbc_init_class(void)
{
  mrbc_init_class_object(0);
  mrbc_init_class_nil(0);
  mrbc_init_class_proc(0);
  mrbc_init_class_false(0);
  mrbc_init_class_true(0);

  mrbc_init_class_fixnum(0);
  mrbc_init_class_symbol(0);
#if MRBC_USE_FLOAT
  mrbc_init_class_float(0);
#endif
#if MRBC_USE_STRING
  mrbc_init_class_string(0);
#endif
  mrbc_init_class_array(0);
  mrbc_init_class_range(0);
  mrbc_init_class_hash(0);

  // etension
  mrbc_init_class_extension(0);
}
