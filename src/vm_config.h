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
#define MAX_VM_COUNT 5

/* max of IREP block in mrb */
#define MAX_IREP_SIZE 500

/* maximum number of ireps */
#define MAX_IREP_COUNT 50

/* maximum size of registers */
#define MAX_REGS_SIZE 100

/* maximum size of callinfo (callstack) */
#define MAX_CALLINFO_SIZE 100

/* maximum number of objects */
#define MAX_OBJECT_COUNT 400

/* maximum number of classes */
#define MAX_CLASS_COUNT 20

/* maximum number of procs */
#define MAX_PROC_COUNT 50

/* maximum size of symbol table */
#define MAX_SYMBOLS_SIZE 200

/* maximum number of symbols */
#define MAX_SYMBOLS_COUNT 200


/* maximum size of global objects */
#define MAX_GLOBAL_OBJECT_SIZE 20




/* Configure environment */
/* 0: NOT USE */
/* 1: USE */

/* USE FileIO, fopen, fread, ... */
#define MRUBYC_USE_FILEIO 1

/* USE Float. Support Float class */
#define MRUBYC_USE_FLOAT 1

#endif
