#include "c_range.h"

#include "alloc.h"
#include "class.h"
#include "static.h"
#include "value.h"
#include "vm.h"


mrb_value mrb_range_new(mrb_vm *vm, mrb_value *v_st, mrb_value *v_ed, int exclude)
{
  mrb_value value;
  mrb_value *ptr = (mrb_value*)mrbc_alloc(vm, sizeof(mrb_value)*3);
  if( exclude ){
    ptr[0].tt = MRB_TT_TRUE;
  } else {
    ptr[0].tt = MRB_TT_FALSE;
  }
  ptr[1] = *v_st;
  ptr[2] = *v_ed;
  value.tt = MRB_TT_RANGE;
  value.value.range = ptr;

  return value;
}




// init class
void mrb_init_class_range(mrb_vm *vm)
{
  static_class_range = mrb_class_alloc(vm, "Range", static_class_object);


}
