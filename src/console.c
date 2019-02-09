/*! @file
  @brief
  console output module. (not yet input)

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include "vm_config.h"
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#if MRBC_USE_FLOAT
#include <stdio.h>
#endif
#include "value.h"
#include "alloc.h"
#include "console.h"


//================================================================
/*! output formatted string

  @param  fstr		format string.
*/
void console_printf(const char *fstr, ...)
{
  va_list ap;
  va_start(ap, fstr);

  mrbc_printf pf;
  char buf[82];
  mrbc_printf_init( &pf, buf, sizeof(buf), fstr );

  int ret;
  while( 1 ) {
    ret = mrbc_printf_main( &pf );
    if( mrbc_printf_len( &pf ) ) {
      hal_write(1, buf, mrbc_printf_len( &pf ));
      mrbc_printf_clear( &pf );
    }
    if( ret == 0 ) break;
    if( ret < 0 ) continue;
    if( ret > 0 ) {
      switch(pf.fmt.type) {
      case 'c':
	ret = mrbc_printf_char( &pf, va_arg(ap, int) );
	break;

      case 's':
	ret = mrbc_printf_str( &pf, va_arg(ap, char *), ' ');
	break;

      case 'd':
      case 'i':
      case 'u':
	ret = mrbc_printf_int( &pf, va_arg(ap, int), 10);
	break;

      case 'D':	// for mrbc_int (see mrbc_print_sub in class.c)
	ret = mrbc_printf_int( &pf, va_arg(ap, mrbc_int), 10);
	break;

      case 'b':
      case 'B':
	ret = mrbc_printf_bit( &pf, va_arg(ap, unsigned int), 1);
	break;

      case 'x':
      case 'X':
	ret = mrbc_printf_bit( &pf, va_arg(ap, unsigned int), 4);
	break;

#if MRBC_USE_FLOAT
      case 'f':
      case 'e':
      case 'E':
      case 'g':
      case 'G':
	ret = mrbc_printf_float( &pf, va_arg(ap, double) );
	break;
#endif

      default:
	break;
      }

      hal_write(1, buf, mrbc_printf_len( &pf ));
      mrbc_printf_clear( &pf );
    }
  }

  va_end(ap);
}



//================================================================
/*! sprintf subcontract function

  @param  pf	pointer to mrbc_printf
  @retval 0	(format string) done.
  @retval 1	found a format identifier.
  @retval -1	buffer full.
  @note		not terminate ('\0') buffer tail.
*/
int mrbc_printf_main( mrbc_printf *pf )
{
  int ch = -1;
  pf->fmt = (struct RPrintfFormat){0};

  while( pf->p < pf->buf_end && (ch = *pf->fstr) != '\0' ) {
    pf->fstr++;
    if( ch == '%' ) {
      if( *pf->fstr == '%' ) {	// is "%%"
	pf->fstr++;
      } else {
	goto PARSE_FLAG;
      }
    }
    *pf->p++ = ch;
  }
  return -(ch != '\0');


 PARSE_FLAG:
  // parse format - '%' [flag] [width] [.precision] type
  //   e.g. "%05d"
  while( (ch = *pf->fstr) ) {
    switch( ch ) {
    case '+': pf->fmt.flag_plus = 1; break;
    case ' ': pf->fmt.flag_space = 1; break;
    case '-': pf->fmt.flag_minus = 1; break;
    case '0': pf->fmt.flag_zero = 1; break;
    default : goto PARSE_WIDTH;
    }
    pf->fstr++;
  }

 PARSE_WIDTH:
  while( (ch = *pf->fstr - '0'), (0 <= ch && ch <= 9)) {	// isdigit()
    pf->fmt.width = pf->fmt.width * 10 + ch;
    pf->fstr++;
  }
  if( *pf->fstr == '.' ) {
    pf->fstr++;
    while( (ch = *pf->fstr - '0'), (0 <= ch && ch <= 9)) {
      pf->fmt.precision = pf->fmt.precision * 10 + ch;
      pf->fstr++;
    }
  }
  if( *pf->fstr ) pf->fmt.type = *pf->fstr++;

  return 1;
}



