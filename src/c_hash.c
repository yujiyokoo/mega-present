#include "c_hash.h"

#include "alloc.h"
#include "class.h"
#include "static.h"
#include "value.h"

static void c_hash_size(mrb_vm *vm, mrb_value *v)
{
  mrb_value *hash = v->value.obj;

  SET_INT_RETURN(hash->value.i);
}


// Hash = []
static void c_hash_get(mrb_vm *vm, mrb_value *v)
{
  mrb_value *hash = v->value.obj;
  int i;
  int n = hash->value.i;       // hash size
  mrb_value key = GET_ARG(0);  // search key

  for( i=0 ; i<n ; i++ ){
    if( mrb_eq(&hash[i*2+1], &key) ){
      SET_RETURN(hash[i*2+2]);
      return;
    }
  }

  SET_NIL_RETURN();
}

// Hash = []=
static void c_hash_set(mrb_vm *vm, mrb_value *v)
{
  mrb_value *hash = v->value.obj;
  int i;
  int n = hash->value.i;       // hash size
  mrb_value key = GET_ARG(0);  // search key
  mrb_value val = GET_ARG(1);  // store value

  for( i=0 ; i<n ; i++ ){
    if( mrb_eq(&hash[i*2+1], &key) ){
      hash[i*2+2] = val;
      return;
    }
  }
  
  // key was not found
  // TODO: add key
}


void mrb_init_class_hash(mrb_vm *vm)
{
  // Hash
  mrbc_class_hash = mrb_class_alloc(vm, "Hash", mrbc_class_object);
  
  mrb_define_method(vm, mrbc_class_hash, "size", c_hash_size);
  mrb_define_method(vm, mrbc_class_hash, "[]", c_hash_get);
  mrb_define_method(vm, mrbc_class_hash, "[]=", c_hash_set);

}
