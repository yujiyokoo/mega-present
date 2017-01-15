#include "c_symbol.h"

#include "class.h"
#include "static.h"
#include "value.h"


void mrbc_init_class_symbol(mrb_vm *vm)
{
  // Symbol
  mrbc_class_symbol = mrbc_class_alloc(vm, "Symbol", mrbc_class_object);

}

