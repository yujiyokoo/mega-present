#include <stdio.h>

#include "vm_config.h"
#include "c_numeric.h"
#include "alloc.h"
#include "class.h"
#include "static.h"
#include "value.h"
#include "console.h"
#include "c_string.h"


static void c_fixnum_eq(mrb_vm *vm, mrb_value *v)
{
  console_printf("ERROR ==\n");
}

// Operator %
static void c_fixnum_mod(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( v->i % num );
}

// Operator <=>
static void c_fixnum_comp(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(1);
  if(v->i > num){
    SET_INT_RETURN(1);
  }else if(v->i == num){
    SET_INT_RETURN(0);
  }else{
    SET_INT_RETURN(-1);
  }
}

// Unary Operator ~; bit operation NOT
static void c_fixnum_deny(mrb_vm *vm, mrb_value *v)
{
  SET_INT_RETURN( (v->i + 1) * (-1)  );
}


// Operator &; bit operation AND
static void c_fixnum_and(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN(v->i & num);
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
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( shift(v->i, num) );
}

// Operator >>; bit operation RIGHT_SHIFT
static void c_fixnum_rshift(mrb_vm *vm, mrb_value *v)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( shift(v->i, -num) );
}

#if MRBC_USE_STRING
static void c_fixnum_to_s(mrb_vm *vm, mrb_value *v)
{
  char buf[16];
  char *p = buf + sizeof(buf) - 1;
  int num = v->i;
  int flag_minus = (num < 0);

  if( flag_minus ) num = -num;

  *p = '\0';
  do {
    *--p = (num % 10) + '0';
    num /= 10;
  } while( num > 0 );

  if( flag_minus ) *--p = '-';

  v->tt = MRB_TT_STRING;
  v->handle = mrbc_string_constructor(vm, p);
}
#endif



void mrbc_init_class_fixnum(mrb_vm *vm)
{
  // Fixnum
  mrbc_class_fixnum = mrbc_class_alloc(vm, "Fixnum", mrbc_class_object);
  mrbc_define_method(vm, mrbc_class_fixnum, "==", c_fixnum_eq);
  mrbc_define_method(vm, mrbc_class_fixnum, "%", c_fixnum_mod);
  mrbc_define_method(vm, mrbc_class_fixnum, "<=>", c_fixnum_comp);
  mrbc_define_method(vm, mrbc_class_fixnum, "~", c_fixnum_deny);
  mrbc_define_method(vm, mrbc_class_fixnum, "&", c_fixnum_and);
  mrbc_define_method(vm, mrbc_class_fixnum, "<<", c_fixnum_lshift);
  mrbc_define_method(vm, mrbc_class_fixnum, ">>", c_fixnum_rshift);
#if MRBC_USE_STRING
  mrbc_define_method(vm, mrbc_class_fixnum, "to_s", c_fixnum_to_s);
#endif
}


// Float
#if MRBC_USE_FLOAT

// Operator unary -
static void c_float_negative(mrb_vm *vm, mrb_value *v)
{
  double num = GET_FLOAT_ARG(0);
  SET_FLOAT_RETURN( -num );
}



void mrbc_init_class_float(mrb_vm *vm)
{
  // Float
  mrbc_class_float = mrbc_class_alloc(vm, "Float", mrbc_class_object);
  mrbc_define_method(vm, mrbc_class_float, "-@", c_float_negative);
}

#endif
