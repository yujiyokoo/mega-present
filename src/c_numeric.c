/*! @file
  @brief
  mruby/c Integer and Float class

  <pre>
  Copyright (C) 2015-2021 Kyushu Institute of Technology.
  Copyright (C) 2015-2021 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/


#include "vm_config.h"
#include "opcode.h"
#include <stdio.h>
#include <limits.h>
#if MRBC_USE_FLOAT
#include <math.h>
#endif

#include "value.h"
#include "class.h"
#include "console.h"
#include "c_numeric.h"
#include "c_string.h"


//================================================================
/*! (operator) [] bit reference
 */
static void c_integer_bitref(struct VM *vm, mrbc_value v[], int argc)
{
  if( 0 <= v[1].i && v[1].i < 32 ) {
    SET_INT_RETURN( (v[0].i & (1 << v[1].i)) ? 1 : 0 );
  } else {
    SET_INT_RETURN( 0 );
  }
}


//================================================================
/*! (operator) unary +
*/
static void c_integer_positive(struct VM *vm, mrbc_value v[], int argc)
{
  // do nothing
}


//================================================================
/*! (operator) unary -
*/
static void c_integer_negative(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_int num = GET_INT_ARG(0);
  SET_INT_RETURN( -num );
}


//================================================================
/*! (operator) ** power
 */
static void c_integer_power(struct VM *vm, mrbc_value v[], int argc)
{
  if( v[1].tt == MRBC_TT_INTEGER ) {
    mrbc_int x = 1;
    int i;

    if( v[1].i < 0 ) x = 0;
    for( i = 0; i < v[1].i; i++ ) {
      x *= v[0].i;;
    }
    SET_INT_RETURN( x );
  }

#if MRBC_USE_FLOAT && MRBC_USE_MATH
  else if( v[1].tt == MRBC_TT_FLOAT ) {
    SET_FLOAT_RETURN( pow( v[0].i, v[1].d ) );
  }
#endif
}


//================================================================
/*! (operator) %
 */
static void c_integer_mod(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_int num = GET_INT_ARG(1);
  SET_INT_RETURN( v->i % num );
}


//================================================================
/*! (operator) &; bit operation AND
 */
static void c_integer_and(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_int num = GET_INT_ARG(1);
  SET_INT_RETURN(v->i & num);
}


//================================================================
/*! (operator) |; bit operation OR
 */
static void c_integer_or(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_int num = GET_INT_ARG(1);
  SET_INT_RETURN(v->i | num);
}


//================================================================
/*! (operator) ^; bit operation XOR
 */
static void c_integer_xor(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_int num = GET_INT_ARG(1);
  SET_INT_RETURN( v->i ^ num );
}


//================================================================
/*! (operator) ~; bit operation NOT
 */
static void c_integer_not(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_int num = GET_INT_ARG(0);
  SET_INT_RETURN( ~num );
}


//================================================================
/*! x-bit left shift for x
 */
static mrbc_int shift(mrbc_int x, mrbc_int y)
{
  // Don't support environments that include padding in int.
  const int INT_BITS = sizeof(mrbc_int) * CHAR_BIT;

  if( y >= INT_BITS ) return 0;
  if( y >= 0 ) return x << y;
  if( y <= -INT_BITS ) return 0;
  return x >> -y;
}


//================================================================
/*! (operator) <<; bit operation LEFT_SHIFT
 */
static void c_integer_lshift(struct VM *vm, mrbc_value v[], int argc)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( shift(v->i, num) );
}


//================================================================
/*! (operator) >>; bit operation RIGHT_SHIFT
 */
static void c_integer_rshift(struct VM *vm, mrbc_value v[], int argc)
{
  int num = GET_INT_ARG(1);
  SET_INT_RETURN( shift(v->i, -num) );
}


//================================================================
/*! (method) abs
*/
static void c_integer_abs(struct VM *vm, mrbc_value v[], int argc)
{
  if( v[0].i < 0 ) {
    v[0].i = -v[0].i;
  }
}


#if MRBC_USE_FLOAT
//================================================================
/*! (method) to_f
*/
static void c_integer_to_f(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_float f = GET_INT_ARG(0);
  SET_FLOAT_RETURN( f );
}
#endif


