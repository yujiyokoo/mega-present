#include <stdio.h>

#include "c_numeric.h"

#include "class.h"
#include "static.h"
#include "value.h"
#include "syslog.h"

static void c_fixnum_eq(mrb_vm *vm, mrb_value *v)
{
  syslog_message("ERROR ==\n");
}

// Operator %
static void c_fixnum_mod(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(0);
  SET_INT_RETURN( v->value.i % num );
}

// Operator <=>
static void c_fixnum_comp(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(0);
  if(v->value.i > num){
  SET_INT_RETURN(1);
  }else if(v->value.i == num){
  SET_INT_RETURN(0);
  }else{
  SET_INT_RETURN(-1);
  }
}

// Unary Operator ~; bit operation NOT
static void c_fixnum_deny(mrb_vm *vm, mrb_value *v)
{
  SET_INT_RETURN( (v->value.i + 1) * (-1)  );
}


// Operator &; bit operation AND
static void c_fixnum_and(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(0);
  SET_INT_RETURN(v->value.i & num);
}

// Operator <<; bit operation LEFT_SHIFT
static void c_fixnum_lshift(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(0);
  SET_INT_RETURN(v->value.i << num);
}

// Operator <<; bit operation RIGHT_SHIFT
static void c_fixnum_rshift(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(0);
  SET_INT_RETURN(v->value.i >> num);
}

void mrb_init_class_fixnum(void)
{
  // Fixnum
  static_class_fixnum = mrb_class_alloc("Fixnum", static_class_object);
  mrb_define_method(static_class_fixnum, "==", c_fixnum_eq);
  mrb_define_method(static_class_fixnum, "%", c_fixnum_mod);
  mrb_define_method(static_class_fixnum, "<=>", c_fixnum_comp);
  mrb_define_method(static_class_fixnum, "~", c_fixnum_deny);
  mrb_define_method(static_class_fixnum, "&", c_fixnum_and);
  mrb_define_method(static_class_fixnum, "<<", c_fixnum_lshift);
  mrb_define_method(static_class_fixnum, ">>", c_fixnum_rshift);
}


// Float
#if MRUBYC_USE_FLOAT

void mrb_init_class_float(void)
{
  // Float
  static_class_float = mrb_class_alloc("Float", static_class_object);

}

#endif
