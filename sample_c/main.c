/*
 * Sample Main Program
 */

#include <stdio.h>
#include <stdlib.h>
#include "mrubyc.h"

#define MEMORY_SIZE (1024*40)
static uint8_t memory_pool[MEMORY_SIZE];

uint8_t * load_mrb_file(const char *filename)
{
  FILE *fp = fopen(filename, "rb");

  if( fp == NULL ) {
    fprintf(stderr, "File not found\n");
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


void mrubyc(uint8_t *mrbbuf)
{
  mrbc_init(memory_pool, MEMORY_SIZE);

  if( mrbc_create_task(mrbbuf, 0) != NULL ){
    mrbc_run();
  }
}


int main(int argc, char *argv[])
{
  if( argc != 2 ) {
    printf("Usage: %s <xxxx.mrb>\n", argv[0]);
    return 1;
  }

  uint8_t *mrbbuf = load_mrb_file( argv[1] );
  if( mrbbuf == 0 ) return 1;

  mrubyc( mrbbuf );
  free( mrbbuf );

  return 0;
}
