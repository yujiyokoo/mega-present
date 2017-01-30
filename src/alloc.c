/*! @file
  @brief
  mrubyc memory management.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Memory management for objects in mruby/c.

  </pre>
*/

#include <stddef.h>
#include <string.h>
#include "alloc.h"
#include "console.h"

//
#define ALLOC_TOTAL_MEMORY_SIZE 0x2800

// address space 16bit, 64KB
#define ALLOC_MAX_BIT 16

// Layer 1st(f) and 2nd(s) model
// last 4bit is ignored
// f : size
// 0 : 0000-007f
// 1 : 0080-00ff
// 2 : 0100-01ff
// 3 : 0200-03ff
// 4 : 0400-07ff
// 5 : 0800-0fff
// 6 : 1000-1fff
// 7 : 2000-3fff
// 8 : 4000-7fff
// 9 : 8000-ffff

#define ALLOC_1ST_LAYER 8
#define ALLOC_1ST_LAYER_BIT ALLOC_1ST_LAYER
#define ALLOC_1ST_LAYER_MASK 0xff80

// 2nd layer size
#define ALLOC_2ND_LAYER 8
#define ALLOC_2ND_LAYER_BIT 3
#define ALLOC_2ND_LAYER_MASK 0x0070

// memory
static uint8_t memory_pool[ALLOC_TOTAL_MEMORY_SIZE];

// define flags
#define FLAG_TAIL_BLOCK 1
#define FLAG_NOT_TAIL_BLOCK 0
#define FLAG_FREE_BLOCK 1
#define FLAG_USED_BLOCK 0

// memory block header
struct USED_BLOCK {
  unsigned int t: 1;  /* FLAG_TAIL_BLOCK or FLAG_NOT_TAIL_BLOCK */
  unsigned int f: 1;  /* FLAG_FREE_BLOCK or BLOCK_IS_NOT_FREE */
  unsigned int size: 14; /* block size, header included */
  struct USED_BLOCK *prev;  /* link to previous block */
  uint8_t data[];
};

struct FREE_BLOCK {
  unsigned int t: 1;  /* 0: not tail,  1: tail */
  unsigned int f: 1;  /* 0: not free,  1: free */
  unsigned int size: 14; /* block size, header included */
  struct FREE_BLOCK *prev;  /* link to previous block */
  struct FREE_BLOCK *next_free;
  struct FREE_BLOCK *prev_free;
};

// free memory bitmap
static struct FREE_BLOCK *free_blocks[ALLOC_1ST_LAYER * ALLOC_2ND_LAYER];


// calc f and s, and returns index of free_blocks
static int calc_index(uint32_t alloc_size)
{
  if( alloc_size < 16 ) alloc_size = 16;

  // 1st layer
  int f = 0;
  uint32_t f_bit = ALLOC_1ST_LAYER_MASK;
  uint32_t s_bit = ALLOC_2ND_LAYER_MASK;
  while( (alloc_size & f_bit) != 0 ){
    f++;
    f_bit <<= 1;
    s_bit <<= 1;
  }

  // 2nd layer
  int s = (alloc_size & s_bit) >> (f + 4);

  return f * ALLOC_2ND_LAYER + s;
}


//
static void add_free_block(struct FREE_BLOCK *block)
{
  block->f = FLAG_FREE_BLOCK;
  int index = calc_index(block->size);

  block->prev_free = NULL;
  block->next_free = free_blocks[index];
  if( free_blocks[index] != NULL ){
    free_blocks[index]->prev_free = block;
  }
  free_blocks[index] = block;
}


// initialize free block
void mrbc_init_alloc(void)
{
  // clear links to free block
  int i;
  for( i=0 ; i<ALLOC_1ST_LAYER*ALLOC_2ND_LAYER ; i++ ){
    free_blocks[i] = NULL;
  }

  // memory pool
  struct FREE_BLOCK *block = (struct FREE_BLOCK *)memory_pool;
  block->t = FLAG_TAIL_BLOCK;
  block->size = ALLOC_TOTAL_MEMORY_SIZE;
  add_free_block(block);
}

// split block *alloc by size
static struct FREE_BLOCK *split_block(struct FREE_BLOCK *alloc, int size)
{
  if( alloc->size < size + 24 ) return NULL;

