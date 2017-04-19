/*! @file
  @brief
  mruby bytecode loader.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "vm.h"
#include "vm_config.h"
#include "load.h"
#include "errorcode.h"
#include "static.h"
#include "value.h"


//================================================================
/*!@brief
  Parse header section.

  @param  vm    A pointer of VM.
  @param  pos	A pointer of pointer of RITE header.
  @return int	zero if no error.

  <pre>
  Structure
   "RITE"	identifier
   "0003"	version
   0000		CRC
   0000_0000	total size
   "MATZ"	compiler name
   "0000"	compiler version
  </pre>
*/
static int load_header(struct VM *vm, const uint8_t **pos)
{
  const uint8_t *p = *pos;

  if( memcmp(p + 4, "0003", 4) != 0 ) {
    vm->error_code = LOAD_FILE_HEADER_ERROR_VERSION;
    return -1;
  }

  /* Ignore CRC */

  /* Ignore size */

  if( memcmp(p + 14, "MATZ", 4) != 0 ) {
    vm->error_code = LOAD_FILE_HEADER_ERROR_MATZ;
    return -1;
  }
  if( memcmp(p + 18, "0000", 4) != 0 ) {
    vm->error_code = LOAD_FILE_HEADER_ERROR_VERSION;
    return -1;
  }

  *pos += 22;
  return 0;
}


//================================================================
/*!@brief
  Parse IREP section.

  @param  vm    A pointer of VM.
  @param  pos	A pointer of pointer of IREP section.
  @return int	zero if no error.

  <pre>
  Structure
   "IREP"	section identifier
   0000_0000	section size
   "0000"	rite version

   (loop n of child irep bellow)
   0000_0000	record size
   0000		n of local variable
   0000		n of register
   0000		n of child irep

   0000_0000	n of byte code  (ISEQ BLOCK)
   ...		byte codes

   0000_0000	n of pool	(POOL BLOCK)
   (loop n of pool)
     00		type
     0000	length
     ...	pool data

   0000_0000	n of symbol	(SYMS BLOCK)
   (loop n of symbol)
     0000	length
     ...	symbol data
  </pre>
*/
static int load_irep(struct VM *vm, const uint8_t **pos)
{
  const uint8_t *p = *pos;
  p += 4;
  int section_size = bin_to_uint32(p);
  p += 4;

  if( memcmp(p, "0000", 4) != 0 ) {
    vm->error_code = LOAD_FILE_IREP_ERROR_VERSION;
    return -1;
  }
  p += 4;

  int cnt = 0;
  while( cnt < section_size ) {
    cnt += bin_to_uint32(p) + 8;
    p += 4;

    // new irep
    mrb_irep *irep = new_irep(0);
    if( irep == 0 ) {
      vm->error_code = LOAD_FILE_IREP_ERROR_ALLOCATION;
      return -1;
    }

    // add irep into vm->irep (at tail)
    // TODO: Optimize this process
    if( vm->irep == 0 ) {
      vm->irep = irep;
    } else {
      mrb_irep *p = vm->irep;
      while( p->next != 0 ) {
        p = p->next;
      }
      p->next = irep;
    }
    irep->next = 0;

    // nlocals,nregs,rlen
    irep->nlocals = bin_to_uint16(p);  p += 2;
    irep->nregs = bin_to_uint16(p);    p += 2;
    irep->rlen = bin_to_uint16(p);     p += 2;
    irep->ilen = bin_to_uint32(p);     p += 4;

    // padding
    p += (-(p - *pos + 2) & 0x03);  // +2 = (RITE(22) + IREP(12)) & 0x03

    // ISEQ (code) BLOCK
    irep->code = (uint8_t *)p;
    p += irep->ilen * 4;

    // POOL BLOCK
    irep->ptr_to_pool = 0;
    int plen = bin_to_uint32(p);    p += 4;
    int i;
    for( i=0 ; i<plen ; i++ ){
      int tt = *p++;
      int obj_size = bin_to_uint16(p);   p += 2;
      mrb_object *ptr = mrbc_obj_alloc(0, MRB_TT_FALSE);
      if( ptr == 0 ){
        vm->error_code = LOAD_FILE_IREP_ERROR_ALLOCATION;
	return -1;
      }
      switch( tt ){
#if MRBC_USE_STRING
        case 0: { // IREP_TT_STRING
          ptr->tt = MRB_TT_STRING;
	  ptr->value.str = (char*)p;
        } break;
#endif
        case 1: { // IREP_TT_FIXNUM
          char buf[obj_size+1];
          memcpy(buf, p, obj_size);
          buf[obj_size] = '\0';
          ptr->tt = MRB_TT_FIXNUM;
          ptr->value.i = atoi(buf);
        } break;
#if MRBC_USE_FLOAT
        case 2: { // IREP_TT_FLOAT
          char buf[obj_size+1];
          memcpy(buf, p, obj_size);
          buf[obj_size] = '\0';
          ptr->tt = MRB_TT_FLOAT;
          ptr->value.d = atof(buf);
        } break;
#endif
        default:
          break;
      }
      if( irep->ptr_to_pool == 0 ){
        irep->ptr_to_pool = ptr;
      } else {
        mrb_object *pp = irep->ptr_to_pool;
        while( pp->next != 0 ) pp = pp->next;
        pp->next = ptr;
      }
      p += obj_size;
    }

    // SYMS BLOCK
    irep->ptr_to_sym = (uint8_t*)p;
    int slen = bin_to_uint32(p);    p += 4;
    for( i=0 ; i<slen ; i++ ){
      int s = bin_to_uint16(p);     p += 2;
      p += s+1;
    }
  }

  /* TODO: size */
  *pos += section_size;
  return 0;
}


//================================================================
/*!@brief
  Parse LVAR section.

  @param  vm    A pointer of VM.
  @param  pos	A pointer of pointer of LVAR section.
  @return int	zero if no error.
*/
static int load_lvar(struct VM *vm, const uint8_t **pos)
{
  const uint8_t *p = *pos;

  /* size */
  *pos += bin_to_uint32(p+4);

  return 0;
}


//================================================================
/*!@brief
  Load the VM bytecode.

  @param  vm    Pointer to VM.
  @param  ptr	Pointer to bytecode.

*/
int mrbc_load_mrb(mrb_vm *vm, const uint8_t *ptr)
{
  int ret = -1;
  vm->mrb = ptr;

  if( memcmp(ptr, "RITE", 4) == 0 ) {
    ret = load_header(vm, &ptr);
  }
  while( ret == 0 ) {
    if( memcmp(ptr, "IREP", 4) == 0 ) {
      ret = load_irep(vm, &ptr);
    }
    else if( memcmp(ptr, "LVAR", 4) == 0 ) {
      ret = load_lvar(vm, &ptr);
    }
    else if( memcmp(ptr, "END\0", 4) == 0 ) {
      break;
    }
  }

  return ret;
}
