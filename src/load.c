/*! @file
  @brief
  mruby bytecode loader.

  <pre>
  Copyright (C) 2015-2021 Kyushu Institute of Technology.
  Copyright (C) 2015-2021 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include "vm_config.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "vm.h"
#include "load.h"
#include "value.h"
#include "alloc.h"
#include "console.h"

//
// This is a dummy code for raise
//
#define mrbc_raise(vm,err,msg) console_printf("<raise> %s:%d\n", __FILE__, __LINE__);


// IREP TT
enum irep_pool_type {
  IREP_TT_STR   = 0,	// string (need free)
  IREP_TT_SSTR  = 2,	// string (static)
  IREP_TT_INT32 = 1,	// 32bit integer
  IREP_TT_INT64 = 3,	// 64bit integer
  IREP_TT_FLOAT = 5,	// float (double/float)
};



//================================================================
/*! Parse header section.

  @param  vm    A pointer to VM.
  @param  bin	A pointer to RITE header.
  @return int	zero if no error.

  <pre>
  Structure
   "RITE"     identifier
   "01"       major version
   "00"       minor version
   0000_0000  total size
   "MATZ"     compiler name
   "0000"     compiler version
  </pre>
*/
static int load_header(struct VM *vm, const uint8_t *bin)
{
  static const char IDENT[6] = "RITE02";

  if( memcmp(bin, IDENT, sizeof(IDENT)) != 0 ) {
    mrbc_raise(vm, E_BYTECODE_ERROR, NULL);
    return -1;
  }

  /* Ignore others. */

  return 0;
}


//================================================================
/*! read one irep section.

  @param  vm	A pointer to VM.
  @param  bin	A pointer to RITE ISEQ.
  @param  len	Returns the parsed length.
  @return	Pointer to allocated mrbc_irep or NULL

  <pre>
   (loop n of child irep bellow)
   0000_0000	record size
   0000		n of local variable
   0000		n of register
   0000		n of child irep
   0000		n of catch handler
   0000		n of byte code  (ISEQ BLOCK)
   ...		byte codes

   0000		n of pool	(POOL BLOCK)
   (loop n of pool)
     00		type
     ...	pool data

   0000		n of symbol	(SYMS BLOCK)
   (loop n of symbol)
     0000	length
     ...	symbol data
  </pre>
*/
static mrbc_irep * load_irep_1(struct VM *vm, const uint8_t *bin, int *len)
{
  const uint8_t *p = bin + 4;

  // new irep
  mrbc_irep *irep = mrbc_irep_alloc(0);
  if( irep == NULL ) {
    mrbc_raise(vm, E_BYTECODE_ERROR, NULL);
    return NULL;
  }
  irep->nlocals = bin_to_uint16(p);	p += 2;
  irep->nregs = bin_to_uint16(p);	p += 2;
  irep->rlen = bin_to_uint16(p);	p += 2;
  irep->clen = bin_to_uint16(p);	p += 2;
  irep->ilen = bin_to_uint16(p);	p += 2;

  // allocate memory for child irep's pointers
  if( irep->rlen ) {
    irep->reps = (mrbc_irep **)mrbc_alloc(0, sizeof(mrbc_irep *) * irep->rlen);
    if( irep->reps == NULL ) {
      mrbc_raise(vm, E_BYTECODE_ERROR, NULL);
      return NULL;
    }
  }

  // ISEQ (code) BLOCK
  irep->code = (uint8_t *)p;
  p += irep->ilen + sizeof(mrbc_irep_catch_handler) * irep->clen;
  assert( sizeof(mrbc_irep_catch_handler) == 13 );

  // POOL BLOCK
  irep->plen = bin_to_uint16(p);	p += 2;
  if( irep->plen ) {
    irep->pools = (mrbc_object**)mrbc_alloc(0, sizeof(void*) * irep->plen);
    if(irep->pools == NULL ) {
      mrbc_raise(vm, E_BYTECODE_ERROR, NULL);
      return NULL;
    }
  }

  int i;
  for( i = 0; i < irep->plen; i++ ) {
    int tt = *p++;
    mrbc_object *obj = mrbc_alloc(0, sizeof(mrbc_object));
    if( obj == NULL ) {
      mrbc_raise(vm, E_BYTECODE_ERROR, NULL);
      return NULL;
    }
    switch( tt ) {
#if MRBC_USE_STRING
    case IREP_TT_STR:
    case IREP_TT_SSTR: {
      int pool_data_len = bin_to_uint16(p);
      p += sizeof(uint16_t);
      obj->tt = MRBC_TT_STRING;
      obj->str = (char*)p;
      p += pool_data_len + 1;
    } break;
#endif
    case IREP_TT_INT32: {
      int32_t value = bin_to_uint32(p);
      p += sizeof(int32_t);
      obj->tt = MRBC_TT_FIXNUM;
      obj->i = value;
    } break;
#if MRBC_USE_FLOAT
    case IREP_TT_FLOAT: {
      double value;
      memcpy(&value, p, sizeof(double));
      p += sizeof(double);
      obj->tt = MRBC_TT_FLOAT;
      obj->d = value;
    } break;
#endif
    case IREP_TT_INT64: {
#ifdef MRBC_INT64
      uint64_t value = bin_to_uint32(p);
      p += sizeof(uint32_t);
      value <<= 32;
      value |= bin_to_uint32(p);
      p += sizeof(uint32_t);
      obj->tt = MRBC_TT_FIXNUM;
      obj->i = value;
#else
      p += sizeof(uint32_t) * 2;
      mrbc_raise(vm, E_BYTECODE_ERROR, NULL);
#endif
    } break;
    default:
      assert(!"Unknown tt");
    }

    irep->pools[i] = obj;
  }

  // SYMS BLOCK
  irep->ptr_to_sym = (uint8_t*)p;

  *len = bin_to_uint32(bin);

#if defined(MRBC_DEBUG)
  int slen = bin_to_uint16(p);		p += 2;
  while( --slen >= 0 ) {
    int s = bin_to_uint16(p);		p += 2;
    p += s+1;
  }
  assert( *len == (p - bin) );
#endif

  return irep;
}


