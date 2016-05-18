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
#include <ctype.h>
#include "syslog.h"


// put char
// target dependent function: putchar
void syslog_putchar(const char c)
{
  putchar(c);
}

// simple string
void syslog_print(const char *str)
{
  char c;
  while( (c = *str++) ){
    syslog_putchar(c);
  }
}

// format string
void syslog_printf(const char *fmt, ...)
{
  va_list params;
  char buf[21];

  va_start(params, fmt);

  char c;
  while( (c = *fmt++) ){
    if( c != '%' ){
      syslog_putchar(c);
      continue;
    }
    c = *fmt++;

    switch( toupper(c) ){
    case 'D':{
      int value = va_arg(params, int);
      int sign = 1;
      if( value < 0 ){
	sign = -1;
	value = -value;
      }
      int idx = 0;
      while( value > 0 ){
	buf[idx++] = (value % 10) + '0';
	value /= 10;
      }
      
      if( sign < 0 ) syslog_putchar('-');
      while( --idx >= 0 ){
	syslog_putchar(buf[idx]);
      }

    } break;
    case '\0':
      return;
    default:
      syslog_putchar(c);
      continue;
    }

  }

  va_end(params);
}
