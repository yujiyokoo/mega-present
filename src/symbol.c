/*! @file
  @brief
  Symbol

  <pre>
  Copyright (C) 2015-2016 Kyushu Institute of Technology.
  Copyright (C) 2015-2016 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include <string.h>
#include "vm_config.h"
#include "symbol.h"
#include "console.h"


struct SYM_INDEX {
  uint16_t hash;	//!< hash value, returned by calc_hash().
  char    *pos;		//!< point to the symbol string. maybe in sym_table[].
};


static struct SYM_INDEX sym_index[MAX_SYMBOLS_COUNT];
static int sym_index_pos;	// point to the last(free) sym_index array.
static char  sym_table[MAX_SYMBOLS_SIZE];	// symbol string table.
static char *sym_table_pos = sym_table;	// point to the last(free) sym_table.


//================================================================
/*! Caliculate hash value.

  @param  str		Target string.
  @return uint16_t	Hash value.
*/
static uint16_t calc_hash(const char *str)
{
  uint16_t h = 0;

  while( *str != '\0' ) {
    h = h * 37 + *str;
    str++;
  }
  return h;
}


//================================================================
/*! Add symbol to symbol table.

  @param  str		Target string.
  @return mrb_sym	Symbol value.
  @retval -1		If error occurred.
*/
mrb_sym add_sym(const char *str)
{
  mrb_sym sym_id = str_to_symid(str);

  if( sym_id < 0 ) {
    // check overflow.
    if( sym_index_pos >= MAX_SYMBOLS_COUNT ) {
      console_printf( "Overflow %s '%s'\n", "MAX_SYMBOLS_COUNT", str );
      return -1;
    }
    int len = strlen(str);
    if( len == 0 ) return -1;
    len++;
    if( len > (MAX_SYMBOLS_SIZE - (sym_table_pos - sym_table)) ) {
      console_printf( "Overflow %s '%s'\n", "MAX_SYMBOLS_SIZE", str );
      return -1;
    }

    // ok! go.
    sym_index[sym_index_pos].hash = calc_hash(str);
    sym_index[sym_index_pos].pos = sym_table_pos;
    sym_id = sym_index_pos;
    sym_index_pos++;
    memcpy(sym_table_pos, str, len);
    sym_table_pos += len;
  }

  return sym_id;
}


//================================================================
/*! Convert string to symbol value.

  @param  str		Target string.
  @return mrb_sym	Symbol value.
*/
mrb_sym str_to_symid(const char *str)
{
  uint16_t h = calc_hash(str);
  int i;

  for( i = 0; i < sym_index_pos; i++ ) {
    if( sym_index[i].hash == h ) {
      if( strcmp(str, sym_index[i].pos) == 0 ) {
        return i;
      }
    }
  }
  return -1;
}


//================================================================
/*! Convert symbol value to string.

  @param  mrb_sym	Symbol value.
  @return const char*	String.
  @retval NULL		Invalid sym_id was given.
*/
const char* symid_to_str(mrb_sym sym_id)
{
  if( sym_id < 0 ) return NULL;
  if( sym_id >= sym_index_pos ) return NULL;

  return sym_index[sym_id].pos;
}


void debug_all_symbols(void)
{
#ifdef MRBC_DEBUG
  int i;
  for( i=0 ; i<sym_index_pos ; i++ ){
    console_printf("%03d:%s\n", i, sym_index[i].pos);
  }
#endif
}
