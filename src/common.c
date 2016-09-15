#include <stdint.h>
#include "common.h"


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
