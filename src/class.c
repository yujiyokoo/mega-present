/*! @file
  @brief

  <pre>
  Copyright (C) 2015-2017 Kyushu Institute of Technology.
  Copyright (C) 2015-2017 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#include "vm_config.h"
#include <string.h>
#include <assert.h>
#include "value.h"
#include "class.h"
#include "alloc.h"
#include "static.h"
#include "symbol.h"
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
  case MRB_TT_CLASS:    cls = obj->cls;                 break;
  case MRB_TT_PROC:	cls = mrbc_class_proc;		break;
  case MRB_TT_ARRAY:	cls = mrbc_class_array; 	break;
  case MRB_TT_STRING:	cls = mrbc_class_string;	break;
  case MRB_TT_RANGE:	cls = mrbc_class_range; 	break;
  case MRB_TT_HASH:	cls = mrbc_class_hash;		break;

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



//================================================================
/*!@brief
  define class

  @param  vm		pointer to vm.
  @param  name		class name.
  @param  super		super class.
*/
mrb_class * mrbc_define_class(mrb_vm *vm, const char *name, mrb_class *super)
{
  mrb_class *cls;
  mrb_sym sym_id = add_sym(name);
  mrb_object obj = const_object_get(sym_id);

  // create a new class?
  if( obj.tt == MRB_TT_NIL ) {
    cls = mrbc_alloc( 0, sizeof(mrb_class) );
    if( !cls ) return cls;	// ENOMEM

    cls->sym_id = sym_id;
#ifdef MRBC_DEBUG
    cls->names = name;	// for debug; delete soon.
#endif
    cls->super = super;
    cls->procs = 0;

    // register to global constant.
    mrb_value v = {.tt = MRB_TT_CLASS};
    v.cls = cls;
    const_object_add(sym_id, &v);

    return cls;
  }

  // already?
  if( obj.tt == MRB_TT_CLASS ) {
    return obj.cls;
  }

  // error.
  // raise TypeError.
  assert( !"TypeError" );
}



//================================================================
/*!@brief
  define class method or instance method.

  @param  vm		pointer to vm.
  @param  cls		pointer to class.
  @param  name		method name.
  @param  cfunc		pointer to function.
*/
void mrbc_define_method(mrb_vm *vm, mrb_class *cls, const char *name, mrb_func_t cfunc)
{
  mrb_proc *rproc = mrbc_rproc_alloc(vm, name);
  rproc->c_func = 1;  // c-func
  rproc->next = cls->procs;
  cls->procs = rproc;
  rproc->func = cfunc;
}


// Call a method
// v[0]: receiver
// v[1..]: params
//================================================================
/*!@brief
  call a method with params

  @param  vm		pointer to vm
  @param  name		method name
  @param  v		receiver and params
  @param  argc		num of params
*/
void mrbc_funcall(mrb_vm *vm, const char *name, mrb_value *v, int argc)
{
  mrb_sym sym_id = str_to_symid(name);
  mrb_proc *m = find_method(vm, v[0], sym_id);

  if( m==0 ) return;   // no initialize method
  // call initialize method

  mrb_callinfo *callinfo = vm->callinfo + vm->callinfo_top;
  callinfo->reg_top = vm->reg_top;
  callinfo->pc_irep = vm->pc_irep;
  callinfo->pc = vm->pc;
  callinfo->n_args = 0;
  callinfo->target_class = vm->target_class;
  vm->callinfo_top++;

  // target irep
  vm->pc = 0;
  vm->pc_irep = m->irep;

  // new regs
  vm->reg_top += 2;   // recv and symbol(:new)

}



//================================================================
// Object class

// Object - puts
void c_puts(mrb_value *v)
{
  switch( v->tt ){
  case MRB_TT_FIXNUM:
    console_printf("%d", v->i);
    break;

  case MRB_TT_NIL:
    console_printf("(nil)");
    break;

  case MRB_TT_TRUE:
    console_print("true");
    break;

  case MRB_TT_FALSE:
    console_print("false");
    break;

#if MRBC_USE_FLOAT
  case MRB_TT_FLOAT:
    console_printf("%f", v->d);
    break;
#endif

  case MRB_TT_STRING:
    console_print(MRBC_STRING_CSTR(v));
    break;

  case MRB_TT_RANGE:
    c_puts(&v->h_range->first);
    console_printf("..");
    if( v->h_range->flag_exclude ) console_putchar('.');
    c_puts(&v->h_range->last);
    break;

  case MRB_TT_ARRAY:{
    mrb_value *array = v->h_array->data;
    int i, n = v->h_array->n_stored;
    console_putchar('[');
    for( i = 0 ; i < n ; i++ ) {
      if( i > 0 ) console_print(", ");
      c_puts(array + i);
    }
    console_putchar(']');
  } break;

  case MRB_TT_HASH:{
    mrb_value *data = v->h_hash->data;
    int i, n = v->h_hash->n_stored;
    console_putchar('{');
    for( i = 0 ; i < n ; i++ ) {
      if( i > 0 ) console_print(", ");
      c_puts(data + i);
      i++;
      console_printf("=>");
      c_puts(data + i);
    }
    console_putchar('}');
  } break;

  case MRB_TT_CLASS:
    console_print( symid_to_str( v->cls->sym_id ) );
    break;

  case MRB_TT_SYMBOL:
    console_print( symid_to_str( v->i ) );
    break;

  default:
    console_printf("Not supported: MRB_TT_XX(%d)", v->tt);
    break;
  }
}

