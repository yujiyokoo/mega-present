#include <stdio.h>

#include "vm_config.h"
#include "c_numeric.h"
#include "alloc.h"
#include "class.h"
#include "static.h"
#include "value.h"
#include "console.h"
#include "c_string.h"


// Operator %
static void c_fixnum_mod(mrb_vm *vm, mrb_value *v, int argc)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( v->i % num );
}

// Operator <=>
static void c_fixnum_comp(mrb_vm *vm, mrb_value *v, int argc)
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
static void c_fixnum_deny(mrb_vm *vm, mrb_value *v, int argc)
{
  SET_INT_RETURN( (v->i + 1) * (-1)  );
}


// Operator &; bit operation AND
static void c_fixnum_and(mrb_vm *vm, mrb_value *v, int argc)
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
static void c_fixnum_lshift(mrb_vm *vm, mrb_value *v, int argc)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( shift(v->i, num) );
}

// Operator >>; bit operation RIGHT_SHIFT
static void c_fixnum_rshift(mrb_vm *vm, mrb_value *v, int argc)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( shift(v->i, -num) );
}

#if MRBC_USE_STRING
static void c_fixnum_to_s(mrb_vm *vm, mrb_value *v, int argc)
{
  int base = 10;
  if( argc ) {
	base = GET_INT_ARG(1);
	if( base < 2 || base > 36 ) {
	  return;	// raise ? ArgumentError
	}
  }

  MrbcPrintf pf;
  char buf[16];
  mrbc_printf_init( &pf, buf, sizeof(buf), NULL );
  pf.fmt.type = 'd';
  mrbc_printf_int( &pf, v->i, base );
  mrbc_printf_end( &pf );

  mrb_value value = mrbc_string_new_cstr(vm, buf);
  SET_RETURN(value);
}
#endif



void mrbc_init_class_fixnum(mrb_vm *vm)
{
  // Fixnum
  mrbc_class_fixnum = mrbc_class_alloc(vm, "Fixnum", mrbc_class_object);

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

//================================================================
/*! Operator unary -
*/
static void c_float_negative(mrb_vm *vm, mrb_value *v, int argc)
{
  double num = GET_FLOAT_ARG(0);
  SET_FLOAT_RETURN( -num );
}


//================================================================
/*! (method) to_i
*/
static void c_float_to_i(mrb_vm *vm, mrb_value *v, int argc)
{
  int i = (int)GET_FLOAT_ARG(0);
  SET_INT_RETURN( i );
}


void mrbc_init_class_float(mrb_vm *vm)
{
  // Float
  mrbc_class_float = mrbc_class_alloc(vm, "Float", mrbc_class_object);
  mrbc_define_method(vm, mrbc_class_float, "-@", c_float_negative);
  mrbc_define_method(vm, mrbc_class_float, "to_i", c_float_to_i);
}

#endif
