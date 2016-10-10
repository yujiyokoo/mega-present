#include "c_array.h"

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
    array[pos+1] = *v;
  } else {
    SET_NIL_RETURN();
  }
}

// Array = operator +
static void c_array_plus(mrb_vm *vm, mrb_value *v)
{

}


static void c_array_index(mrb_vm *vm, mrb_value *v)
{
  if( GET_TT_ARG(0) == MRB_TT_FIXNUM ){
    int num = GET_INT_ARG(0);
    int cnt = 0;
    mrb_object *obj = v->value.obj;
    while( obj && num != obj->value.i ){
      cnt++;
      obj = obj->next;
    }
    if( obj != 0 ){
      SET_INT_RETURN(cnt);
    } else {
      SET_NIL_RETURN();
    }
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
  mrb_define_method(vm, static_class_array, "+", c_array_plus);
  mrb_define_method(vm, static_class_array, "[]", c_array_get);
  mrb_define_method(vm, static_class_array, "at", c_array_get);
  mrb_define_method(vm, static_class_array, "[]=", c_array_set);
  mrb_define_method(vm, static_class_array, "empty?", c_array_empty);
  mrb_define_method(vm, static_class_array, "index", c_array_index);
  mrb_define_method(vm, static_class_array, "last", c_array_last);
  mrb_define_method(vm, static_class_array, "length", c_array_size);
  mrb_define_method(vm, static_class_array, "pop", c_array_pop);
  mrb_define_method(vm, static_class_array, "size", c_array_size);
}
