/*
 * Sample Main Program
 */

#include <stdio.h>
#include <stdlib.h>
#include "mrubyc.h"

#include "sample01.c"

#define MEMORY_SIZE (1024*30)
static uint8_t memory_pool[MEMORY_SIZE];

int main(void)
{
  mrbc_init(memory_pool, MEMORY_SIZE);
  
  if( mrbc_create_task(ary, 0) != NULL ){
    mrbc_run();
  }

  return 0;
}
