/*! @file
  @brief
  Global configration of mruby/c VM's

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_VM_CONFIG_H_
#define MRUBYC_SRC_VM_CONFIG_H_

/* maximum number of VMs */
#ifndef MAX_VM_COUNT
#define MAX_VM_COUNT 5
#endif

/* maximum size of registers */
#ifndef MAX_REGS_SIZE
#define MAX_REGS_SIZE 100
#endif

/* maximum size of callinfo (callstack) */
#ifndef MAX_CALLINFO_SIZE
#define MAX_CALLINFO_SIZE 100
#endif

/* maximum number of objects */
#ifndef MAX_OBJECT_COUNT
#define MAX_OBJECT_COUNT 400
#endif

/* maximum number of classes */
#ifndef MAX_CLASS_COUNT
#define MAX_CLASS_COUNT 20
#endif

/* maximum size of symbol table */
#ifndef MAX_SYMBOLS_SIZE
#define MAX_SYMBOLS_SIZE 400
#endif

/* maximum number of symbols */
#ifndef MAX_SYMBOLS_COUNT
#define MAX_SYMBOLS_COUNT 200
#endif

/* maximum size of global objects */
#ifndef MAX_GLOBAL_OBJECT_SIZE
#define MAX_GLOBAL_OBJECT_SIZE 20
#endif

/* maximum size of consts */
#ifndef MAX_CONST_COUNT
#define MAX_CONST_COUNT 20
#endif


/* Configure environment */
/* 0: NOT USE */
/* 1: USE */

/* USE Float. Support Float class */
#define MRUBYC_USE_FLOAT 1

/* USE String. Support String class */
#define MRUBYC_USE_STRING 1

#endif
