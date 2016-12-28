#include <stdint.h>
#include "common.h"


int get_int_4(const void *s)
{
  uint8_t *s1 = (uint8_t *)s;
  int ret = *s1++;
  ret = (ret << 8) + *s1++;
  ret = (ret << 8) + *s1++;
  ret = (ret << 8) + *s1;
  return  ret;
}

int get_int_2(const void *s)
{
  uint8_t *s1 = (uint8_t *)s;
  int ret = *s1++;
  return (ret << 8) + *s1;
}
