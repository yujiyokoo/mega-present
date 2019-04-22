/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRBC_SRC_OPCODE_H_
#define MRBC_SRC_OPCODE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define PEEK_B(pc) ((pc)[0])
#define PEEK_S(pc) ((pc)[0]<<8|(pc)[1])
#define PEEK_W(pc) ((pc)[0]<<16|(pc)[1]<<8|(pc)[2])

#define READ_B() (vm->inst+=1, PEEK_B(vm->inst-1))
#define READ_S() (vm->inst+=2, PEEK_S(vm->inst-2))
#define READ_W() (vm->inst+=3, PEEK_W(vm->inst-3))

#define EXT_CLEAR() vm->ext_flag = 0
#define FETCH_Z() EXT_CLEAR()
#define FETCH_B() uint32_t a = (vm->ext_flag & 1) ? READ_S() : READ_B(); EXT_CLEAR()
#define FETCH_BB() uint32_t a,b; a = (vm->ext_flag & 1) ? READ_S() : READ_B(); b = (vm->ext_flag & 2)? READ_S() : READ_B(); EXT_CLEAR()
#define FETCH_BBB() uint32_t a,b,c; a = (vm->ext_flag & 1) ? READ_S() : READ_B(); b = (vm->ext_flag & 2)? READ_S() : READ_B(); c=READ_B(); EXT_CLEAR()
#define FETCH_BS() uint32_t a,b; a = (vm->ext_flag & 1) ? READ_S() : READ_B(); b=READ_S(); EXT_CLEAR()
#define FETCH_S() uint32_t a=READ_S(); EXT_CLEAR()
#define FETCH_W() uint32_t a=READ_W(); EXT_CLEAR()

  
//================================================================
/*!@brief

*/
enum OPCODE {
  OP_NOP       = 0x00,
  OP_MOVE      = 0x01,
  OP_LOADL     = 0x02,
  OP_LOADI     = 0x03,
  OP_LOADNEG   = 0x04,
  OP_LOADI__1  = 0x05,
  OP_LOADI_0   = 0x06,
  OP_LOADI_1   = 0x07,
  OP_LOADI_2   = 0x08,
  OP_LOADI_3   = 0x09,
  OP_LOADI_4   = 0x0a,
  OP_LOADI_5   = 0x0b,
  OP_LOADI_6   = 0x0c,
  OP_LOADI_7   = 0x0d,
  OP_LOADSYM   = 0x0e,
  OP_LOADNIL   = 0x0f,
  OP_LOADSELF  = 0x10,
  OP_LOADT     = 0x11,
  OP_LOADF     = 0x12,
  OP_GETGV     = 0x13,
  OP_SETGV     = 0x14,

  OP_GETCONST  = 0x1b,
  OP_SETCONST  = 0x1c,

  OP_JMP       = 0x21,
  OP_JMPIF     = 0x22,
  OP_JMPNOT    = 0x23,
  OP_JMPNIL    = 0x24,

  OP_SENDV     = 0x2c,

  OP_SEND      = 0x2e,
  OP_SENDB     = 0x2f,

  OP_ENTER     = 0x33,

  OP_RETURN    = 0x37,

  OP_BLKPUSH   = 0x3a,
  OP_ADD       = 0x3b,
  OP_ADDI      = 0x3c,
  OP_SUB       = 0x3d,
  OP_SUBI      = 0x3e,
  OP_MUL       = 0x3f,
  OP_DIV       = 0x40,
  OP_EQ        = 0x41,
  OP_LT        = 0x42,
  OP_LE        = 0x43,
  OP_GT        = 0x44,
  OP_GE        = 0x45,
  OP_ARRAY     = 0x46,
  OP_ARRAY2    = 0x47,
  OP_ARYCAT    = 0x48,

  OP_ARYDUP    = 0x4a,
  OP_AREF      = 0x4b,

  OP_APOST     = 0x4d,

  OP_STRING    = 0x4f,
  OP_STRCAT    = 0x50,
  OP_HASH      = 0x51,

  OP_BLOCK     = 0x55,
  OP_METHOD    = 0x56,

  OP_CLASS     = 0x5a,

  OP_EXEC      = 0x5c,
  OP_DEF       = 0x5d,

  OP_TCLASS    = 0x61,

  OP_EXT1      = 0x64,
  OP_EXT2      = 0x65,
  OP_EXT3      = 0x66,

  OP_STOP      = 0x67,
  OP_ABORT     = 0xff,
};

//================================================================
/*!@brief
  OP_RETURN parameter

*/
#define OP_R_NORMAL 0
#define OP_R_BREAK  1
#define OP_R_RETURN 2


#if defined(MRBC_LITTLE_ENDIAN)
#define MKOPCODE(op) (((uint32_t)(op) & 0x7f)<<24)
#define MKARG_A(x)   (((uint32_t)(x) & 0xff)<<1 | ((uint32_t)(x) & 0x01)>>8)
#define MKARG_B(x)   (((uint32_t)(x) & 0x1fc)<<6 | ((uint32_t)(x) & 0x03)<<22)
#define MKARG_C(x)   (((uint32_t)(x) & 0x7e)<<15 | ((uint32_t)(x) & 0x01)<<31)

#elif defined(MRBC_BIG_ENDIAN)
#define MKOPCODE(op) ((uint32_t)(op) & 0x7f)
#define MKARG_A(x)   ((uint32_t)(x) << 23)
#define MKARG_B(x)   ((uint32_t)(x) << 14)
#define MKARG_C(x)   ((uint32_t)(x) << 7)
#endif

  
#ifdef __cplusplus
}
#endif
#endif
