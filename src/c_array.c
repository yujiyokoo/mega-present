#include "c_array.h"

#include "alloc.h"
#include "class.h"
#include "static.h"
#include "value.h"

// Internal use only
// get size of array
static int array_size(mrb_value *v)
{
  mrb_value *array = v->value.obj;
  return array->value.i;
}


// Array#!=
static void c_array_neq(mrb_vm *vm, mrb_value *v)
{
  if( mrbc_eq(v, v+1) ){
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}

// Array = empty?
static void c_array_empty(mrb_vm *vm, mrb_value *v)
{
  if( array_size(v) > 0 ){
    SET_FALSE_RETURN();
  } else {
    SET_TRUE_RETURN();
  }
}

// Array = size
static void c_array_size(mrb_vm *vm, mrb_value *v)
{
  int cnt = array_size(v);
  SET_INT_RETURN( cnt );
}

// Array = []
static void c_array_get(mrb_vm *vm, mrb_value *v)
{
  int pos = GET_INT_ARG(0);
  mrb_value *array = v->value.obj;

  if( pos >= 0 && pos < array->value.i ){
    *v = array[pos+1];
  } else {
    SET_NIL_RETURN();
  }
}

// Array = []=
static void c_array_set(mrb_vm *vm, mrb_value *v)
{
  int pos = GET_INT_ARG(0);
  mrb_value *array = v->value.obj;

  if( pos >= 0 && pos < array->value.i ){
    array[pos+1] = GET_ARG(1);
  } else {
    SET_NIL_RETURN();
  }
}

// Array = operator +
static void c_array_plus(mrb_vm *vm, mrb_value *v)
{
  // because realloc is not ready in alloc.c,
  // use free and alloc
  mrb_value *array1 = v->value.array;
  mrb_value *array2 = GET_ARY_ARG(0).value.array;
  int len1 = array1->value.i;
  int len2 = array2->value.i;
  mrb_value *new_array = (mrb_value *)mrbc_alloc(vm, sizeof(mrb_value)*(len1+len2+1));

  new_array->tt = MRB_TT_FIXNUM;
  new_array->value.i = len1+len2;
  mrb_value *p = new_array + 1;
  int i;
  for( i=0 ; i<len1 ; i++ ){
    *p++ = array1[i+1];
  }
  for( i=0 ; i<len2 ; i++ ){
    *p++ = array2[i+1];
  }
  mrbc_free(vm, array1);
  mrbc_free(vm, array2);
  // return
  v->value.array = new_array;
}


static void c_array_index(mrb_vm *vm, mrb_value *v)
{
  int len = v->value.array->value.i;
  mrb_value *array = v->value.array + 1;
  mrb_value value = GET_ARG(0);

  int i;
  for( i=0 ; i<len ; i++ ){
    // check EQ
    if( mrbc_eq(array+i, &value) ) break;
  }
  if( i<len ){
    SET_INT_RETURN(i);
  } else {
    SET_NIL_RETURN();
  }
}

static void c_array_first(mrb_vm *vm, mrb_value *v)
{
  if( GET_TT_ARG(0) == MRB_TT_FIXNUM ){
    mrb_value *array = v->value.array + 1;
    SET_RETURN( array[0] );
  } else {
    SET_NIL_RETURN();
  }
}

static void c_array_last(mrb_vm *vm, mrb_value *v)
{
  if( GET_TT_ARG(0) == MRB_TT_FIXNUM ){
    int len = v->value.array->value.i;
    mrb_value *array = v->value.array + 1;
    SET_RETURN( array[len-1] );
  } else {
    SET_NIL_RETURN();
  }
}

static void c_array_pop(mrb_vm *vm, mrb_value *v)
{
	mrb_object *obj = v->value.obj;
	mrb_object *tmp = obj->next;
	while( tmp->next ){
		obj = obj->next;
		tmp = obj->next;
	}
	obj->next = tmp->next;
	SET_INT_RETURN(tmp->value.i);
}


void mrbc_init_class_array(mrb_vm *vm)
{
  // Array
  static_class_array = mrbc_class_alloc(vm, "Array", static_class_object);

  mrbc_define_method(vm, static_class_array, "!=", c_array_neq);
  mrbc_define_method(vm, static_class_array, "count", c_array_size);
  mrbc_define_method(vm, static_class_array, "length", c_array_size);
  mrbc_define_method(vm, static_class_array, "size", c_array_size);
  mrbc_define_method(vm, static_class_array, "+", c_array_plus);
  mrbc_define_method(vm, static_class_array, "empty?", c_array_empty);
  mrbc_define_method(vm, static_class_array, "[]", c_array_get);
  mrbc_define_method(vm, static_class_array, "at", c_array_get);
  mrbc_define_method(vm, static_class_array, "[]=", c_array_set);
  mrbc_define_method(vm, static_class_array, "index", c_array_index);


  mrbc_define_method(vm, static_class_array, "first", c_array_first);
  mrbc_define_method(vm, static_class_array, "last", c_array_last);
  mrbc_define_method(vm, static_class_array, "pop", c_array_pop);
}
