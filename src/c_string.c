#include "c_string.h"

#include "alloc.h"
#include "class.h"
#include "common.h"
#include "static.h"
#include "value.h"


// dupulicate string (clone)
// returns duplicated string pointer
char* mrb_string_dup(const char *str)
{
  int len = my_strlen((char *)str);
  char *ptr = (char *)mrbc_alloc(0, len+1);
  my_strcpy(ptr, str);
  return ptr;
}

// catination string
// returns new string
char *mrb_string_cat(char *s1, const char *s2)
{
  int len1 = my_strlen(s1);
  int len2 = my_strlen(s2);
  char *ptr = (char *)mrbc_alloc(0, len1+len2+1);
  my_strcpy(ptr, s1);
  my_strcpy(ptr+len1, s2);
  return ptr;
}

// substr
// returns new string
static char *mrb_string_substr(char *s, int start, int len)
{
  char *ptr = (char *)mrbc_alloc(0, len+1);
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
  int cnt = my_strlen(v->value.str);
  SET_INT_RETURN( cnt );
}

// method
// string []
//  string[Fixnum]
static void c_string_idx_get(mrb_vm *vm, mrb_value *v)
{
  int index = GET_INT_ARG(0);
  char *str = mrb_string_substr(v->value.str, index, 1);
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
void mrb_init_class_string(void)
{
  static_class_string = mrb_class_alloc("String", static_class_object);

  mrb_define_method(static_class_string, "size", c_string_size);
  mrb_define_method(static_class_string, "length", c_string_size);
  mrb_define_method(static_class_string, "[]", c_string_idx_get);
  mrb_define_method(static_class_string, "to_i", c_string_to_fixnum);
}
