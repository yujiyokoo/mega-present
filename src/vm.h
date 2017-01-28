/*! @file
  @brief
  mruby bytecode executor.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

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

*/
typedef struct IREP {
  int16_t unused;       //! unused flag
  struct IREP *next;         //! irep linked list

  uint8_t *code;
  mrb_object *ptr_to_pool;
  uint8_t *ptr_to_sym;

  int16_t nlocals;
  int16_t nregs;
  int16_t rlen;
  int32_t ilen;

  int16_t iseq;

} mrb_irep;


//================================================================
/*!@brief

*/
typedef struct CALLINFO {
  mrb_irep *pc_irep;
  uint32_t pc;
  uint32_t reg_top;
  uint8_t n_args;   // num of args
} mrb_callinfo;


//================================================================
/*!@brief

*/
typedef struct VM {
  mrb_irep *irep;    // irep linked list

  uint8_t vm_id;     // vm_id : 1..n
  int16_t priority;  //
  const uint8_t *mrb;      // bytecode

  mrb_irep *pc_irep;  // PC
  int16_t pc;       // PC

  int reg_top;
  mrb_value regs[MAX_REGS_SIZE];
  int callinfo_top;
  mrb_callinfo callinfo[MAX_CALLINFO_SIZE];

  mrb_class *target_class;
  mrb_object *top_self;  // ?

  int32_t error_code;

  volatile int8_t flag_preemption;
} mrb_vm;



mrb_irep *new_irep(mrb_vm *vm);
struct VM *vm_open(void);
void vm_close(struct VM *vm);
void vm_boot(struct VM *vm);
int vm_run(struct VM *vm);


//================================================================
/*!@brief
  Get 32bit value from memory big endian.

  @param  s	Pointer of memory.
  @return	32bit unsigned value.
*/
inline static uint32_t bin_to_uint32( const void *s )
{
  uint8_t *s1 = (uint8_t *)s;
  uint32_t ret;

  ret = *s1++;
  ret = (ret << 8) + *s1++;
  ret = (ret << 8) + *s1++;
  ret = (ret << 8) + *s1;
  return ret;
}


//================================================================
/*!@brief
  Get 16bit value from memory big endian.

  @param  s	Pointer of memory.
  @return	16bit unsigned value.
*/
inline static uint16_t bin_to_uint16( const void *s )
{
  uint8_t *s1 = (uint8_t *)s;
  uint16_t ret;

  ret = *s1++;
  ret = (ret << 8) + *s1;
  return ret;
}


#ifdef __cplusplus
}
#endif
#endif