static void c_puts_nl(mrb_vm *vm, mrb_value *v, int argc)
{
  int i;
  for( i = 0; i < argc; i++ ) {
    c_puts(v+i+1);
    console_putchar('\n');
  }
}


static void c_object_not(mrb_vm *vm, mrb_value *v, int argc)
{
  mrbc_release(v);
  SET_FALSE_RETURN();
}

// Object !=
static void c_object_neq(mrb_vm *vm, mrb_value *v, int argc)
{
  int result = mrbc_eq(v, v+1);

  mrbc_release(v);
  if( result ) {
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}

// Object#class
static void c_object_class(mrb_vm *vm, mrb_value *v, int argc)
{
#if MRBC_USE_STRING
  mrb_class *cls = find_class_by_object( vm, v );
  mrb_value value = mrbc_string_new_cstr(vm, symid_to_str(cls->sym_id) );
  mrbc_release(v);
  SET_RETURN(value);
#endif
}


// Object.new
static void c_object_new(mrb_vm *vm, mrb_value *v, int argc)
{
  *v = mrbc_instance_new(vm, v->cls, 0);
  // call "initialize"
  mrbc_funcall(vm, "initialize", v, argc);
}


// Object.debug
#ifdef MRBC_DEBUG
static void c_object_debug(mrb_vm *vm, mrb_value *v, int argc)
{
  debug_all_symbols();
}


static void c_object_instance_methods(mrb_vm *vm, mrb_value *v, int argc)
{
  // TODO: check argument.

  // temporary code for operation check.
  console_printf( "[" );
  int flag_first = 1;

  mrb_class *cls = find_class_by_object( vm, v );
  mrb_proc *proc = cls->procs;
  while( proc ) {
    console_printf( "%s:%s", (flag_first ? "" : ", "),
		    symid_to_str(proc->sym_id) );
    flag_first = 0;
    proc = proc->next;
  }

  console_printf( "]" );

  mrbc_release(v);
  SET_NIL_RETURN();
}
#endif


static void mrbc_init_class_object(mrb_vm *vm)
{
  // Class
  mrbc_class_object = mrbc_define_class(vm, "Object", 0);
  // Methods
  mrbc_define_method(vm, mrbc_class_object, "puts", c_puts_nl);
  mrbc_define_method(vm, mrbc_class_object, "!", c_object_not);
  mrbc_define_method(vm, mrbc_class_object, "!=", c_object_neq);
  mrbc_define_method(vm, mrbc_class_object, "class", c_object_class);
  mrbc_define_method(vm, mrbc_class_object, "new", c_object_new);

#ifdef MRBC_DEBUG
  mrbc_define_method(vm, mrbc_class_object, "debug", c_object_debug);
  mrbc_define_method(vm, mrbc_class_object, "instance_methods", c_object_instance_methods);
#endif
}

// =============== ProcClass

static void c_proc_call(mrb_vm *vm, mrb_value *v, int argc)
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
  vm->pc_irep = v->proc->irep;
}


static void mrbc_init_class_proc(mrb_vm *vm)
{
  // Class
  mrbc_class_proc= mrbc_define_class(vm, "Proc", mrbc_class_object);
  // Methods
  mrbc_define_method(vm, mrbc_class_proc, "call", c_proc_call);
}


//================================================================
// Nil class

static void c_nil_false_not(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_TRUE_RETURN();
}

static void mrbc_init_class_nil(mrb_vm *vm)
{
  // Class
  mrbc_class_nil = mrbc_define_class(vm, "NilClass", mrbc_class_object);
  // Methods
  mrbc_define_method(vm, mrbc_class_nil, "!", c_nil_false_not);
}



//================================================================
// False class

static void mrbc_init_class_false(mrb_vm *vm)
{
  // Class
  mrbc_class_false = mrbc_define_class(vm, "FalseClass", mrbc_class_object);
  // Methods
  mrbc_define_method(vm, mrbc_class_false, "!", c_nil_false_not);
}



//================================================================
// True class

static void mrbc_init_class_true(mrb_vm *vm)
{
  // Class
  mrbc_class_true = mrbc_define_class(vm, "TrueClass", mrbc_class_object);
  // Methods
}



//================================================================
/*! Ineffect operator / method
*/
void c_ineffect(mrb_vm *vm, mrb_value *v, int argc)
{
  // nothing to do.
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
