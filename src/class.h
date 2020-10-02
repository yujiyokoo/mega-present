/*! @file
  @brief
  mruby/c Object, Proc, Nil, False and True class and class specific functions.

  <pre>
  Copyright (C) 2015-2020 Kyushu Institute of Technology.
  Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRBC_SRC_CLASS_H_
#define MRBC_SRC_CLASS_H_

#ifdef __cplusplus
extern "C" {
#endif

/***** Feature test switches ************************************************/
#include "vm_config.h"

/***** System headers *******************************************************/
/***** Local headers ********************************************************/
#include "value.h"
#include "keyvalue.h"


/***** Constant values ******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
//================================================================
/*! mruby/c class object.
*/
typedef struct RClass {
  mrbc_sym sym_id;	// class name
#ifdef MRBC_DEBUG
  const char *names;	// for debug. delete soon.
#endif
  struct RClass *super;	// mrbc_class[super]
  struct RMethod *method_link;

} mrbc_class;
typedef struct RClass mrb_class;


//================================================================
/*! mruby/c instance object.
*/
typedef struct RInstance {
  MRBC_OBJECT_HEADER;

  struct RClass *cls;
  struct RKeyValueHandle ivar;
  uint8_t data[];

} mrbc_instance;
typedef struct RInstance mrb_instance;


//================================================================
/*! mruby/c proc object.
*/
typedef struct RProc {
  MRBC_OBJECT_HEADER;

  unsigned int c_func : 1;	// 0:IREP, 1:C Func
  mrbc_sym sym_id;
  struct RProc *next;
  struct CALLINFO *callinfo;
  struct CALLINFO *callinfo_self;

  union {
    struct IREP *irep;
    mrbc_func_t func;
  };

} mrbc_proc;
typedef struct RProc mrb_proc;


//================================================================
/*! Method management structure.
*/
typedef struct RMethod {
  uint8_t type;		// for debug
  uint8_t c_func;	// 0:IREP, 1:C Func
  mrbc_sym sym_id;
  union {
    struct IREP *irep;
    mrbc_func_t func;
  };
  struct RMethod *next;
} mrbc_method;


/***** Global variables *****************************************************/
extern struct RClass *mrbc_class_tbl[];
#define mrbc_class_nil		mrbc_class_tbl[ MRBC_TT_NIL ]
#define mrbc_class_false	mrbc_class_tbl[ MRBC_TT_FALSE ]
#define mrbc_class_true		mrbc_class_tbl[ MRBC_TT_TRUE ]
#define mrbc_class_fixnum	mrbc_class_tbl[ MRBC_TT_FIXNUM ]
#define mrbc_class_float	mrbc_class_tbl[ MRBC_TT_FLOAT ]
#define mrbc_class_symbol	mrbc_class_tbl[ MRBC_TT_SYMBOL ]
#define mrbc_class_proc		mrbc_class_tbl[ MRBC_TT_PROC ]
#define mrbc_class_array	mrbc_class_tbl[ MRBC_TT_ARRAY ]
#define mrbc_class_string	mrbc_class_tbl[ MRBC_TT_STRING ]
#define mrbc_class_range	mrbc_class_tbl[ MRBC_TT_RANGE ]
#define mrbc_class_hash		mrbc_class_tbl[ MRBC_TT_HASH ]
extern struct RClass *mrbc_class_object;
extern struct RClass *mrbc_class_math;
extern struct RClass *mrbc_class_exception;
extern struct RClass *mrbc_class_standarderror;
extern struct RClass *mrbc_class_runtimeerror;
extern struct RClass *mrbc_class_zerodivisionerror;
extern struct RClass *mrbc_class_argumenterror;
extern struct RClass *mrbc_class_indexerror;
extern struct RClass *mrbc_class_typeerror;


/***** Function prototypes **************************************************/
mrbc_class *mrbc_define_class(struct VM *vm, const char *name, mrbc_class *super);
void mrbc_define_method(struct VM *vm, mrbc_class *cls, const char *name, mrbc_func_t cfunc);
mrbc_value mrbc_instance_new(struct VM *vm, mrbc_class *cls, int size);
void mrbc_instance_delete(mrbc_value *v);
void mrbc_instance_setiv(mrbc_object *obj, mrbc_sym sym_id, mrbc_value *v);
mrbc_value mrbc_instance_getiv(mrbc_object *obj, mrbc_sym sym_id);
mrbc_value mrbc_proc_new(struct VM *vm, void *irep);
void mrbc_proc_delete(mrbc_value *val);
int mrbc_obj_is_kind_of(const mrbc_value *obj, const mrb_class *cls);
mrbc_method *find_method(struct VM *vm, const mrbc_object *recv, mrbc_sym sym_id);
mrbc_method *find_method_by_class(mrbc_class **r_cls, mrbc_class *cls, mrbc_sym sym_id);
mrbc_class *mrbc_get_class_by_name(const char *name);
mrbc_value mrbc_send(struct VM *vm, mrbc_value *v, int reg_ofs, mrbc_value *recv, const char *method, int argc, ...);
void c_ineffect(struct VM *vm, mrbc_value v[], int argc);


/***** Inline functions *****************************************************/

//================================================================
/*! find class by object

  @param  obj	pointer to object
  @return	pointer to mrbc_class
*/
static inline mrbc_class *find_class_by_object(const mrbc_object *obj)
{
  assert( obj->tt > 0 );
  assert( obj->tt <= MRBC_TT_MAXVAL );

  mrbc_class *cls = mrbc_class_tbl[ obj->tt ];
  if( !cls ) {
    // obj->tt is MRBC_TT_OBJECT or MRBC_TT_CLASS
    cls = (obj->tt == MRBC_TT_OBJECT) ? obj->instance->cls : obj->cls;
  }

  return cls;
}


#ifdef __cplusplus
}
#endif
#endif
