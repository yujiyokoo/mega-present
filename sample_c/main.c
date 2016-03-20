/*
 * Sample Main Program
 */

#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "load.h"
#include "errorcode.h"
#include "static.h"


void mrubyc(char *fn)
{
  struct VM *vm;

  init_static();

  vm = vm_open();
  if( vm == 0 ){
    printf("VM open Error\n");
    return;
  }
  int ret = load_mrb_file(vm, fn);
  if( ret != NO_ERROR ){
    printf("MRB Load Error (%04x_%04x)\n", ret>>16, ret&0xffff);
    return;
  }

  vm_boot( vm );

  int keep_execute = 1;
  while( keep_execute ){
    if( vm_run_step(vm) < 0 ){
      keep_execute = 0;
    }
  }

  vm_close( vm );
}

int main(int argc, char *argv[])
{
  if( argc != 2 ){
    printf("Usage: %s <xxxx.mrb>\n", argv[0]);
    return -1;
  }

  mrubyc(argv[1]);

  return 0;
}
