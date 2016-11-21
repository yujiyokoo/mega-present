#include <stdio.h>

#include "vm_config.h"
#include "c_numeric.h"
#include "alloc.h"
#include "class.h"
#include "static.h"
#include "value.h"
#include "console.h"

static void c_fixnum_eq(mrb_vm *vm, mrb_value *v)
{
  console_printf("ERROR ==\n");
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

// x-bit left shift for x
static int32_t shift(int32_t x, int32_t y)
{
  if( y >= 33 ){
    x = 0;
  } else if( y >= 0 ){
    x <<= y;
  } else if( y > -33 ){
    x = x >> -y;
  } else {
    x = 0;
  }
  return x;
} 

// Operator <<; bit operation LEFT_SHIFT
static void c_fixnum_lshift(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(0);
  SET_INT_RETURN( shift(v->value.i, num) );
}

// Operator >>; bit operation RIGHT_SHIFT
static void c_fixnum_rshift(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(0);
  SET_INT_RETURN( shift(v->value.i, -num) );
}

#if MRUBYC_USE_STRING
static void c_fixnum_to_s(mrb_vm *vm, mrb_value *v)
{
  int num = v->value.i;
  int i = 0, j = 0;
  char buf[10];
  int sign = 0;

  if( num < 0 ){
    sign = 1;
    num = -num;
  }
  do {
    buf[i++] = (num % 10) + '0';
    num = num / 10;
  } while( num > 0 );
  if( sign ){
    buf[i] = '-';
  } else {
    i--;
  }
  char *str = (char *)mrbc_alloc(vm, i+2);
  while( i>=0 ){
    str[j++] = buf[i--];
  }
  str[j] = 0;
  v->tt = MRB_TT_STRING;
  v->value.str = str;
}
#endif



void mrb_init_class_fixnum(mrb_vm *vm)
{
  // Fixnum
  static_class_fixnum = mrb_class_alloc(vm, "Fixnum", static_class_object);
  mrb_define_method(vm, static_class_fixnum, "==", c_fixnum_eq);
  mrb_define_method(vm, static_class_fixnum, "%", c_fixnum_mod);
  mrb_define_method(vm, static_class_fixnum, "<=>", c_fixnum_comp);
  mrb_define_method(vm, static_class_fixnum, "~", c_fixnum_deny);
  mrb_define_method(vm, static_class_fixnum, "&", c_fixnum_and);
  mrb_define_method(vm, static_class_fixnum, "<<", c_fixnum_lshift);
  mrb_define_method(vm, static_class_fixnum, ">>", c_fixnum_rshift);
#if MRUBYC_USE_STRING
  mrb_define_method(vm, static_class_fixnum, "to_s", c_fixnum_to_s);
#endif
}


// Float
#if MRUBYC_USE_FLOAT

void mrb_init_class_float(mrb_vm *vm)
{
  // Float
  static_class_float = mrb_class_alloc(vm, "Float", static_class_object);

}

#endif
