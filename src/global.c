
#include "value.h"
#include "static.h"
#include "vm_config.h"

/*

  GLobal objects are stored in 'mrbc_global' array.
  'mrbc_global' array is decending order by sym_id.
  In case of searching a global object, binary search is used.
  In case of adding a global object, insertion sort is used.

*/

typedef struct GLOBAL_OBJECT {
  mrb_sym sym_id;
  mrb_object obj;
} mrb_globalobject;

// max of global object in mrbc_global[]
static int global_right;
static mrb_globalobject mrbc_global[MAX_GLOBAL_OBJECT_SIZE];

//
void  mrbc_init_global(void)
{
  global_right = 0;
}

/* search */
static int search_global_object(mrb_sym sym_id)
{
  int left = 0, right = global_right - 1;
  while( left <= right ){
    int mid = (left+right)/2;
    if( mrbc_global[mid].sym_id == sym_id ) return mid;
    if( mrbc_global[mid].sym_id < sym_id ){
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }
  return -1;
}

/*
static int search_const(mrb_sym sym_id) {
  int left = 0, right = MAX_CONST_COUNT-1;
  while (left <= right) {
    int mid = (left + right) / 2;
    if ( mrbc_const[mid].sym_id == sym_id ) return mid;
    if ( mrbc_const[mid].sym_id < sym_id ) {
      right = mid - 1;
    } else {
      left = mid + 1;
    }
  }
  return -1;
}
*/

/* add */
void global_object_add(mrb_sym sym_id, mrb_value v)
{
  int index = search_global_object(sym_id);
  if( index == -1 ){
    index = global_right - 1;
    while( index > 0 && mrbc_global[index].sym_id < sym_id ){
      mrbc_global[index] = mrbc_global[index-1];
      index--;
    }
  }
  mrbc_global[index].sym_id = sym_id;
  mrbc_global[index].obj = v;
}

void const_add(mrb_sym sym_id, mrb_object *obj)
{
  global_object_add(sym_id, *obj);
}

/* get */
mrb_value global_object_get(mrb_sym sym_id)
{
  int index = search_global_object(sym_id);
  if( index >= 0 ){
    return mrbc_global[index].obj;
  } else {
    /* nil */
    mrb_value v;
    v.tt = MRB_TT_NIL;
    return v;
  }
}

mrb_object const_get(mrb_sym sym_id) {
  return global_object_get(sym_id);
}