//================================================================
/*! sprintf subcontract function for char '%c'

  @param  pf	pointer to mrbc_printf
  @param  ch	output character (ASCII)
  @retval 0	done.
  @retval -1	buffer full.
  @note		not terminate ('\0') buffer tail.
*/
int mrbc_printf_char( mrbc_printf *pf, int ch )
{
  if( pf->fmt.flag_minus ) {
    if( pf->p == pf->buf_end ) return -1;
    *pf->p++ = ch;
  }

  int width = pf->fmt.width;
  while( --width > 0 ) {
    if( pf->p == pf->buf_end ) return -1;
    *pf->p++ = ' ';
  }

  if( !pf->fmt.flag_minus ) {
    if( pf->p == pf->buf_end ) return -1;
    *pf->p++ = ch;
  }

  return 0;
}


//================================================================
/*! sprintf subcontract function for byte array.

  @param  pf	pointer to mrbc_printf.
  @param  str	pointer to byte array.
  @param  len	byte length.
  @param  pad	padding character.
  @retval 0	done.
  @retval -1	buffer full.
  @note		not terminate ('\0') buffer tail.
*/
int mrbc_printf_bstr( mrbc_printf *pf, const char *str, int len, int pad )
{
  int ret = 0;

  if( str == NULL ) {
    str = "(null)";
    len = 6;
  }
  if( pf->fmt.precision && len > pf->fmt.precision ) len = pf->fmt.precision;

  int tw = len;
  if( pf->fmt.width > len ) tw = pf->fmt.width;

  int remain = pf->buf_end - pf->p;
  if( len > remain ) {
    len = remain;
    ret = -1;
  }
  if( tw > remain ) {
    tw = remain;
    ret = -1;
  }

  int n_pad = tw - len;

  if( !pf->fmt.flag_minus ) {
    while( n_pad-- > 0 ) {
      *pf->p++ = pad;
    }
  }
  while( len-- > 0 ) {
    *pf->p++ = *str++;
  }
  while( n_pad-- > 0 ) {
    *pf->p++ = pad;
  }

  return ret;
}



//================================================================
/*! sprintf subcontract function for integer '%d', '%u'

  @param  pf	pointer to mrbc_printf.
  @param  value	output value.
  @param  base	n base.
  @retval 0	done.
  @retval -1	buffer full.
  @note		not terminate ('\0') buffer tail.
*/
int mrbc_printf_int( mrbc_printf *pf, mrbc_int value, int base )
{
  int sign = 0;
  mrbc_int v = value;

  if( value < 0 ) {
    sign = '-';
    v = -value;
  } else if( pf->fmt.flag_plus ) {
    sign = '+';
  } else if( pf->fmt.flag_space ) {
    sign = ' ';
  }

  if( pf->fmt.flag_minus || pf->fmt.width == 0 ) {
    pf->fmt.flag_zero = 0; // disable zero padding if left align or width zero.
  }

  // create string to allocated buffer
  int alloc_size = 32+2;	// int32 + terminate + 1
  if( alloc_size < pf->fmt.precision + 1 ) alloc_size = pf->fmt.precision + 1;
  assert( sizeof(mrbc_int) * 8 < alloc_size );

  char *buf = mrbc_raw_alloc( alloc_size );
  if( buf == NULL ) return 0;	// ENOMEM

  char *p = buf + alloc_size - 1;
  *p = '\0';
  do {
    assert( p != buf );
    int i = v % base;
    *--p = i + ((i < 10)? '0' : 'a' - 10);
    v /= base;
  } while( v != 0 );

  // precision parameter
  int precision_remain = (int)pf->fmt.precision - ((buf + alloc_size - 1) - p);
  while( --precision_remain >= 0 ) {
    *--p = '0';
  }
  pf->fmt.precision = 0;

  // decide pad character and output sign character
  int ret;
  int pad;
  if( pf->fmt.flag_zero ) {
    pad = '0';
    if( sign ) {
      *pf->p++ = sign;
      if( pf->p >= pf->buf_end ) {
	ret = -1;
	goto DONE;
      }
      pf->fmt.width--;
    }
  } else {
    pad = ' ';
    if( sign ) *--p = sign;
  }
  ret = mrbc_printf_str( pf, p, pad );

 DONE:
  mrbc_raw_free( buf );
  return ret;
}



