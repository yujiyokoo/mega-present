/*! @file
  @brief
  exception classes

  <pre>
  Copyright (C) 2015-2019 Kyushu Institute of Technology.
  Copyright (C) 2015-2019 Shimane IT Open-Innovation Center.

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
  vm->exc.exception = mrbc_class_runtimeerror;
  // vm->exc_message = mrbc_nil_value();
  // if( vm->exception_tail == NULL ) return;
}


void mrbc_init_class_exception(struct VM *vm)
{
  mrbc_class_exception = mrbc_define_class(vm, "Exception", mrbc_class_object);
  mrbc_define_method(vm, mrbc_class_exception, "message", c_exception_message);

  // Exception
  //  |
  //  +-- StandardError
  mrbc_class_standarderror = mrbc_define_class(vm, "StandardError", mrbc_class_exception);
  mrbc_class_runtimeerror = mrbc_define_class(vm, "RuntimeError", mrbc_class_standarderror);
  mrbc_define_class(vm, "ZeroDivisionError", mrbc_class_standarderror);
  mrbc_define_class(vm, "ArgumentError", mrbc_class_standarderror);
  mrbc_define_class(vm, "IndexError", mrbc_class_standarderror);
  mrbc_define_class(vm, "RangeError", mrbc_class_standarderror);
  mrbc_define_class(vm, "TypeError", mrbc_class_standarderror);

  // Exception
  //  |
  //  +--NoMemoryError
  mrbc_define_class(vm, "NoMemoryError", mrbc_class_exception);

  // NameError
  //  |
  //  +--NoMethodError 
  struct RClass *class_nameerror;
  class_nameerror =  mrbc_define_class(vm, "NameError", mrbc_class_standarderror);
  mrbc_define_class(vm, "NoMethodError", class_nameerror);
}
