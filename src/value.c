#include <stdint.h>
#include <string.h>
#include "value.h"
#include "static.h"
#include "symbol.h"
#include "alloc.h"
#include "vm.h"

mrb_object *mrbc_obj_alloc(mrb_vm *vm, mrb_vtype tt)
{
  mrb_object *ptr = (mrb_object *)mrbc_alloc(vm, sizeof(mrb_object));
  if( ptr ){
    ptr->tt = tt;
    ptr->next = 0;
  }
  return ptr;
}

mrb_class *mrbc_class_alloc(mrb_vm *vm, const char *name, mrb_class *super)
{
  mrb_class *ptr = (mrb_class *)mrbc_alloc(vm, sizeof(mrb_class));
  mrb_value v;
  if( ptr ){
    // new class
    mrb_sym sym_id = add_sym(name);
    ptr->super = super;
    ptr->name = sym_id;
    ptr->procs = 0;
    // create value
    v.tt = MRB_TT_CLASS;
    v.cls = ptr;
    // Add to global
    global_object_add(sym_id, v);
  }
  return ptr;
}

mrb_proc *mrbc_rproc_alloc(mrb_vm *vm, const char *name)
{
  mrb_proc *ptr = (mrb_proc *)mrbc_alloc(vm, sizeof(mrb_proc));
  if( ptr ) {
    ptr->sym_id = add_sym(name);
    ptr->next = 0;
  }
  return ptr;
}

mrb_proc *mrbc_rproc_alloc_to_class(mrb_vm *vm, const char *name, mrb_class *cls)
{
  mrb_proc *rproc = mrbc_rproc_alloc(vm, name);
  if( rproc != 0 ){
    rproc->next = cls->procs;
    cls->procs = rproc;
  }
  return rproc;
}


// EQ? two objects
// EQ: return true
// NEQ: return false
int mrbc_eq(mrb_value *v1, mrb_value *v2)
{
  // TT_XXX is different
  if( v1->tt != v2->tt ) return 0;
  // check value
  switch( v1->tt ){
  case MRB_TT_TRUE:
  case MRB_TT_FALSE:
  case MRB_TT_NIL:
    return 1;
  case MRB_TT_FIXNUM:
  case MRB_TT_SYMBOL:
    return v1->i == v2->i;
  case MRB_TT_FLOAT:
    return v1->d == v2->d;
  case MRB_TT_STRING:
    return !strcmp(v1->str, v2->str);
  case MRB_TT_ARRAY: {
    mrb_value *array1 = v1->obj;
    mrb_value *array2 = v2->obj;
    int i, len = array1[0].i;
    if( len != array2[0].i ) return 0;
    for( i=1 ; i<=len ; i++ ){
      if( !mrbc_eq(array1+i, array2+i) ) break;
    }
    if( i > len ){
      return 1;
    } else {
      return 0;
    }
  } break;
  default:
    return 0;
  }
}
