/*! @file
  @brief
  mruby/c Symbol class

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include "vm_config.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "value.h"
#include "vm.h"
#include "alloc.h"
#include "static.h"
#include "class.h"
#include "symbol.h"
#include "c_string.h"
#include "console.h"



struct SYM_INDEX {
  uint16_t hash;	//!< hash value, returned by calc_hash().
  const char *pos;	//!< point to the symbol string.
};


static struct SYM_INDEX sym_index[MAX_SYMBOLS_COUNT];
static int sym_index_pos;	// point to the last(free) sym_index array.


//================================================================
/*! search index table
 */
static int search_index( uint16_t hash, const char *str )
{
  int i;

  for( i = 0; i < sym_index_pos; i++ ) {
    if( sym_index[i].hash == hash && strcmp(str, sym_index[i].pos) == 0 ) {
      return i;
    }
  }

  return -1;
}


//================================================================
/*! constructor

  @param  vm	pointer to VM.
  @param  str	String
  @return 	symbol object
*/
mrb_value mrbc_symbol_new(struct VM *vm, const char *str)
{
  mrb_value ret = {.tt = MRB_TT_SYMBOL};
  uint16_t h = calc_hash(str);
  mrb_sym sym_id = search_index(h, str);

  if( sym_id >= 0 ) {
    ret.i = sym_id;
    return ret;		// already exist.
  }

  // check overflow.
  if( sym_index_pos >= MAX_SYMBOLS_COUNT ) {
    console_printf( "Overflow %s for '%s'\n", "MAX_SYMBOLS_COUNT", str );
    return ret;
  }

  // create symbol object dynamically.
  int size = strlen(str) + 1;
  char *buf = mrbc_raw_alloc(size);
  if( buf == NULL ) return ret;		// ENOMEM raise?

  memcpy(buf, str, size);

  sym_index[sym_index_pos].hash = h;
  sym_index[sym_index_pos].pos = buf;
  ret.i = sym_index_pos++;

  return ret;
}


//================================================================
/*! Calculate hash value.

  @param  str		Target string.
  @return uint16_t	Hash value.
*/
uint16_t calc_hash(const char *str)
{
  uint16_t h = 0;

  while( *str != '\0' ) {
    h = h * 37 + *str;
    str++;
  }
  return h;
}


//================================================================
/*! Convert string to symbol value.

  @param  str		Target string.
  @return mrb_sym	Symbol value.
*/
mrb_sym str_to_symid(const char *str)
{
  uint16_t h = calc_hash(str);
  mrb_sym sym_id = search_index(h, str);
  if( sym_id >= 0 ) return sym_id;

  // check overflow.
  if( sym_index_pos >= MAX_SYMBOLS_COUNT ) {
    console_printf( "Overflow %s '%s'\n", "MAX_SYMBOLS_COUNT", str );
    return -1;
  }

  // append index table.
  sym_index[sym_index_pos].hash = h;
  sym_index[sym_index_pos].pos = str;

  return sym_index_pos++;
}


//================================================================
/*! Convert symbol value to string.

  @param  mrb_sym	Symbol value.
  @return const char*	String.
  @retval NULL		Invalid sym_id was given.
*/
const char * symid_to_str(mrb_sym sym_id)
{
  if( sym_id < 0 ) return NULL;
  if( sym_id >= sym_index_pos ) return NULL;

  return sym_index[sym_id].pos;
}



#if MRBC_USE_STRING
//================================================================
/*! (method) to_s
*/
static void c_to_s(mrb_vm *vm, mrb_value v[], int argc)
{
  mrb_value value = mrbc_string_new_cstr(vm, symid_to_str(v[0].i));
  SET_RETURN(value);
}
#endif



//================================================================
/*! (method) ===
*/
static void c_equal3(mrb_vm *vm, mrb_value v[], int argc)
{
  if( mrbc_eq(&v[0], &v[1]) ) {
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}



void mrbc_init_class_symbol(struct VM *vm)
{
  mrbc_class_symbol = mrbc_define_class(vm, "Symbol", mrbc_class_object);

#if MRBC_USE_STRING
  mrbc_define_method(vm, mrbc_class_symbol, "to_s", c_to_s);
  mrbc_define_method(vm, mrbc_class_symbol, "id2name", c_to_s);
#endif
  mrbc_define_method(vm, mrbc_class_symbol, "to_sym", c_ineffect);
  mrbc_define_method(vm, mrbc_class_symbol, "===", c_equal3);
}
