#include "c_hash.h"

#include "alloc.h"
#include "class.h"
#include "static.h"
#include "value.h"

static void c_hash_size(mrb_vm *vm, mrb_value *v)
{
  mrb_value *hash = v->value.obj;

  SET_INT_RETURN(hash->value.obj->value.i);
}


// Hash = []
static void c_hash_get(mrb_vm *vm, mrb_value *v)
{
  mrb_value *hash = v->value.obj->value.obj;
  int i;
  int n = hash->value.i;       // hash size
  mrb_value key = GET_ARG(0);  // search key

  // ptr: 1st entry(key) of hash
  mrb_value *ptr = &hash[1];

  for( i=0 ; i<n ; i++ ){
    if( mrbc_eq(ptr, &key) ){
      SET_RETURN( *(ptr+1) );
      return;
    }
    ptr += 2;
  }

  SET_NIL_RETURN();
}

// Hash = []=
static void c_hash_set(mrb_vm *vm, mrb_value *v)
{
  mrb_value *hash = v->value.obj->value.obj;
  int i;
  int n = hash[0].value.i;       // hash size
  mrb_value key = GET_ARG(0);  // search key
  mrb_value val = GET_ARG(1);  // new value

  mrb_value *ptr = &hash[1];
  for( i=0 ; i<n ; i++ ){
    if( mrbc_eq(ptr, &key) ){
      *(ptr+1) = val;  // Change value
      return;
    }
  }

  // key was not found
  // add hash entry (key and val)
  int new_size = (n+1)*2 + 1;

  // use alloc instead of realloc, realloc has some bugs?
  mrb_value *new_hash = (mrb_value *)mrbc_alloc(vm, sizeof(mrb_value)*new_size);
  if( new_hash == NULL ) return;  // ENOMEM

  mrb_value *src = &hash[1];
  mrb_value *dst = &new_hash[1];
  for( i=0 ; i<n ; i++ ){
    *dst++ = *src++;  // copy key
    *dst++ = *src++;  // copy value
  }
  new_hash[0].tt = MRB_TT_FIXNUM;
  new_hash[0].value.i = n+1;
  *dst++ = key;
  *dst   = val;
  mrbc_free(vm, v->value.obj->value.obj);
  v->value.obj->value.obj = new_hash;
}


void mrbc_init_class_hash(mrb_vm *vm)
{
  // Hash
  mrbc_class_hash = mrbc_class_alloc(vm, "Hash", mrbc_class_object);

  mrbc_define_method(vm, mrbc_class_hash, "size", c_hash_size);
  mrbc_define_method(vm, mrbc_class_hash, "[]", c_hash_get);
  mrbc_define_method(vm, mrbc_class_hash, "[]=", c_hash_set);

}
