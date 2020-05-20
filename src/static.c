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
#include "value.h"
#include "static.h"

//================================================================
/*! Builtin class table.
*/
struct RClass *mrbc_class_tbl[MRBC_TT_MAXVAL+1];
struct RClass *mrbc_class_object;
struct RClass *mrbc_class_math;
struct RClass *mrbc_class_exception;
struct RClass *mrbc_class_standarderror;
struct RClass *mrbc_class_runtimeerror;
struct RClass *mrbc_class_zerodivisionerror;
struct RClass *mrbc_class_argumenterror;
struct RClass *mrbc_class_indexerror;
struct RClass *mrbc_class_typeerror;


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
