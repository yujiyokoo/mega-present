/*! @file
  @brief
  exception classes

  <pre>
  Copyright (C) 2015-2021 Kyushu Institute of Technology.
  Copyright (C) 2015-2021 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Fetch mruby VM bytecodes, decode and execute.

  </pre>
*/

#include "vm_config.h"
#include <stddef.h>
#include <string.h>
#include "vm.h"
#include "error.h"
#include "c_string.h"


static void c_exception_message(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_decref( &v[0] );
  if( vm->exc_message.tt == MRBC_TT_NIL ){
    v[0] = mrbc_string_new(vm, "", 0);
  } else {
    mrbc_incref( &vm->exc_message );
    v[0] = vm->exc_message;
  }
}


void mrbc_raiseX(mrbc_vm *vm, mrbc_error_code err, char *msg)
{
  vm->exc.tt = MRBC_TT_EXCEPTION;
  vm->exc.exception = MRBC_CLASS(RuntimeError);
  // vm->exc_message = mrbc_nil_value();
  // if( vm->exception_tail == NULL ) return;
}

/* mruby/c Exception class hierarchy.

    Exception
      NoMemoryError
      StandardError
        ArgumentError
        IndexError
        NameError
          NoMethodError
        RangeError
        RuntimeError
        TypeError
        ZeroDivisionError
*/

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("Exception")
  FILE("method_table_exception.h")
  METHOD("message",	c_exception_message )
*/
#include "method_table_exception.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("NoMemoryError")
  SUPER("Exception")
  FILE("method_table_nomemoryerror.h")
*/
#include "method_table_nomemoryerror.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("StandardError")
  SUPER("Exception")
  FILE("method_table_standarderror.h")
*/
#include "method_table_standarderror.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("ArgumentError")
  SUPER("StandardError")
  FILE("method_table_argumenterror.h")
*/
#include "method_table_argumenterror.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("IndexError")
  SUPER("StandardError")
  FILE("method_table_indexerror.h")
*/
#include "method_table_indexerror.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("NameError")
  SUPER("StandardError")
  FILE("method_table_nameerror.h")
*/
#include "method_table_nameerror.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("NoMethodError")
  SUPER("NameError")
  FILE("method_table_nomethoderror.h")
*/
#include "method_table_nomethoderror.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("RangeError")
  SUPER("StandardError")
  FILE("method_table_rangeerror.h")
*/
#include "method_table_rangeerror.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("RuntimeError")
  SUPER("StandardError")
  FILE("method_table_runtimeerror.h")
*/
#include "method_table_runtimeerror.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("TypeError")
  SUPER("StandardError")
  FILE("method_table_typeerror.h")
*/
#include "method_table_typeerror.h"

/* MRBC_AUTOGEN_METHOD_TABLE
  CLASS("ZeroDivisionError")
  SUPER("StandardError")
  FILE("method_table_zerodivisionerror.h")
*/
#include "method_table_zerodivisionerror.h"
