/*
 * Sample Main Program
 */

#include <stdio.h>
#include <stdlib.h>
#include "alloc.h"
#include "vm.h"
#include "load.h"
#include "errorcode.h"
#include "static.h"

#include "sample01.c"

int main(void)
{
  struct VM *vm;

  mrbc_init_alloc();
  init_static();

  vm = vm_open();
  if( vm == NULL ){
    printf("VM open Error\n");
    return -1;
  }

  int ret = loca_mrb_array(vm, ary);
  if( ret != NO_ERROR ){
    printf("MRB Load Error (%04x_%04x)\n", ret>>16, ret&0xffff);
    return -1;
  }

  vm_boot( vm );
  vm_run( vm );
  vm_close( vm );

  return 0;
}
