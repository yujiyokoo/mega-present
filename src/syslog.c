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
      printf("%d", value);
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
