/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_OPCODE_H_
#define MRUBYC_SRC_OPCODE_H_

#ifdef __cplusplus
extern "C" {
#endif


#define GET_OPCODE(code)            ((code) & 0x7f)
#define GETARG_A(code)              (((code) >> 23) & 0x1ff)
#define GETARG_B(code)              (((code) >> 14) & 0x1ff)
#define GETARG_C(code)              (((code) >>  7) & 0x7f)
#define GETARG_Ax(code)             (((code) >>  7) & 0x1ffffff)

#define GETARG_b(code)              GETARG_UNPACK_b(code,14,2)

#define GETARG_UNPACK_b(i,n1,n2)    ((((code)) >> (7+(n2))) & (((1<<(n1))-1)))


#define MAXARG_Bx                   (0xffff)
#define MAXARG_sBx                  (MAXARG_Bx>>1)
#define GETARG_Bx(code)             (((code) >>  7) & 0xffff)
#define GETARG_sBx(code)            (GETARG_Bx(code)-MAXARG_sBx)
#define GETARG_C(code)              (((code) >>  7) & 0x7f)


//================================================================
/*!@brief

*/
enum OPCODE {
  OP_NOP = 0x00,
  OP_MOVE = 0x01,
  OP_LOADL = 0x02,
  OP_LOADI = 0x03,

  OP_LOADNIL = 0x05,
  OP_LOADSELF = 0x06,
  OP_LOADT = 0x07,
  OP_LOADF = 0x08,
  OP_GETGLOBAL = 0x09,
  OP_SETGLOBAL = 0x0a,

  OP_GETCONST = 0x11,
  OP_SETCONST = 0x12,

  OP_JMP = 0x17,
  OP_JMPIF = 0x18,
  OP_JMPNOT = 0x19,
  OP_SEND = 0x20,

  OP_ENTER = 0x26,

  OP_RETURN = 0x29,

  OP_ADD = 0x2c,
  OP_ADDI = 0x2d,
  OP_SUB = 0x2e,
  OP_SUBI = 0x2f,

  OP_EQ = 0x32,
  OP_LT = 0x33,
  OP_LE = 0x34,
  OP_GT = 0x35,
  OP_GE = 0x36,
  OP_ARRAY = 0x37,

  OP_LAMBDA = 0x40,

  OP_CLASS = 0x43,

  OP_METHOD = 0x46,

  OP_TCLASS = 0x48,

  OP_STOP = 0x4a,
};


#ifdef __cplusplus
}
#endif
#endif
