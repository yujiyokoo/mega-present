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
  struct VM *vm;

  mrbc_init_alloc(memory_pool, MEMORY_SIZE);
  init_static();

  vm = mrbc_vm_open(NULL);
  if( vm == NULL ){
    printf("VM open Error\n");
    return 1;
  }

  if( mrbc_load_mrb(vm, ary) != 0 ) {
    fprintf(stderr, "Error: Illegal bytecode.\n");
    return 1;
  }

  mrbc_vm_begin(vm);
  mrbc_vm_run(vm);
  mrbc_vm_end(vm);
  mrbc_vm_close(vm);

  return 0;
}
