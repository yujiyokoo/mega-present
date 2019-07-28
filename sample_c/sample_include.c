/*
 * This sample program executes a static mruby/c program.
 * The compiled mruby bytecode file should be prepared by "mrbc" command as followings:
 *
 *    mrbc -E -Bary sample_include_bytecode.rb
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "mrubyc.h"

#include "sample_include_bytecode.c"

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
