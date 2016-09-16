#include "common.h"
#include "static.h"
#include "vm_config.h"
#include "class.h"
#include "symbol.h"

/* Static Variables */
/* VM contains regs, stack, PC, and so on */
mrb_vm static_vm[MAX_VM_COUNT];

//static mrb_object static_object[MAX_OBJECT_COUNT];
//mrb_object *static_pool_object;

mrb_constobject static_const[MAX_CONST_COUNT];

mrb_globalobject static_global[MAX_GLOBAL_OBJECT_SIZE];

/* Class Tree */
mrb_class *static_class_object;

/* Classes */
mrb_class *static_class_false;
mrb_class *static_class_true;
mrb_class *static_class_nil;
mrb_class *static_class_array;
mrb_class *static_class_fixnum;
#if MRUBYC_USE_FLOAT
mrb_class *static_class_float;
#endif
#if MRUBYC_USE_STRING
mrb_class *static_class_string;
#endif


void init_static(void)
{
  int i;

  for( i=0 ; i<MAX_VM_COUNT ; i++ ){
    static_vm[i].vm_id = i+1;
    static_vm[i].priority = -1;
  }

  /* global objects */
  for( i=0 ; i<MAX_GLOBAL_OBJECT_SIZE ; i++ ){
    static_global[i].sym_id = -1;
  }

  for( i=0 ; i<MAX_CONST_COUNT; i++ ){
    static_const[i].sym_id = -1;
  }

  /* symbol */
  init_sym();

  /* init class */
  mrb_init_class();
}
