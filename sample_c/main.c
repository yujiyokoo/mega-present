/*
 * Sample Main Program
 */

#include <stdio.h>
#include <stdlib.h>
#include "alloc.h"
#include "static.h"
#include "load.h"
#include "vm.h"
#include "errorcode.h"


int load_mrb_file(struct VM *vm, const char *filename)
{
  FILE *fp = fopen(filename, "rb");

  if( fp == NULL ) {
    fprintf(stderr, "File not found\n");
    return -1;
  }

  // get filesize
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // allocate memory
  uint8_t *p = malloc(size);
  if( p == NULL ) {
    fprintf(stderr, "Memory allocate error.\n");
    return -1;
  }
  fread(p, sizeof(char), size, fp);
  fclose(fp);
  vm->mrb = p;

  // load mruby VM code
  int ret = load_mrb(vm);
  if( ret != NO_ERROR ) {
    fprintf(stderr, "MRB Load Error (%04x_%04x)\n", ret >> 16, ret & 0xffff);
    return -1;
  }

  return 0;
}


void mrubyc(char *fn)
{
  struct VM *vm;

  mrbc_init_alloc();
  init_static();

  vm = vm_open();
  if( vm == 0 ) {
    fprintf(stderr, "VM open Error\n");
    return;
  }
  if( load_mrb_file(vm, fn) != 0 ) return;

  vm_boot(vm);
  vm_run(vm);
  vm_close(vm);
}


int main(int argc, char *argv[])
{
  if( argc != 2 ) {
    printf("Usage: %s <xxxx.mrb>\n", argv[0]);
    return 1;
  }

  mrubyc(argv[1]);

  return 0;
}
