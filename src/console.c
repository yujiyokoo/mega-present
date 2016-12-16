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
#include "hal/hal.h"
#include "console.h"


//================================================================
/*! output int value with format

  @param  value		output value
  @param  align		left(-1) or right(1)
  @param  w		width
  @param  base		n base
  @param  pad		padding character
*/
static void format_output_int(int value, int align, int w, int base, char pad)
{
  char buf[21];
  int idx = 0;
  int sign = 0;

  if( value < 0 ) {
    sign  = -1;
    value = -value;
  }

  do {
    buf[idx++] = "0123456789ABCDEF"[value % base];
    value /= base;
  } while( value > 0 );

  if( sign < 0 && align > 0 && pad == '0' ) {
    console_putchar('-');	// when "%08d",-12345 then "-0012345"
    w--;
  } else if( sign < 0 ) {
    buf[idx++] = '-';
  }

  int n_pad = w - idx;
  if( align == 1 ) {
    while( n_pad-- > 0 ) {
      console_putchar(pad);
    }
  }
  while( --idx >= 0 ) {
    console_putchar(buf[idx]);
  }
  if( align == -1 ) {
    while( n_pad-- > 0 ) {
      console_putchar(pad);
    }
  }
}


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

  char c;
  while((c = *fmt++)) {
    if( c != '%' ) {
      console_putchar(c);
      continue;
    }

    int  align = 1;	// left(-1) or right(1)
    char pad   = ' ';	// padding
    int  w     = 0;	// width
    while((c = *fmt++)) {
      switch( c ) {
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

      default:
        goto L_exit;
      }
    }
    return;

L_exit:
    switch(c) {
    case 'S':
    case 's': {
      char *ptr   = va_arg(params, char *);
      int   n_pad = w - strlen(ptr);
      if( n_pad < 0 ) n_pad = 0;
      if( align == 1 ) {
        while( n_pad-- > 0 ) {
          console_putchar(pad);
        }
      }
      console_print(ptr);
      if( align == -1 ) {
        while( n_pad-- > 0 ) {
          console_putchar(pad);
        }
      }
    } break;

    case 'D':
    case 'd':
      format_output_int(va_arg(params, int), align, w, 10, pad);
      break;

    case 'X':
    case 'x':
      format_output_int(va_arg(params, int), align, w, 16, pad);
      break;

    default:
      console_putchar(c);
    }
  }

  va_end(params);
}
