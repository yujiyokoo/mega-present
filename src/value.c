#include <stdint.h>
#include "value.h"
#include "static.h"
#include "symbol.h"

mrb_object *mrb_obj_alloc(mrb_vtype tt)
{
  mrb_object *ptr = static_pool_object;
  if( ptr != 0 ){
    static_pool_object = ptr->next;
    ptr->next = 0;
    ptr->tt = tt;
  }
  return ptr;
}

mrb_class *mrb_class_alloc(const char *name, mrb_class *super)
{
  mrb_class *ptr = static_pool_class;
  if( ptr != 0 ){
    static_pool_class = ptr->next;
    ptr->tt = MRB_TT_CLASS;
    ptr->super = super;
    ptr->name = add_sym(name);
    ptr->procs = 0;
    ptr->next = 0;
  }
  return ptr;
}

mrb_proc *mrb_rproc_alloc(const char *name)
{
  mrb_proc *ptr = static_pool_proc;
  if( ptr != 0 ){
    static_pool_proc = ptr->next;
    ptr->sym_id = add_sym(name);
    ptr->next = 0;
  }
  return ptr;
}

mrb_proc *mrb_rproc_alloc_to_class(const char *name, mrb_class *cls)
{
  mrb_proc *rproc = mrb_rproc_alloc(name);
  if( rproc != 0 ){
    rproc->next = cls->procs;
    cls->procs = rproc;
  }
  return rproc;
}
