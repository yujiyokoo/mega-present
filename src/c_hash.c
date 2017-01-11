#include "c_hash.h"

#include "alloc.h"
#include "class.h"
#include "static.h"
#include "value.h"

static void c_hash_size(mrb_vm *vm, mrb_value *v)
{
}


void mrb_init_class_hash(mrb_vm *vm)
{
  // Hash
  static_class_array = mrb_class_alloc(vm, "Hash", static_class_object);
  
  mrb_define_method(vm, static_class_hash, "size", c_hash_size);

}
