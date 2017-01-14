/*! @file
  @brief
  Declare static data.

  <pre>
  Copyright (C) 2015-2016 Kyushu Institute of Technology.
  Copyright (C) 2015-2016 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#include "common.h"
#include "static.h"
#include "vm_config.h"
#include "class.h"
#include "symbol.h"

/* Static Variables */
/* VM contains regs, stack, PC, and so on */
mrb_vm mrbc_vm[MAX_VM_COUNT];

//static mrb_object mrbc_object[MAX_OBJECT_COUNT];
//mrb_object *mrbc_pool_object;

mrb_constobject mrbc_const[MAX_CONST_COUNT];

mrb_globalobject mrbc_global[MAX_GLOBAL_OBJECT_SIZE];

/* Class Tree */
mrb_class *mrbc_class_object;

/* Classes */
mrb_class *mrbc_class_false;
mrb_class *mrbc_class_true;
mrb_class *mrbc_class_nil;
mrb_class *mrbc_class_array;
mrb_class *mrbc_class_fixnum;
#if MRBC_USE_FLOAT
mrb_class *mrbc_class_float;
#endif
#if MRBC_USE_STRING
mrb_class *mrbc_class_string;
#endif
mrb_class *mrbc_class_range;
mrb_class *mrbc_class_hash;

void init_static(void)
{
  int i;

  for( i=0 ; i<MAX_VM_COUNT ; i++ ){
    mrbc_vm[i].vm_id = i+1;
    mrbc_vm[i].priority = -1;
  }

  /* global objects */
  for( i=0 ; i<MAX_GLOBAL_OBJECT_SIZE ; i++ ){
    mrbc_global[i].sym_id = -1;
  }

  for( i=0 ; i<MAX_CONST_COUNT; i++ ){
    mrbc_const[i].sym_id = -1;
  }

  /* init class */
  mrbc_init_class();
}
