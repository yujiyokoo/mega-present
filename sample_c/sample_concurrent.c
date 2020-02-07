/*
 * This sample program executes multiple mruby/c programs concurrently.
 */

#include <stdio.h>
#include <stdlib.h>
#include "mrubyc.h"

#define MEMORY_SIZE (1024*60)
static uint8_t memory_pool[MEMORY_SIZE];


uint8_t * load_mrb_file(const char *filename)
{
  FILE *fp = fopen(filename, "rb");

  if( fp == NULL ) {
    fprintf(stderr, "File not found (%s)\n", filename);
    return NULL;
  }

  // get filesize
  fseek(fp, 0, SEEK_END);
  size_t size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // allocate memory
  uint8_t *p = malloc(size);
  if( p != NULL ) {
    fread(p, sizeof(uint8_t), size, fp);
  } else {
    fprintf(stderr, "Memory allocate error.\n");
  }
  fclose(fp);

  return p;
}



int main(int argc, char *argv[])
{
  int i;
  uint8_t *p[MAX_VM_COUNT] = {0};

  mrbc_init(memory_pool, MEMORY_SIZE);

  int vm_cnt = argc-1;
  if( vm_cnt < 1 || vm_cnt > MAX_VM_COUNT ){
    printf("Usage: %s <xxxx.mrb> <xxxx.mrb> ... \n", argv[0]);
    printf("  Maximum number of mrb file: %d\n", MAX_VM_COUNT );
    return 1;
  }

  int flag_error = 0;
  for( i=0 ; i<vm_cnt ; i++ ){
    p[i] = load_mrb_file( argv[i+1] );
    if( p[i] == NULL ){
      flag_error = 1;
      break;
    }

    if( mrbc_create_task( p[i], 0 ) == NULL ) flag_error = 1;
  }

  if( !flag_error ) {
    mrbc_run();
  }

  for ( i=0 ; i<vm_cnt ; i++ ){
    if ( p[i] != NULL ) free( p[i] );
  }

  return 0;
}
