#include "common.h"
#include "static.h"
#include "vm_config.h"
#include "class.h"
#include "symbol.h"

/* Static Variables */
/* Shared Objects */
static mrb_vm static_vm[MAX_VM_COUNT];
mrb_vm *static_pool_vm;

static mrb_irep static_irep[MAX_IREP_COUNT];
mrb_irep *static_pool_irep;

static mrb_object static_object[MAX_OBJECT_COUNT];
mrb_object *static_pool_object;

static mrb_class static_class[MAX_CLASS_COUNT];
mrb_class *static_pool_class;

static mrb_proc static_proc[MAX_PROC_COUNT];
mrb_proc *static_pool_proc;

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

void init_static(void)
{
  int i;

  static_pool_vm = static_vm;
  for( i=0 ; i<MAX_VM_COUNT-1 ; i++ ){
    static_vm[i].next = static_vm + i + 1;
  }
  static_vm[MAX_VM_COUNT-1].next = 0;

  static_pool_irep = static_irep;
  for( i=0 ; i<MAX_IREP_COUNT-1 ; i++ ) {
    static_irep[i].next = static_irep + i + 1;
  }
  static_irep[MAX_IREP_COUNT-1].next = 0;

  static_pool_object = static_object;
  for( i=0 ; i<MAX_OBJECT_COUNT-1 ; i++ ){
    static_object[i].next = static_object + i + 1;
  }
  static_object[MAX_OBJECT_COUNT-1].next = 0;

  static_pool_class = static_class;
  for( i=0 ; i<MAX_CLASS_COUNT-1 ; i++ ){
    static_class[i].next = static_class + i + 1;
  }
  static_class[MAX_CLASS_COUNT-1].next = 0;

  static_pool_proc = static_proc;
  for( i=0 ; i<MAX_PROC_COUNT-1 ; i++ ){
    static_proc[i].next = static_proc + i + 1;
  }
  static_proc[MAX_PROC_COUNT-1].next = 0;

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
