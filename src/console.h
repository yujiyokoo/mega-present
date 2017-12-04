/*! @file
  @brief
  console output module. (not yet input)

  <pre>
  Copyright (C) 2015-2017 Kyushu Institute of Technology.
  Copyright (C) 2015-2017 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_CONSOLE_H_
#define MRBC_SRC_CONSOLE_H_

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "hal/hal.h"

#ifdef __cplusplus
extern "C" {
#endif


//================================================================
/*! printf tiny (mruby/c) version data container.
*/
typedef struct MrbcPrintf {
  char *buf;
  const char *buf_end;
  char *p;
  const char *fstr;

  struct MrbcPrintfFormat {
    char type;
    unsigned int flag_plus : 1;
    unsigned int flag_minus : 1;
    unsigned int flag_space : 1;
    unsigned int flag_zero : 1;
    int width;
    int precision;
  } fmt;
} MrbcPrintf;



void console_printf(const char *fstr, ...);
int mrbc_printf_main(MrbcPrintf *pf);
int mrbc_printf_char(MrbcPrintf *pf, int ch);
int mrbc_printf_str(MrbcPrintf *pf, const char *str, int pad);
int mrbc_printf_int(MrbcPrintf *pf, int32_t value, int base);
int mrbc_printf_float( MrbcPrintf *pf, double value );
void mrbc_printf_replace_buffer(MrbcPrintf *pf, char *buf, int size);


//================================================================
/*! output a character

  @param  c	character
*/
static inline void console_putchar(char c)
{
  hal_write(1, &c, 1);
}


//================================================================
/*! output string

  @param str	str
*/
static inline void console_print(const char *str)
{
  hal_write(1, str, strlen(str));
}


//================================================================
/*! initialize data container.

  @param  pf	pointer to MrbcPrintf
  @param  buf	pointer to output buffer.
  @param  size	buffer size.
  @param  fstr	format string.
*/
static inline void mrbc_printf_init( MrbcPrintf *pf, char *buf, int size,
				     const char *fstr )
{
  pf->p = pf->buf = buf;
  pf->buf_end = buf + size - 1;
  pf->fstr = fstr;
  pf->fmt = (struct MrbcPrintfFormat){0};
}


//================================================================
/*! clear output buffer in container.

  @param  pf	pointer to MrbcPrintf
*/
static inline void mrbc_printf_clear( MrbcPrintf *pf )
{
  pf->p = pf->buf;
}


//================================================================
/*! terminate ('\0') output buffer.

  @param  pf	pointer to MrbcPrintf
*/
static inline void mrbc_printf_end( MrbcPrintf *pf )
{
  *pf->p = '\0';
}


//================================================================
/*! return buffer length

  @param  pf	pointer to MrbcPrintf
  @return	buffer length
*/
static inline int mrbc_printf_len( MrbcPrintf *pf )
{
  return pf->p - pf->buf;
}

#ifdef __cplusplus
}
#endif
#endif
