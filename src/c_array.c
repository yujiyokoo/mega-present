#include "c_array.h"

#include "class.h"
#include "static.h"
#include "value.h"

// Internal use only
// get size of array
static int array_size(mrb_value *v)
{
  int cnt = 0;
  mrb_object *obj = v->value.obj;
  while( obj ){
    cnt++;
    obj = obj->next;
  }
  return cnt;
}

// Array = empty?
static void c_array_empty(mrb_vm *vm, mrb_value *v)
{
  if(array_size(v) > 0)
   {SET_FALSE_RETURN();}
  else
   {SET_TRUE_RETURN();}
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
  mrb_object *obj = v->value.obj;
  while( pos > 0 && obj != 0 ){
    pos--;
    obj = obj->next;
  }
  if( obj ){
    *v = *obj;
  } else {
    SET_NIL_RETURN();
  }
}

// Array = []=
static void c_array_set(mrb_vm *vm, mrb_value *v)
{
  int pos = GET_INT_ARG(0);

  mrb_object *obj = v->value.obj;
  while( pos > 0 && obj != 0 ){
    pos--;
    obj = obj->next;
  }
  if( obj ){
    int tt = GET_TT_ARG(1);
    obj->tt = tt;
    if( tt == MRB_TT_FIXNUM ){
      obj->value.i = GET_INT_ARG(1);
    } else if( tt == MRB_TT_FLOAT ){
      obj->value.i = GET_INT_ARG(1);
    } else {
      //
    }
  } else {
    SET_NIL_RETURN();
  }
}

// Array = operator +
static void c_array_plus(mrb_vm *vm, mrb_value *v)
{
  if( GET_TT_ARG(0) == MRB_TT_ARRAY ){
    mrb_value ary;
    ary.tt = MRB_TT_ARRAY;
    ary.value.obj = 0;
    mrb_value *src = v->value.obj;
    mrb_value **dst = &ary.value.obj;
    // SRC
    if( src ){
      *dst = mrb_obj_alloc(MRB_TT_OBJECT);
      **dst = *src;
      (*dst)->next = 0;
      dst = &((*dst)->next);
      src = src->next;
      while( src ){
        *dst = mrb_obj_alloc(MRB_TT_OBJECT);
        **dst = *src;
        (*dst)->next = 0;
        dst = &((*dst)->next);
        src = src->next;
      }
    }
    // DST
    src = GET_ARY_ARG(0).value.obj;
    if( src ){
      *dst = mrb_obj_alloc(MRB_TT_OBJECT);
      **dst = *src;
      (*dst)->next = 0;
      dst = &((*dst)->next);
      src = src->next;
      while( src ){
        *dst = mrb_obj_alloc(MRB_TT_OBJECT);
        **dst = *src;
        (*dst)->next = 0;
        dst = &((*dst)->next);
        src = src->next;
      }
    }

    SET_RETURN(ary);
  } else {
    SET_NIL_RETURN();
  }
}

// Join other array to this array's last
/*
static void c_array_concat(mrb_vm *vm, mrb_value *v)
{
	mrb_object *other_ary = GET_ARY_ARG(0);
	mrb_object *obj = v->value.obj;
	mrb_object *before_obj;
	while( obj ){
		before_obj = obj;
		obj = obj->next;
	}
	before_obj->next = other_ary;
}
*/

// Delete any position element
/*
static void c_array_delete_at(mrb_vm *vm, mrb_value *v)
{
  if( GET_TT_ARG(0) != MRB_TT_FIXNUM ) return;  // arg is not Fixnum
  if( v->value.obj == 0 ) return;  // ary is []
	int pos = GET_INT_ARG(0);
  if( 0 <= pos && pos < array_size(v) ){
    if( pos == 0 ){
      // delete top element
      v->value.obj->tt = MRB_TT_EMPTY;
      v->value.obj = v->value.obj->next;
    } else {
      // delete following element

    }
  } else {
    // nothing: pos is out of array
  }
}
*/

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
      mrb_value *ptr = mrb_obj_alloc(MRB_TT_OBJECT);
      ret.value.obj = ptr;
      *ptr = *obj;
      ptr->next = 0;
      obj = obj->next;
      while( obj ){
        ptr->next = mrb_obj_alloc(MRB_TT_OBJECT);
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

// Array = reverse!
/*
static void c_array_reverse_bang(mrb_vm *vm, mrb_value *v)
{
mrb_value ret;
mrb_value *one = v->value.obj;

  ret.tt = MRB_TT_ARRAY;
  ret.value.obj = 0;

  if(array_size(v) > 1)
   {
   mrb_value *two = one->next;
   mrb_value *tmp;
   }
  SET_RETURN(ret);
} */


void mrb_init_class_array(void)
{
	// Array
	static_class_array = mrb_class_alloc("Array", static_class_object);
	mrb_define_method(static_class_array, "count", c_array_size);
	mrb_define_method(static_class_array, "+", c_array_plus);
	//mrb_define_method(static_class_array, "*", c_array_times);
	//mrb_define_method(static_class_array, "<<", c_array_push);
	mrb_define_method(static_class_array, "[]", c_array_get);
	mrb_define_method(static_class_array, "at", c_array_get);
	mrb_define_method(static_class_array, "[]=", c_array_set);
//	mrb_define_method(static_class_array, "concat", c_array_concat);
//	mrb_define_method(static_class_array, "delete_at", c_array_delete_at);
	mrb_define_method(static_class_array, "empty?", c_array_empty);
	//mrb_define_method(static_class_array, "first", c_array_first);
	mrb_define_method(static_class_array, "index", c_array_index);
	//mrb_define_method(static_class_array, "initialize_copy", c_array_replace);
	//mrb_define_method(static_class_array, "join", c_array_join);
	mrb_define_method(static_class_array, "last", c_array_last);
	mrb_define_method(static_class_array, "length", c_array_size);
	mrb_define_method(static_class_array, "pop", c_array_pop);
	//mrb_define_method(static_class_array, "push", c_array_push);
	//mrb_define_method(static_class_array, "replace", c_array_replace);
	//mrb_define_method(static_class_array, "reverse", c_array_reverse);
	//mrb_define_method(static_class_array, "reverse!", c_array_reverse_bang);
	//mrb_define_method(static_class_array, "rindex", c_array_rindex);
	//mrb_define_method(static_class_array, "shift", c_array_shift);
	mrb_define_method(static_class_array, "size", c_array_size);
	//mrb_define_method(static_class_array, "slice", c_array_get);
	//mrb_define_method(static_class_array, "unshift", c_array_unshift);
}
