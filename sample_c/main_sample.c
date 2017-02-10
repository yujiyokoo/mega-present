/*
 * Sample Main Program
 */

#include <stdio.h>
#include <stdlib.h>
#include "mrubyc.h"

#include "sample01.c"

#define MEMORY_SIZE (1024*10)
static uint8_t memory_pool[MEMORY_SIZE];

int main(void)
{
  struct VM *vm;

  mrbc_init_alloc(memory_pool, MEMORY_SIZE);
  init_static();

  vm = vm_open();
  if( vm == NULL ){
    printf("VM open Error\n");
    return -1;
  }

  int ret = loca_mrb_array(vm, ary);
  if( ret != 0 ){
    printf("MRB Load Error (%04x_%04x)\n", ret>>16, ret&0xffff);
    return -1;
  }

  vm_boot( vm );
  vm_run( vm );
  vm_close( vm );

  return 0;
}
