/*! @file
  @brief
  mruby/c value definitions

  <pre>
  Copyright (C) 2015-2018 Kyushu Institute of Technology.
  Copyright (C) 2015-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRBC_SRC_VALUE_H_
#define MRBC_SRC_VALUE_H_

#include <stdint.h>
#include "vm_config.h"


#ifdef __cplusplus
extern "C" {
#endif


// mrb types
//typedef float mrb_float;
//typedef int32_t mrb_int;
typedef int16_t mrb_sym;

/* aspec access ? */
#define MRB_ASPEC_REQ(a)          (((a) >> 18) & 0x1f)
#define MRB_ASPEC_OPT(a)          (((a) >> 13) & 0x1f)
#define MRB_ASPEC_REST(a)         (((a) >> 12) & 0x1)
#define MRB_ASPEC_POST(a)         (((a) >> 7) & 0x1f)

// #define GET_TYPE(v) ((v).tt)
#define IS_FIXNUM(v) (((v).tt)==MRB_TT_FIXNUM)


//================================================================
/*!@brief

*/
typedef enum {
  /* internal use */
  MRB_TT_HANDLE = -1,
  /* primitive */
  MRB_TT_EMPTY = 0,
  MRB_TT_TRUE,
  MRB_TT_FALSE,
  MRB_TT_NIL,
  MRB_TT_FIXNUM,
  MRB_TT_FLOAT,
  MRB_TT_SYMBOL,
  /* non-primitive */
  MRB_TT_OBJECT = 20,
  MRB_TT_CLASS,
  MRB_TT_PROC,
  MRB_TT_ARRAY,
  MRB_TT_STRING,
  MRB_TT_RANGE,
  MRB_TT_HASH,

  MRB_TT_USERTOP,

} mrb_vtype;


//================================================================
/*!@brief

*/
typedef struct RClass {
  mrb_sym name;   // class name
  struct RClass *super;    // mrbc_class[super]
  struct RProc *procs;   // mrbc_proc[rprocs], linked list
} mrb_class;



//================================================================
/*!@brief

*/
typedef struct RInstance {
  struct RClass *cls;    // class
  uint8_t data[];
} mrb_instance;


struct MrbcHandleString;
//================================================================
/*!@brief
  mruby/c value object.
*/
typedef struct RObject {
  mrb_vtype tt : 8;
  union {
    int32_t i;             // MRB_TT_FIXNUM
    struct RObject *handle;  // handle to objects
    struct RInstance *instance;  // MRB_TT_OBJECT : link to instance
    //    struct RObject *obj;   // MRB_TT_OBJECT : link to instance
    struct RClass *cls;    // MRB_TT_CLASS : link to class
    struct RProc *proc;    // MRB_TT_PROC : link to proc
    struct RObject *array; // MRB_TT_ARRAY : array of objects
    struct RObject *range; // MRB_TT_RANGE : link to range
    struct RObject *hash;  // MRB_TT_HASH : link to range
    double d;              // MRB_TT_FLOAT : float
    char *str;             // MRB_TT_STRING : C-string (only loader use.)

    struct MrbcHandleString *h_str;
  };
} mrb_object;
typedef struct RObject mrb_value;


struct VM;
typedef void (*mrb_func_t)(struct VM *vm, mrb_value *v, int argc);



//================================================================
/*!@brief

*/
typedef struct RProc {
  struct RProc *next;
  unsigned int c_func:1;   // 0:IREP, 1:C Func
  mrb_sym sym_id;
  union {
    struct IREP *irep;
    mrb_func_t func;
  };
} mrb_proc;


// alloc one object
mrb_object *mrbc_obj_alloc(struct VM *vm, mrb_vtype tt);

// alloc one class
mrb_class *mrbc_class_alloc(struct VM *vm, const char *name, mrb_class *super);


// alloc one RProc
mrb_proc *mrbc_rproc_alloc(struct VM *vm, const char *name);
mrb_proc *mrbc_rproc_alloc_to_class(struct VM *vm, const char *name, mrb_class *cls);

// EQ two objects
int mrbc_eq(mrb_value *v1, mrb_value *v2);

// Other functions
void mrbc_dup(struct VM *vm, mrb_value *v);
void mrbc_release(struct VM *vm, mrb_value *v);

int32_t mrbc_atoi( const char *s, int base );

mrb_value mrbc_instance_new(struct VM *vm, mrb_class *cls, int size);
void mrbc_instance_delete(struct VM *vm, mrb_value *v);



// for C call
#define SET_INT_RETURN(n)	(v[0].tt=MRB_TT_FIXNUM, v[0].i=(n))
#define SET_NIL_RETURN()	(v[0].tt=MRB_TT_NIL)
#define SET_FLOAT_RETURN(n)	(v[0].tt=MRB_TT_FLOAT, v[0].d=(n))
#define SET_FALSE_RETURN()	(v[0].tt=MRB_TT_FALSE)
#define SET_TRUE_RETURN()	(v[0].tt=MRB_TT_TRUE)
#define SET_RETURN(n)		(v[0]=(n))

#define GET_TT_ARG(n)		(v[(n)].tt)
#define GET_INT_ARG(n)		(v[(n)].i)
#define GET_ARY_ARG(n)		(v[(n)])
#define GET_ARG(n)		(v[(n)])
#define GET_FLOAT_ARG(n)	(v[(n)].d)
#define GET_STRING_ARG(n)	(v[(n)].h_str->str)



//================================================================
/*!@brief
  Returns a fixnum in mruby/c.

  @param  n	int value
  @return	mrb_value of type fixnum.
*/
static inline mrb_value mrb_fixnum_value( int32_t n )
{
  mrb_value value = {.tt = MRB_TT_FIXNUM};
  value.i = n;
  return value;
}


//================================================================
/*!@brief
  Returns a float in mruby/c.

  @param  n	dluble value
  @return	mrb_value of type float.
*/
static inline mrb_value mrb_float_value( double n )
{
  mrb_value value = {.tt = MRB_TT_FLOAT};
  value.d = n;
  return value;
}


//================================================================
/*!@brief
  Returns a nil in mruby/c.

  @return	mrb_value of type nil.
*/
static inline mrb_value mrb_nil_value(void)
{
  mrb_value value = {.tt = MRB_TT_NIL};
  return value;
}


//================================================================
/*!@brief
  Returns a true in mruby/c.

  @return	mrb_value of type true.
*/
static inline mrb_value mrb_true_value(void)
{
  mrb_value value = {.tt = MRB_TT_TRUE};
  return value;
}


//================================================================
/*!@brief
  Returns a false in mruby/c.

  @return	mrb_value of type false.
*/
static inline mrb_value mrb_false_value(void)
{
  mrb_value value = {.tt = MRB_TT_FALSE};
  return value;
}


#ifdef __cplusplus
}
#endif
#endif
