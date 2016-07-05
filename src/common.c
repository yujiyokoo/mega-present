#include <stdint.h>
#include "common.h"


// simple strcmp
int my_strcmp(const char *s1, const char *s2)
{
  while( (*s1 == *s2) && (*s1 != '\0') ){
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int my_strlen(const char *s)
{
  const char *p = s;
  while( *p ) p++;
  return p-s;
}

void my_strcpy(char *s1, const char *s2)
{
  while( *s2 ){
    *s1 = *s2;
    s1++;
    s2++;
  }
  *s1 = 0;
}

int check_str_4(char *s1, char *s2)
{
  int ret = *s1++ == *s2++;
  ret = ret && (*s1++ == *s2++);
  ret = ret && (*s1++ == *s2++);
  ret = ret && (*s1   == *s2  );
  return ret;
}

int get_int_4(void *s)
{
  uint8_t *s1 = (uint8_t *)s;
  int ret = *s1++;
  ret = (ret << 8) + *s1++;
  ret = (ret << 8) + *s1++;
  ret = (ret << 8) + *s1;
  return  ret;
}

int get_int_2(void *s)
{
  uint8_t *s1 = (uint8_t *)s;
  int ret = *s1++;
  return (ret << 8) + *s1;
}
