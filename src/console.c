/*! @file
  @brief
  console I/O module.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "hal/hal.h"
#include "vm_config.h"
#include "console.h"
#if MRBC_USE_FLOAT
#include <stdio.h>
#endif


//================================================================
/*! output string with format

  @param  value		output value
  @param  align		left(-1) or right(1)
  @param  w		width
  @param  base		n base
  @param  pad		padding character
*/
static void format_output_str(const char *value, int align, int w, char pad)
{
  if( !value ) return;

  int len = strlen(value);
  int n_pad = w - len;

  if( align == 1 ) {
    while( n_pad-- > 0 ) {
      console_putchar(pad);
    }
  }
  hal_write(1, value, len);
  while( n_pad-- > 0 ) {
    console_putchar(pad);
  }
}


//================================================================
/*! output int value with format

  @param  value		output value
  @param  align		left(-1) or right(1)
  @param  w		width
  @param  base		n base
  @param  pad		padding character
*/
static void format_output_int(int32_t value, int align, int w, int base, char pad)
{
  char buf[21];
  int sign = 0;
  int idx = sizeof(buf);
  buf[--idx] = 0;

  if( value < 0 ) {
    sign  = -1;
    value = -value;
  }

  do {
    buf[--idx] = "0123456789ABCDEF"[value % base];
    value /= base;
  } while( value != 0 && idx != 0 );

  if( sign < 0 && align > 0 && pad == '0' ) {
    console_putchar('-');	// when "%08d",-12345 then "-0012345"
    w--;
  } else if( sign < 0 && idx != 0 ) {
    buf[--idx] = '-';
  }

  format_output_str(buf + idx, align, w, pad);
}


//================================================================
/*! output unsigned int value with format

  @param  value		output value
  @param  align		left(-1) or right(1)
  @param  w		width
  @param  base		n base
  @param  pad		padding character
*/
static void format_output_uint(uint32_t value, int align, int w, int base, char pad)
{
  char buf[21];
  int idx = sizeof(buf);
  buf[--idx] = 0;

  do {
    buf[--idx] = "0123456789ABCDEF"[value % base];
    value /= base;
  } while( value != 0 && idx != 0 );

  format_output_str(buf + idx, align, w, pad);
}


//================================================================
/*! output double value with format

  @param  value		output value
  @param  align		left(-1) or right(1)
  @param  w		width
  @param  pad		padding character
*/
#if MRBC_USE_FLOAT
static void format_output_float(double value, int align, int w, char pad)
{
  char buf[21];
  sprintf(buf, "%f", value);
  console_print(buf);
}
#endif

//================================================================
/*! output a character

  @param  c	character
*/
void console_putchar(char c)
{
  hal_write(1, &c, 1);
}


//================================================================
/*! output string

  @param str	str
*/
void console_print(const char *str)
{
  hal_write(1, str, strlen(str));
}


//================================================================
/*! output formatted string

  @param  fmt		format string.
  @note
*/
void console_printf(const char *fmt, ...)
{
  va_list params;
  va_start(params, fmt);

  int c;
  while((c = *fmt++)) {
    if( c != '%' ) {
      console_putchar(c);
      continue;
    }

    int  align = 1;	// left(-1) or right(1)
    char pad   = ' ';	// padding
    int  w     = 0;	// width
    while( 1 ) {
      switch( (c = *fmt++) ) {
      case '-':
        align = -1;
        break;

      case '0':
        if( pad == ' ' ) {
          pad = '0';
          break;
        }
	// fall through.

      case '1': case '2': case '3': case '4': case '5':
      case '6': case '7': case '8': case '9':
        w = w * 10 + (c - '0');
        break;

      case '\0':
	goto L_return;

      default:
        goto L_exit;
      }
    }

L_exit:
    switch(c) {
    case 's':
      format_output_str(va_arg(params, char *), align, w, pad);
      break;

    case 'd':
    case 'i':
      format_output_int(va_arg(params, int), align, w, 10, pad);
      break;

    case 'u':
      format_output_uint(va_arg(params, unsigned int), align, w, 10, pad);
      break;

    case 'X':
    case 'x':
      format_output_uint(va_arg(params, unsigned int), align, w, 16, pad);
      break;

#if MRBC_USE_FLOAT
    case 'F':
    case 'f':
      format_output_float(va_arg(params, double), align, w, pad);
      break;
#endif

    case 'c':
      console_putchar(va_arg(params, int));	// ignore "%03c" and others.
      break;

    default:
      console_putchar(c);
    }
  }

L_return:
  va_end(params);
}