  // split block, free
  uint8_t *p = (uint8_t *)alloc;
  struct FREE_BLOCK *split = (struct FREE_BLOCK *)(p + size);
  struct FREE_BLOCK *next = (struct FREE_BLOCK *)(p + alloc->size);
  split->size = alloc->size - size;
  split->prev = alloc;
  split->f = FLAG_FREE_BLOCK;
  split->t = alloc->t;
  alloc->size = size;
  alloc->f = FLAG_USED_BLOCK;
  alloc->t = FLAG_NOT_TAIL_BLOCK;
  if( split->t == FLAG_NOT_TAIL_BLOCK ){
    next->prev = split;
  }

  return split;
}


// just remove the free_block *remove from index
static void remove_index(struct FREE_BLOCK *remove)
{
  // remove is top of linked list?
  if( remove->prev_free == NULL ){
    int index = calc_index(remove->size);
    free_blocks[index] = remove->next_free;
    if( free_blocks[index] != NULL ){
      free_blocks[index]->prev = NULL;
    }
  } else {
    remove->prev_free->next_free = remove->next_free;
    if( remove->next_free != NULL ){
      remove->next_free->prev_free = remove->prev_free;
    }
  }
}

// memory allocation
uint8_t *mrbc_raw_alloc(uint32_t size)
{
  uint32_t alloc_size = size + sizeof(struct USED_BLOCK);

  int index = calc_index(alloc_size);
  while( index < ALLOC_1ST_LAYER*ALLOC_2ND_LAYER && free_blocks[index] == NULL ){
    index++;
  }
  if( index >= ALLOC_1ST_LAYER*ALLOC_2ND_LAYER ){
    // out of memory
    console_print("Fatal error: Out of memory.\n");
    return NULL;
  }

  // alloc a free block
  struct FREE_BLOCK *alloc = free_blocks[index];
  alloc->f = FLAG_USED_BLOCK;
  remove_index(alloc);

  // split a block
  struct FREE_BLOCK *release = split_block(alloc, alloc_size);
  if( release != NULL ){
    // split alloc -> alloc + release
    int index = calc_index(release->size);
    release->next_free = free_blocks[index];
    release->prev_free = NULL;
    free_blocks[index] = release;
    if( release->next_free != NULL ){
      release->next_free->prev_free = release;
    }
  }

  return ((struct USED_BLOCK *)alloc)->data;
}


// merge ptr1 and ptr2
// ptr2 will disappear
static void merge(struct FREE_BLOCK *ptr1, struct FREE_BLOCK *ptr2)
{
  // merge ptr1 and ptr2
  ptr1->t = ptr2->t;
  ptr1->size += ptr2->size;

  // update block info
  if( ptr1->t == FLAG_NOT_TAIL_BLOCK ){
    uint8_t *p = (uint8_t *)ptr1;
    struct FREE_BLOCK *next = (struct FREE_BLOCK *)(p + ptr1->size);
    next->prev = ptr1;
  }
}

// memory release
void mrbc_raw_free(void *ptr)
{
  // get free block
  uint8_t *p = ptr;
  struct FREE_BLOCK *free_ptr = (struct FREE_BLOCK *)(p - sizeof(struct USED_BLOCK));
  free_ptr->f = FLAG_FREE_BLOCK;

  // check next block, merge?
  p = (uint8_t *)free_ptr;
  struct FREE_BLOCK *next_ptr = (struct FREE_BLOCK *)(p + free_ptr->size);
  if( free_ptr->t == FLAG_NOT_TAIL_BLOCK && next_ptr->f == FLAG_FREE_BLOCK ){
    remove_index(next_ptr);
    merge(free_ptr, next_ptr);
  }

  // check prev block, merge?
  struct FREE_BLOCK *prev_ptr = free_ptr->prev;
  if( prev_ptr != NULL && prev_ptr->f == FLAG_FREE_BLOCK ){
    remove_index(prev_ptr);
    merge(prev_ptr, free_ptr);
    free_ptr = prev_ptr;
  }

  // free, add to index
  add_free_block(free_ptr);
}


