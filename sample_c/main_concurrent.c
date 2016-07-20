/*
 * Sample Main Program
 */

#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "load.h"
#include "errorcode.h"
#include "static.h"
#include "alloc.h"

int main(int argc, char *argv[])
{
  struct VM *vm[10];

  mrbc_init_alloc();
  init_static();

  int vm_cnt = argc-1;
  if( vm_cnt < 1 || vm_cnt > 10 ){
    printf("Usage: %s <xxxx.mrb> <xxxx.mrb> ... \n", argv[0]);
    return -1;
  }

  int i;
  for( i=0 ; i<vm_cnt ; i++ ){
    vm[i] = vm_open();
    if( vm[i] == 0 ){
      printf("VM[%d] open Error\n", i);
      return -1;
    }
    int ret = load_mrb_file(vm[i], argv[i+1]);
    if( ret != NO_ERROR ){
      printf("MRB Load Error (%04x_%04x)\n", ret>>16, ret&0xffff);
      return -1;
    }
    vm_boot( vm[i] );
  }


  int keep_execute = 1;
  while( keep_execute ){
    keep_execute = 0;
    for( i=0 ; i<vm_cnt ; i++ ){
      if( vm[i] == 0 ) continue;
      if( vm_run_step(vm[i]) < 0 ){
        vm_close( vm[i] );
        vm[i] = 0;
      } else {
        keep_execute = 1;
      }
    }
  }

  return 0;
}
