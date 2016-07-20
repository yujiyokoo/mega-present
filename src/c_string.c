#include "c_string.h"

#include "alloc.h"
#include "class.h"
#include "common.h"
#include "static.h"
#include "value.h"



char* mrb_string_dup(const char *str)
{
  int len = my_strlen((char *)str);
  char *ptr = (char *)mrbc_alloc(0, len+1);
  my_strcpy(ptr, str);
  return ptr;
}


char *mrb_string_cat(char *s1, const char *s2)
{
  int len1 = my_strlen(s1);
  int len2 = my_strlen(s2);
  char *ptr = (char *)mrbc_alloc(0, len1+len2+1);
  my_strcpy(ptr, s1);
  my_strcpy(ptr+len1, s2);
  mrbc_free(0, s1);
  return ptr;
}


void mrb_init_class_string(void)
{
  static_class_string = mrb_class_alloc("String", static_class_object);

  
}
