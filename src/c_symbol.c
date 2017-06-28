#include "c_symbol.h"

#include "class.h"
#include "static.h"
#include "value.h"
#include "symbol.h"

static void c_symbol_to_s(mrb_vm *vm, mrb_value *v)
{
  v->tt = MRB_TT_STRING;

  const char *sym = symid_to_str(v->i);
  v->str = mrbc_string_dup(vm, sym);
}

static void c_symbol_to_sym(mrb_vm *vm, mrb_value *v)
{
  // nothing
}

static void c_symbol_equal(mrb_vm *vm, mrb_value *v)
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
  mrbc_class_symbol = mrbc_class_alloc(vm, "Symbol", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_symbol, "to_s", c_symbol_to_s);
  mrbc_define_method(vm, mrbc_class_symbol, "id2name", c_symbol_to_s);
  mrbc_define_method(vm, mrbc_class_symbol, "to_sym", c_symbol_to_sym);
  mrbc_define_method(vm, mrbc_class_symbol, "===", c_symbol_equal);
}

