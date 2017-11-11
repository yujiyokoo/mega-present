/*! @file
  @brief
  mruby bytecode executor.

  <pre>
  Copyright (C) 2015-2017 Kyushu Institute of Technology.
  Copyright (C) 2015-2017 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Fetch mruby VM bytecodes, decode and execute.

  </pre>
*/

#ifndef MRBC_SRC_VM_H_
#define MRBC_SRC_VM_H_

#include <stdint.h>
#include "value.h"
#include "vm_config.h"

#ifdef __cplusplus
extern "C" {
#endif


//================================================================
/*!@brief
  IREP Internal REPresentation
*/
typedef struct IREP {
  uint16_t nlocals;		//!< # of local variables
  uint16_t nregs;		//!< # of register variables
  uint16_t rlen;		//!< # of child IREP blocks
  uint16_t ilen;		//!< # of irep
  uint16_t plen;		//!< # of pool

  uint8_t     *code;		//!< ISEQ (code) BLOCK
  mrb_object  **pools;          //!< array of POOL objects pointer.
  uint8_t     *ptr_to_sym;
  struct IREP **reps;		//!< array of child IREP's pointer.

} mrb_irep;


//================================================================
/*!@brief
  Call information
*/
typedef struct CALLINFO {
  mrb_irep *pc_irep;
  uint16_t  pc;
  uint16_t  reg_top;
  uint8_t   n_args;     // num of args
} mrb_callinfo;


//================================================================
/*!@brief
  Virtual Machine
*/
typedef struct VM {
  mrb_irep *irep;

  uint8_t        vm_id; // vm_id : 1..n
  const uint8_t *mrb;   // bytecode

  mrb_irep *pc_irep;    // PC
  uint16_t  pc;         // PC

  uint16_t     reg_top;
  mrb_value    regs[MAX_REGS_SIZE];
  uint16_t     callinfo_top;
  mrb_callinfo callinfo[MAX_CALLINFO_SIZE];

  mrb_class  *target_class;
  mrb_object *top_self; // ?

  int32_t error_code;

  volatile int8_t flag_preemption;
  int8_t flag_need_memfree;
} mrb_vm;


mrb_irep *new_irep(mrb_vm *vm);
mrb_vm *mrbc_vm_open(mrb_vm *vm);
void mrbc_vm_close(mrb_vm *vm);
void mrbc_vm_begin(mrb_vm *vm);
void mrbc_vm_end(mrb_vm *vm);
int mrbc_vm_run(mrb_vm *vm);


//================================================================
/*!@brief
  Get 32bit value from memory big endian.

  @param  s	Pointer of memory.
  @return	32bit unsigned value.
*/
inline static uint32_t bin_to_uint32( const void *s )
{
  uint32_t x = *((uint32_t *)s);
  return (x << 24) | ((x & 0xff00) << 8) | ((x >> 8) & 0xff00) | (x >> 24);
}


//================================================================
/*!@brief
  Get 16bit value from memory big endian.

  @param  s	Pointer of memory.
  @return	16bit unsigned value.
*/
inline static uint16_t bin_to_uint16( const void *s )
{
  uint16_t x = *((uint16_t *)s);
  return (x << 8) | (x >> 8);
}


#ifdef __cplusplus
}
#endif
#endif
