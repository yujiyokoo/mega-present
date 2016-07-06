#include <stdint.h>
#include "value.h"
#include "static.h"
#include "symbol.h"
#include "alloc.h"

mrb_object *mrb_obj_alloc(mrb_vtype tt)
{
  mrb_object *ptr = (mrb_object *)mrbc_alloc(0, sizeof(mrb_object));
  if( ptr  ){
    ptr->tt = tt;
    ptr->next = 0;
  }
  return ptr;
}

mrb_class *mrb_class_alloc(const char *name, mrb_class *super)
{
  mrb_class *ptr = (mrb_class *)mrbc_alloc(0, sizeof(mrb_class));
  if( ptr ){
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
  mrb_proc *ptr = (mrb_proc *)mrbc_alloc(0, sizeof(mrb_proc));
  ptr->sym_id = add_sym(name);
  ptr->next = 0;
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
