/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_VALUE_H_
#define MRUBYC_SRC_VALUE_H_

#include <stdint.h>
#include "vm_config.h"

#ifdef __cplusplus
extern "C" {
#endif


// mrb types
//typedef float mrb_float;
typedef int32_t mrb_int;
typedef int32_t mrb_sym;

/* aspec access ? */
#define MRB_ASPEC_REQ(a)          (((a) >> 18) & 0x1f)
#define MRB_ASPEC_OPT(a)          (((a) >> 13) & 0x1f)
#define MRB_ASPEC_REST(a)         (((a) >> 12) & 0x1)
#define MRB_ASPEC_POST(a)         (((a) >> 7) & 0x1f)

// #define GET_TYPE(v) ((v).tt)
#define IS_FIXNUM(v) (((v).tt)==MRB_TT_FIXNUM)

#pragma pack(2)

//================================================================
/*!@brief

*/
typedef enum {
  /* primitive */
  MRB_TT_EMPTY = 0,
  MRB_TT_TRUE,
  MRB_TT_FALSE,
  MRB_TT_NIL,
  MRB_TT_FIXNUM,
  MRB_TT_FLOAT,
  /* non-primitive */
  MRB_TT_OBJECT = 64,
  MRB_TT_CLASS,
  MRB_TT_PROC,
  MRB_TT_ARRAY,
  MRB_TT_STRING,
} mrb_vtype;


//================================================================
/*!@brief

*/
typedef struct RClass {
  struct RClass *next;  // linked list
  mrb_vtype tt:8;
  mrb_sym name;   // class name
  struct RClass *super;    // static_class[super]
  struct RProc *procs;   // static_proc[rprocs], linked list
} mrb_class;



//================================================================
/*!@brief

*/
typedef struct RObject {
  struct RObject *next;
  mrb_vtype tt;
  union {
    int i;        // MRB_TT_FIXNUM
    struct RObject *obj;   // MRB_TT_OBJECT : link to object
    struct RClass *cls;    // MRB_TT_CLASS : link to class
    struct RProc *proc;    // MRB_TT_PROC : link to proc
    struct RArray *array;  // MRB_TT_ARRAY : array object
#if MRUBYC_USE_FLOAT
    double d;              // MRB_TT_FLOAT : float
#endif
#if MRUBYC_USE_STRING
    char *str;             // MRB_TT_STRING : C-string
#endif    
  } value;
} mrb_object;
typedef struct RObject mrb_value;


struct VM;
typedef void (*mrb_func_t)(struct VM *vm, mrb_value *v);


//================================================================
/*!@brief

*/ 
struct RArray {
  int len;
  struct RObject data[0];
};



//================================================================
/*!@brief

*/
typedef struct RProc {
  struct RProc *next;
  unsigned int c_func:1;   // 0:IREP, 1:C Func
  int16_t sym_id;
  union {
    struct IREP *irep;
    mrb_func_t func;
  } func;
} mrb_proc;


// alloc one object
mrb_object *mrb_obj_alloc(mrb_vtype tt);

// alloc one class
mrb_class *mrb_class_alloc(const char *name, mrb_class *super);


// alloc one RProc
mrb_proc *mrb_rproc_alloc(const char *name);
mrb_proc *mrb_rproc_alloc_to_class(const char *name, mrb_class *cls);


// for C call
#define SET_INT_RETURN(n)         {v[0].tt=MRB_TT_FIXNUM;v[0].value.i=(n);}
#define SET_NIL_RETURN()          v[0].tt=MRB_TT_FALSE
#define SET_FALSE_RETURN()        v[0].tt=MRB_TT_FALSE
#define SET_TRUE_RETURN()         v[0].tt=MRB_TT_TRUE
#define SET_RETURN(n)             v[0]=n

#define GET_TT_ARG(n)             v[(n)+1].tt
#define GET_INT_ARG(n)            v[(n)+1].value.i
#define GET_ARY_ARG(n)            v[(n)+1]

#if MRUBYC_USE_FLOAT
#define GET_FLOAT_ARG(n)          v[(n)+1].value.d
#endif

#if MRUBYC_USE_STRING
#define GET_STRING_ARG(n)          v[(n)+1].value.str
#endif

#ifdef __cplusplus
}
#endif
#endif
