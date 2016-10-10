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
  return array->value.array_len;
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

  if( pos >= 0 && pos < array->value.array_len ){
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

  if( pos >= 0 && pos < array->value.array_len ){
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
  int len1 = array1->value.array_len;
  int len2 = array2->value.array_len;
  mrb_value *new_array = (mrb_value *)mrbc_alloc(vm, sizeof(mrb_value)*(len1+len2+1));
  
  new_array->value.array_len = len1+len2;
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
  int len = v->value.array->value.array_len;
  mrb_value *array = v->value.array + 1;
  mrb_value value = GET_ARG(0);

  int i;
  for( i=0 ; i<len ; i++ ){
    // check EQ
    if( mrb_eq(array+i, &value) ) break;
  }
  if( i<len ){
    SET_INT_RETURN(i);
  } else {
    SET_NIL_RETURN();
  }
}

static void c_array_last(mrb_vm *vm, mrb_value *v)
{
  if( GET_TT_ARG(0) == MRB_TT_FIXNUM ){
    int num = GET_INT_ARG(0);
    int cnt = array_size(v);
    mrb_value ret;
    ret.tt = MRB_TT_ARRAY;
    ret.value.obj = 0;
    mrb_value *obj = v->value.obj;
    while( obj && cnt > num ){
      num++;
      obj = obj->next;
    }
    if( obj ){
      mrb_value *ptr = mrb_obj_alloc(vm, MRB_TT_OBJECT);
      ret.value.obj = ptr;
      *ptr = *obj;
      ptr->next = 0;
      obj = obj->next;
      while( obj ){
        ptr->next = mrb_obj_alloc(vm, MRB_TT_OBJECT);
        ptr = ptr->next;
        *ptr = *obj;
        ptr->next = 0;
        obj = obj->next;
      }
    }
    SET_RETURN(ret);
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


void mrb_init_class_array(mrb_vm *vm)
{
  // Array
  static_class_array = mrb_class_alloc(vm, "Array", static_class_object);
  
  mrb_define_method(vm, static_class_array, "count", c_array_size);
  mrb_define_method(vm, static_class_array, "length", c_array_size);
  mrb_define_method(vm, static_class_array, "size", c_array_size);
  mrb_define_method(vm, static_class_array, "+", c_array_plus);
  mrb_define_method(vm, static_class_array, "empty?", c_array_empty);
  mrb_define_method(vm, static_class_array, "[]", c_array_get);
  mrb_define_method(vm, static_class_array, "at", c_array_get);
  mrb_define_method(vm, static_class_array, "[]=", c_array_set);
  mrb_define_method(vm, static_class_array, "index", c_array_index);

  mrb_define_method(vm, static_class_array, "last", c_array_last);
  mrb_define_method(vm, static_class_array, "pop", c_array_pop);
}
