/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "hal/hal.h"
#include "console.h"


// put char
// target dependent function: putchar
void console_putchar(const char c)
{
  hal_write(1, &c, 1);
}

// simple string
void console_print(const char *str)
{
  char c;
  while( (c = *str++) ){
    console_putchar(c);
  }
}

// format output
static void console_print_value(int value, int dir, int w, int base, char pad)
{
  char buf[21];
  const char hex[16] = "0123456789ABCDEF";

  int sign = 1;
  if( value < 0 ){
    sign = -1;
    value = -value;
  }

  int idx = 0;
  while( value > 0 ){
    buf[idx++] = hex[value % base];
    value /= base;
  }

  if( sign == -1 ) w--;
  if( idx > w ) w = idx;

  int n_pad = w - idx;
  if( dir == 1 ){
    while( n_pad-- > 0 ){
      console_putchar(pad);
    }
  }

  if( sign < 0 ) console_putchar('-');

  if( idx==0 ){
    console_putchar('0');
  } else {
    while( --idx >= 0 ) {
      console_putchar(buf[idx]);
    }
  }

  if( dir == -1 ){
    while( n_pad-- > 0 ){
      console_putchar(pad);
    }
  }
}


// format string
void console_printf(const char *fmt, ...)
{
  va_list params;

  va_start(params, fmt);

  char c;
  while( (c = *fmt++) ){
    if( c != '%' ){
      console_putchar(c);
      continue;
    }

    int dir = 1;     // align left(-1) or right(1)
    char pad = ' ';  // padding
    int w = 0;       // width
    while( (c = *fmt++) ){
      switch( c ){
      case '-':
	dir = -1;
	break;
      case '0':
	pad = '0';
	break;
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
    switch( toupper(c) ){
    case 'S':{
      char *ptr = va_arg(params, char*);
      int n_pad = w - strlen(ptr);
      if( n_pad < 0 ) n_pad = 0;
      if( dir == 1 ){
	while( n_pad-- > 0 ){
	  console_putchar(pad);
	}
      }
      console_print(ptr);
      if( dir == -1 ){
	while( n_pad-- > 0 ){
	  console_putchar(pad);
	}
      }
    } break;
    case 'D':
      console_print_value(va_arg(params, int), dir, w, 10, pad);
      break;
    case 'X':
      console_print_value(va_arg(params, int), dir, w, 16, pad);
      break;
    case '\0':
      return;
    default:
      console_putchar(c);
      continue;
    }

  }

  va_end(params);
}
