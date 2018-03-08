#include "c_symbol.h"

#include "class.h"
#include "static.h"
#include "value.h"
#include "symbol.h"
#include "c_string.h"

#if MRBC_USE_STRING
static void c_symbol_to_s(mrb_vm *vm, mrb_value *v, int argc)
{
  mrb_value value = mrbc_string_new_cstr(vm, symid_to_str(v->i));
  SET_RETURN(value);
}
#endif


static void c_symbol_to_sym(mrb_vm *vm, mrb_value *v, int argc)
{
  // nothing
}

static void c_symbol_equal(mrb_vm *vm, mrb_value *v, int argc)
{
  if( mrbc_eq(v, &GET_ARG(1)) ){
    SET_TRUE_RETURN();
  } else {
    SET_FALSE_RETURN();
  }
}


void mrbc_init_class_symbol(mrb_vm *vm)
{
  // Symbol
  mrbc_class_symbol = mrbc_define_class(vm, "Symbol", mrbc_class_object);

#if MRBC_USE_STRING
  mrbc_define_method(vm, mrbc_class_symbol, "to_s", c_symbol_to_s);
  mrbc_define_method(vm, mrbc_class_symbol, "id2name", c_symbol_to_s);
#endif
  mrbc_define_method(vm, mrbc_class_symbol, "to_sym", c_symbol_to_sym);
  mrbc_define_method(vm, mrbc_class_symbol, "===", c_symbol_equal);
}
