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
#include "symbol.h"

static void c_exception_message(struct VM *vm, mrbc_value v[], int argc)
{
  mrbc_decref( &v[0] );
  if( vm->exc_message.tt == MRBC_TT_NIL ){
    const char *name = mrbc_symid_to_str(v[0].cls->sym_id);
    v[0] = mrbc_string_new(vm, name, strlen(name));
  } else {
    mrbc_incref( &vm->exc_message );
    v[0] = vm->exc_message;
  }
}


void mrbc_raiseX(mrbc_vm *vm, mrbc_error_code err, char *msg)
{
  vm->exc.tt = MRBC_TT_EXCEPTION;
  vm->exc.exception = mrbc_class_runtimeerror;
  // vm->exc_message = mrbc_nil_value();
  // if( vm->exception_tail == NULL ) return;
}


void mrbc_init_class_exception(void)
{
  mrbc_class_exception = mrbc_define_class(0, "Exception", mrbc_class_object);
  mrbc_define_method(0, mrbc_class_exception, "message", c_exception_message);

  // Exception
  //  |
  //  +-- StandardError
  mrbc_class_standarderror = mrbc_define_class(0, "StandardError", mrbc_class_exception);
  mrbc_class_runtimeerror = mrbc_define_class(0, "RuntimeError", mrbc_class_standarderror);
  mrbc_define_class(0, "ZeroDivisionError", mrbc_class_standarderror);
  mrbc_define_class(0, "ArgumentError", mrbc_class_standarderror);
  mrbc_define_class(0, "IndexError", mrbc_class_standarderror);
  mrbc_define_class(0, "RangeError", mrbc_class_standarderror);
  mrbc_define_class(0, "TypeError", mrbc_class_standarderror);

  // Exception
  //  |
  //  +--NoMemoryError
  mrbc_define_class(0, "NoMemoryError", mrbc_class_exception);

  // NameError
  //  |
  //  +--NoMethodError
  struct RClass *class_nameerror;
  class_nameerror =  mrbc_define_class(0, "NameError", mrbc_class_standarderror);
  mrbc_define_class(0, "NoMethodError", class_nameerror);
}
