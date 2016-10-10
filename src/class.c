#include "value.h"
#include "class.h"
#include "static.h"
#include "console.h"

#include "c_array.h"
#include "c_numeric.h"
#include "c_string.h"
#include "c_range.h"



//================================================================
/*!@brief
  find class from a object

  @param  vm
  @param  obj
  @return pointer to mrb_class
*/
mrb_class *find_class_by_object(mrb_vm *vm, mrb_object *obj)
{
  mrb_class *cls = static_class_object;
  switch( obj->tt ){
    case MRB_TT_ARRAY:
      cls = static_class_array;
      break;
    case MRB_TT_FIXNUM:
      cls = static_class_fixnum;
      break;
    case MRB_TT_FALSE:
      cls = static_class_false;
      break;
    case MRB_TT_TRUE:
      cls = static_class_true;
      break;
#if MRUBYC_USE_FLOAT
    case MRB_TT_FLOAT:
      cls = static_class_float;
      break;
#endif
#if MRUBYC_USE_STRING
    case MRB_TT_STRING:
      cls = static_class_string;
      break;
#endif
    default:
      break;
  }
  return cls;
}



//================================================================
/*!@brief
  find method from

  @param  vm
  @param  recv
  @param  sym_id
  @return
*/
mrb_proc *find_method(mrb_vm *vm, mrb_value recv, mrb_sym sym_id)
{
  mrb_class *cls = find_class_by_object(vm, &recv);

  while( cls != 0 ) {
    mrb_proc *proc = cls->procs;
    while( proc != 0 ) {
      if( proc->sym_id == sym_id ) {
        return proc;
      }
      proc = proc->next;
    }
    cls = cls->super;
  }
  return 0;
}



void mrb_define_method(mrb_vm *vm, mrb_class *cls, const char *name, mrb_func_t cfunc)
{
  mrb_proc *rproc = mrb_rproc_alloc(vm, name);
  rproc->c_func = 1;  // c-func
  rproc->next = cls->procs;
  cls->procs = rproc;
  rproc->func.func = cfunc;
}




void mrb_define_method_proc(mrb_vm *vm, mrb_class *cls, mrb_sym sym_id, mrb_proc *rproc)
{
  rproc->c_func = 0;
  rproc->sym_id = sym_id;
  rproc->next = cls->procs;
  cls->procs = rproc;
}



// Object - puts
void c_puts(mrb_vm *vm, mrb_value *v)
{
  mrb_value *arg0 = v+1;
  switch( arg0->tt ){
  case MRB_TT_FIXNUM:
    console_printf("%d", arg0->value.i);
    break;
  case MRB_TT_FALSE:
//    console_printf("nil");
    break;
#if MRUBYC_USE_FLOAT
  case MRB_TT_FLOAT:
    console_printf("%f", arg0->value.d);
    break;
#endif
#if MRUBYC_USE_STRING
  case MRB_TT_STRING:
    console_printf("%s", arg0->value.str);
    break;
#endif
  case MRB_TT_RANGE:{
    mrb_value *ptr = arg0->value.range;
    if( ptr[0].tt == MRB_TT_TRUE ){
      console_printf("%d...%d", ptr[1].value.i, ptr[2].value.i);
    } else {
      console_printf("%d..%d", ptr[1].value.i, ptr[2].value.i);
    }
  } break;
  default:
    console_printf("Not supported: MRB_TT_XX(%d)", arg0->tt);
    break;
  }
  console_printf("\n");
}


static void mrb_init_class_object(mrb_vm *vm)
{
  // Class
  static_class_object = mrb_class_alloc(vm, "Object", 0);
  // Methods
  mrb_define_method(vm, static_class_object, "puts", c_puts);

}


// =============== FalseClass

void c_false_ne(mrb_vm *vm, mrb_value *v)
{
  mrb_object *arg0 = v+1;
  if( arg0->tt == MRB_TT_FALSE ){
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}


static void mrb_init_class_false(mrb_vm *vm)
{
  // Class
  static_class_false = mrb_class_alloc(vm, "FalseClass", static_class_object);
  // Methods
  mrb_define_method(vm, static_class_false, "!=", c_false_ne);

}


// =============== TrueClass

void c_true_ne(mrb_vm *vm, mrb_value *v)
{
  mrb_object *arg0 = v+1;
  if( arg0->tt == MRB_TT_TRUE ){
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}


static void mrb_init_class_true(mrb_vm *vm)
{
  // Class
  static_class_true = mrb_class_alloc(vm, "TrueClass", static_class_object);
  // Methods
  mrb_define_method(vm, static_class_true, "!=", c_true_ne);

}


void mrb_init_class(void)
{
  mrb_init_class_object(0);
  mrb_init_class_false(0);
  mrb_init_class_true(0);

  mrb_init_class_fixnum(0);
#if MRUBYC_USE_FLOAT
  mrb_init_class_float(0);
#endif
#if MRUBYC_USE_STRING
  mrb_init_class_string(0);
#endif
  mrb_init_class_array(0);
  mrb_init_class_range(0);
}
