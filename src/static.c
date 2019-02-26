/*! @file
  @brief
  Declare static data.

  <pre>
  Copyright (C) 2015-2019 Kyushu Institute of Technology.
  Copyright (C) 2015-2019 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#include "vm_config.h"
#include "static.h"


// Builtin classes.
struct RClass *mrbc_class_object;
struct RClass *mrbc_class_nil;
struct RClass *mrbc_class_false;
struct RClass *mrbc_class_true;
struct RClass *mrbc_class_symbol;
struct RClass *mrbc_class_fixnum;
struct RClass *mrbc_class_float;
struct RClass *mrbc_class_string;
struct RClass *mrbc_class_array;
struct RClass *mrbc_class_range;
struct RClass *mrbc_class_hash;
struct RClass *mrbc_class_proc;
struct RClass *mrbc_class_math;


//================================================================
/*! initialize
*/
void mrbc_init_static(void)
{
  void mrbc_init_global(void);
  void mrbc_init_class(void);

  mrbc_init_global();
  mrbc_init_class();
}


//================================================================
/*! ceeanup
*/
void mrbc_cleanup_static(void)
{
  void mrbc_cleanup_symbol(void);
  void mrbc_cleanup_vm(void);

  mrbc_cleanup_symbol();
  mrbc_cleanup_vm();
}