//================================================================
/*! sprintf subcontract function for '%x', '%o', '%b'

  @param  pf	pointer to mrbc_printf.
  @param  value	output value.
  @param  bit	n bit. (4=hex, 3=oct, 1=bin)
  @retval 0	done.
  @retval -1	buffer full.
  @note		not terminate ('\0') buffer tail.
*/
int mrbc_printf_bit( mrbc_printf *pf, mrbc_int value, int bit )
{
  if( pf->fmt.flag_plus || pf->fmt.flag_space ) {
    return mrbc_printf_int( pf, value, 1 << bit );
  }

  if( pf->fmt.flag_minus || pf->fmt.width == 0 ) {
    pf->fmt.flag_zero = 0; // disable zero padding if left align or width zero.
  }
  pf->fmt.precision = 0;

  mrbc_int v = value;
  int offset_a = (pf->fmt.type == 'X') ? 'A' - 10 : 'a' - 10;
  int mask = (1 << bit) - 1;	// 0x0f, 0x07, 0x01
  int mchar = mask + ((mask < 10)? '0' : offset_a);

  // create string to local buffer
  char buf[40];	// > int32(bit) + '..f\0'
  assert( sizeof(buf) > (sizeof(mrbc_int) * 8 + 4) );
  char *p = buf + sizeof(buf) - 1;
  *p = '\0';
  int n;
  do {
    assert( p >= buf );
    n = v & mask;
    *--p = n + ((n < 10)? '0' : offset_a);
    v >>= bit;
  } while( v != 0 && v != -1 );

  // add "..f" for negative value?
  // (note) '0' flag such as "%08x" is incompatible with ruby.
  if( value < 0 && !pf->fmt.flag_zero ) {
    if( n != mask ) *--p = mchar;
    *--p = '.';
    *--p = '.';
    assert( p >= buf );
  }

  // decide pad character and output sign character
  int pad;
  if( pf->fmt.flag_zero ) {
    pad = (value < 0) ? mchar : '0';
  } else {
    pad = ' ';
  }

  return mrbc_printf_str( pf, p, pad );
}



#if MRBC_USE_FLOAT
//================================================================
/*! sprintf subcontract function for float(double) '%f'

  @param  pf	pointer to mrbc_printf.
  @param  value	output value.
  @retval 0	done.
  @retval -1	buffer full.
*/
int mrbc_printf_float( mrbc_printf *pf, double value )
{
  char fstr[16];
  const char *p1 = pf->fstr;
  char *p2 = fstr + sizeof(fstr) - 1;

  *p2 = '\0';
  while( (*--p2 = *--p1) != '%' )
    ;

  snprintf( pf->p, (pf->buf_end - pf->p + 1), p2, value );

  while( *pf->p != '\0' )
    pf->p++;

  return -(pf->p == pf->buf_end);
}
#endif



//================================================================
/*! replace output buffer

  @param  pf	pointer to mrbc_printf
  @param  buf	pointer to output buffer.
  @param  size	buffer size.
*/
void mrbc_printf_replace_buffer(mrbc_printf *pf, char *buf, int size)
{
  int p_ofs = pf->p - pf->buf;
  pf->buf = buf;
  pf->buf_end = buf + size - 1;
  pf->p = pf->buf + p_ofs;
}