// simple realloc
uint8_t *mrbc_raw_realloc(uint8_t *ptr, uint32_t size)
{
  uint8_t *new_ptr = mrbc_raw_alloc(size);
  if( new_ptr == NULL ) return NULL;  // ENOMEM

  // get block info
  uint8_t *src_ptr = ptr;
  struct USED_BLOCK *src_block = (struct USED_BLOCK *)(src_ptr - sizeof(struct USED_BLOCK));

  // copy size
  int copy_size;
  if( size > src_block->size-sizeof(struct USED_BLOCK) ){
    copy_size = src_block->size - sizeof(struct USED_BLOCK);
  } else {
    copy_size = size;
  }

  // copy and free
  memcpy(new_ptr, src_ptr, copy_size);
  mrbc_raw_free(ptr);

  return new_ptr;
}

// for debug
#ifdef MRBC_DEBUG
#include <stdio.h>
void mrbc_alloc_debug(void)
{
  struct FREE_BLOCK *ptr = (struct FREE_BLOCK *)memory_pool;
  printf("-----\naddress f size\n");
  do {
    uint8_t *p = (uint8_t *)ptr;
    printf("%p: %d %x\n", p, ptr->f, ptr->size);
    if( ptr->t == FLAG_TAIL_BLOCK ) break;
    p += ptr->size;
    ptr = (struct FREE_BLOCK *)p;
  } while(1);

  printf("-----\n");
  int i;
  for( i=0 ; i<ALLOC_1ST_LAYER * ALLOC_2ND_LAYER ; i++ ){
    if( free_blocks[i] == NULL ) continue;
    printf("free[%d]\n", i);
    struct FREE_BLOCK *ptr = free_blocks[i];
    while( ptr != NULL ){
      printf("  %p: size=%d\n", ptr, ptr->size);
      ptr = ptr->next_free;
    }
  }
}
#endif

//// for mruby/c

struct MEM_WITH_VM {
  uint8_t vm_id;
  uint8_t data[];
};

uint8_t *mrbc_alloc(mrb_vm *vm, int size)
{
  int alloc_size = size + sizeof(struct MEM_WITH_VM);
  struct MEM_WITH_VM *alloc_block =
      (struct MEM_WITH_VM *)mrbc_raw_alloc(alloc_size);
  if( alloc_block == NULL ) return NULL;  // ENOMEM

  alloc_block->vm_id = (vm != NULL) ? vm->vm_id : 0;
  return alloc_block->data;
}


uint8_t *mrbc_realloc(mrb_vm *vm, void *ptr, int size)
{
  int alloc_size = size + sizeof(struct MEM_WITH_VM);
  struct MEM_WITH_VM *alloc_block =
      (struct MEM_WITH_VM *)mrbc_raw_realloc(ptr, alloc_size);
  if( alloc_block == NULL ) return NULL;  // ENOMEM

  alloc_block->vm_id = (vm != NULL) ? vm->vm_id : 0;
  return alloc_block->data;
}


void mrbc_free(mrb_vm *vm, void *ptr)
{
  if( ptr == NULL ) return;

  uint8_t *p = (uint8_t *)ptr;
  struct MEM_WITH_VM *free_block = (struct MEM_WITH_VM *)(p - sizeof(struct MEM_WITH_VM));
  mrbc_raw_free(free_block);
}


void mrbc_free_all(mrb_vm *vm)
{
  int vm_id = vm->vm_id;

  struct USED_BLOCK *ptr = (struct USED_BLOCK *)memory_pool;
  while( ptr->t != FLAG_TAIL_BLOCK ){
    struct MEM_WITH_VM *vm_ptr = (struct MEM_WITH_VM *)(ptr->data);
    if( ptr->f == FLAG_FREE_BLOCK || vm_ptr->vm_id != vm_id ){
      uint8_t *p = (uint8_t *)ptr;
      ptr = (struct USED_BLOCK *)(p + ptr->size);
      continue;
    }
    if( vm_ptr->vm_id != vm_id ) continue;
    // free a block
    struct USED_BLOCK *next_ptr = ptr->prev;
    if( next_ptr == NULL ) next_ptr = ptr;
    mrbc_raw_free(ptr->data);
    ptr = next_ptr;
  }  
}
