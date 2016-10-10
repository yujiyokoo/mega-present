#include <string.h>
#include "c_string.h"

#include "alloc.h"
#include "class.h"
#include "static.h"
#include "value.h"
#include "vm.h"

// dupulicate string (clone)
// returns duplicated string pointer
char* mrb_string_dup(mrb_vm *vm, const char *str)
{
  int len = strlen((char *)str);
  char *ptr = (char *)mrbc_alloc(vm, len+1);
  strcpy(ptr, str);
  return ptr;
}

// catination string
// returns new string
char *mrb_string_cat(mrb_vm *vm, char *s1, const char *s2)
{
  int len1 = strlen(s1);
  int len2 = strlen(s2);
  char *ptr = (char *)mrbc_alloc(vm, len1+len2+1);
  strcpy(ptr, s1);
  strcpy(ptr+len1, s2);
  return ptr;
}

// substr
// returns new string
static char *mrb_string_substr(mrb_vm *vm, char *s, int start, int len)
{
  char *ptr = (char *)mrbc_alloc(vm, len+1);
  int i;
  for( i=0 ; i<len ; i++ ){
    ptr[i] = s[start+i];
  }
  ptr[len] = 0;
  return ptr;
}


// method
// string size
static void c_string_size(mrb_vm *vm, mrb_value *v)
{
  int cnt = strlen(v->value.str);
  SET_INT_RETURN( cnt );
}

// method
// string []
//  string[Fixnum]
static void c_string_idx_get(mrb_vm *vm, mrb_value *v)
{
  int index = GET_INT_ARG(0);
  char *str = mrb_string_substr(vm, v->value.str, index, 1);
  v->value.str = str;
}


// method
// string to_i
static void c_string_to_fixnum(mrb_vm *vm, mrb_value *v)
{
  char *str = v->value.str;
  int value = 0;
  while( *str && *str >= '0' && *str <= '9' ){
    value *= 10;
    value += *str - '0';
    str++;
  }
  SET_INT_RETURN(value);
}



// init class
void mrb_init_class_string(mrb_vm *vm)
{
  static_class_string = mrb_class_alloc(vm, "String", static_class_object);

  mrb_define_method(vm, static_class_string, "size", c_string_size);
  mrb_define_method(vm, static_class_string, "length", c_string_size);
  mrb_define_method(vm, static_class_string, "[]", c_string_idx_get);
  mrb_define_method(vm, static_class_string, "to_i", c_string_to_fixnum);
}