//================================================================
/*! Load IREP section.

  @param  vm	A pointer to VM.
  @param  bin	A pointer to RITE ISEQ.
  @param  len	Returns the parsed length.
  @return	Pointer to allocated mrbc_irep or NULL
*/
static mrbc_irep *load_irep(struct VM *vm, const uint8_t *bin, int *len)
{
  int len1;
  mrbc_irep *irep = load_irep_1(vm, bin, &len1);
  if( !irep ) return NULL;
  int total_len = len1;

  int i;
  for( i = 0; i < irep->rlen; i++ ) {
    irep->reps[i] = load_irep(vm, bin + total_len, &len1);
    if( ! irep->reps[i] ) return NULL;
    total_len += len1;
  }

  if( len ) *len = total_len;
  return irep;
}


//================================================================
/*! Load the VM bytecode.

  @param  vm    Pointer to VM.
  @param  bin	Pointer to bytecode.
  @return int	zero if no error.
*/
int mrbc_load_mrb(struct VM *vm, const uint8_t *bin)
{
  static const int SIZE_RITE_BINARY_HEADER = 20;
  static const int SIZE_RITE_SECTION_HEADER = 12;
  static const char IREP[4] = "IREP";
  static const char END[4] = "END\0";

  vm->mrb = bin;
  if( load_header(vm, bin) != 0 ) return -1;
  bin += SIZE_RITE_BINARY_HEADER;

  while( 1 ) {
    uint32_t section_size = bin_to_uint32(bin+4);

    if( memcmp(bin, IREP, sizeof(IREP)) == 0 ) {
      vm->irep = load_irep(vm, bin + SIZE_RITE_SECTION_HEADER, 0);
      if( vm->irep == NULL ) return -1;

    } else if( memcmp(bin, END, sizeof(END)) == 0 ) {
      break;
    }

    bin += section_size;
  }

  return 0;
}