#if MRBC_USE_STRING
//================================================================
/*! (method) chr
*/
static void c_integer_chr(struct VM *vm, mrbc_value v[], int argc)
{
  char buf[2] = { GET_INT_ARG(0) };

  mrbc_value value = mrbc_string_new(vm, buf, 1);
  SET_RETURN(value);
}


//================================================================
/*! (method) to_s
*/
static void c_integer_to_s(struct VM *vm, mrbc_value v[], int argc)
{
  int base = 10;
  if( argc ) {
    base = GET_INT_ARG(1);
    if( base < 2 || base > 36 ) {
      return;	// raise ? ArgumentError
    }
  }

  mrbc_printf pf;
  char buf[16];
  mrbc_printf_init( &pf, buf, sizeof(buf), NULL );
  pf.fmt.type = 'd';
  mrbc_printf_int( &pf, v->i, base );
  mrbc_printf_end( &pf );

  mrbc_value value = mrbc_string_new_cstr(vm, buf);
  SET_RETURN(value);
}
#endif


/* MRBC_AUTOGEN_METHOD_TABLE

  CLASS("Integer")
  FILE("method_table_integer.h")
  FUNC("mrbc_init_class_integer")

  METHOD( "[]",		c_integer_bitref )
  METHOD( "+@",		c_integer_positive )
  METHOD( "-@",		c_integer_negative )
  METHOD( "**",		c_integer_power )
  METHOD( "%",		c_integer_mod )
  METHOD( "&",		c_integer_and )
  METHOD( "|",		c_integer_or )
  METHOD( "^",		c_integer_xor )
  METHOD( "~",		c_integer_not )
  METHOD( "<<",		c_integer_lshift )
  METHOD( ">>",		c_integer_rshift )
  METHOD( "abs",	c_integer_abs )
  METHOD( "to_i",	c_ineffect )
#if MRBC_USE_FLOAT
  METHOD( "to_f",	c_integer_to_f )
#endif
#if MRBC_USE_STRING
  METHOD( "chr",	c_integer_chr )
  METHOD( "inspect",	c_integer_to_s )
  METHOD( "to_s",	c_integer_to_s )
#endif
*/
#include "method_table_integer.h"



// Float
#if MRBC_USE_FLOAT

//================================================================
/*! (operator) unary +
*/
static void c_float_positive(struct VM *vm, mrbc_value v[], int argc)
{
  // do nothing
}


//================================================================
/*! (operator) unary -
*/
static void c_float_negative(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_float num = GET_FLOAT_ARG(0);
  SET_FLOAT_RETURN( -num );
}


#if MRBC_USE_MATH
//================================================================
/*! (operator) ** power
 */
static void c_float_power(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_float n = 0;
  switch( v[1].tt ) {
  case MRBC_TT_INTEGER:	n = v[1].i;	break;
  case MRBC_TT_FLOAT:	n = v[1].d;	break;
  default:				break;
  }

  SET_FLOAT_RETURN( pow( v[0].d, n ));
}
#endif


//================================================================
/*! (method) abs
*/
static void c_float_abs(struct VM *vm, mrbc_value v[], int argc)
{
  if( v[0].d < 0 ) {
    v[0].d = -v[0].d;
  }
}


//================================================================
/*! (method) to_i
*/
static void c_float_to_i(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_int i = (mrbc_int)GET_FLOAT_ARG(0);
  SET_INT_RETURN( i );
}


#if MRBC_USE_STRING
//================================================================
/*! (method) to_s
*/
static void c_float_to_s(struct VM *vm, mrbc_value v[], int argc)
{
  char buf[16];

  snprintf( buf, sizeof(buf), "%g", v->d );
  mrbc_value value = mrbc_string_new_cstr(vm, buf);
  SET_RETURN(value);
}
#endif


/* MRBC_AUTOGEN_METHOD_TABLE

  CLASS("Float")
  FILE("method_table_float.h")
  FUNC("mrbc_init_class_float")

  METHOD( "+@",		c_float_positive )
  METHOD( "-@",		c_float_negative )
#if MRBC_USE_MATH
  METHOD( "**",		c_float_power )
#endif
  METHOD( "abs",	c_float_abs )
  METHOD( "to_i",	c_float_to_i )
  METHOD( "to_f",	c_ineffect )
#if MRBC_USE_STRING
  METHOD( "inspect",	c_float_to_s )
  METHOD( "to_s",	c_float_to_s )
#endif
*/
#include "method_table_float.h"

#endif  // MRBC_USE_FLOAT
