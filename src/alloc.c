#include <stdio.h>  // this is for debug
#include "alloc.h"

// minimum allocation size
#define _ALLOC_STEP 2

// each page has 0x100 entries of minimum allocation
#define _ALLOC_PAGE_SIZE (_ALLOC_STEP * 0x100)

// number of page
#define _ALLOC_PAGE_NUM 5

// minimum block for free
#define _ALLOC_MIN_FREE (_ALLOC_STEP * 3)


// allocation info in each page
struct _ALLOC_INFO {
  uint8_t owner;     // owner vm id, 0xff:Empty
  uint8_t top_idx;   // pointer to the first allocated block, linked list, 0:NULL 
  uint8_t free_idx;  // free blocks, linked list, 0:NULL
  uint8_t reserved;  // padding
};

// bi-directional linked list 
struct _ALLOC_BLOCK {
  uint8_t next_idx;  // 0:NULL
  uint8_t size;      // size of this block, MALLOC_STEP should be multiplied
  uint16_t count;    // reference count
  uint8_t block[0];  // allocated memory block
};

// heap, all allocatable memory
static uint8_t _heap[_ALLOC_PAGE_SIZE * _ALLOC_PAGE_NUM];




// (page) -> pointer to info 
static inline struct _ALLOC_INFO *page_to_info(uint8_t page)
{
  return (struct _ALLOC_INFO *)(_heap + _ALLOC_PAGE_SIZE * page);
}

// (page,idx) -> pointer to block 
static inline struct _ALLOC_BLOCK *idx_to_block(uint8_t page, uint8_t idx)
{
  return (struct _ALLOC_BLOCK *)(_heap + _ALLOC_PAGE_SIZE * page + idx * _ALLOC_STEP);
}

// (pointer) -> page
static inline uint8_t address_to_page(uint8_t *ptr)
{
  return (ptr - _heap) / _ALLOC_PAGE_SIZE;
}

// (pointer) -> idx
static inline uint8_t address_to_idx(uint8_t *ptr)
{
  int offset = ptr - _heap;
  return ((offset % _ALLOC_PAGE_SIZE) - sizeof(struct _ALLOC_BLOCK)) / _ALLOC_STEP;
}

// init a page
static void _alloc_init_page(int page)
{
  struct _ALLOC_INFO *info = page_to_info(page);
  info->owner = 0xff;
  info->top_idx = 0;
  info->free_idx = sizeof(struct _ALLOC_INFO) / _ALLOC_STEP;
  // first free block
  struct _ALLOC_BLOCK *block = idx_to_block(page, info->free_idx);
  block->next_idx = 0;
  block->size = (_ALLOC_PAGE_SIZE - sizeof(struct _ALLOC_INFO) - sizeof(struct _ALLOC_BLOCK)) / _ALLOC_STEP;		  
}

// init pages
void mrbc_init_alloc(void)
{
  int page;
  for( page=0 ; page<_ALLOC_PAGE_NUM ; page++){
    _alloc_init_page(page);
  }
}

// allocate memory in a page
static uint8_t *_page_alloc(int page, struct _ALLOC_INFO *info, int size)
{
  int alloc_size = (sizeof(struct _ALLOC_BLOCK) + size + _ALLOC_STEP - 1) / _ALLOC_STEP;
  uint8_t *free_idx_p = &info->free_idx;
  uint8_t free_idx = *free_idx_p;
  while( free_idx ){
    struct _ALLOC_BLOCK *block = idx_to_block(page, free_idx);
    if( block->size >= alloc_size ){
      uint8_t next_idx = block->next_idx;
      int free_size = block->size;
      // memory allocate
      struct _ALLOC_BLOCK *allocated = block;
      allocated->next_idx = info->top_idx;
      allocated->count = 1;
      info->top_idx = free_idx;
      if( free_size - alloc_size <= _ALLOC_MIN_FREE ){
	// rest is too small, whole free block is used
	*free_idx_p = next_idx;
      } else {
	// rest is enough, split a block to allocated and free
	allocated->size = alloc_size;
	free_idx += alloc_size;
	struct _ALLOC_BLOCK *freeblock = idx_to_block(page, free_idx);
	*free_idx_p = free_idx;
	freeblock->size = free_size - alloc_size - sizeof(struct _ALLOC_BLOCK)/_ALLOC_STEP;
	freeblock->next_idx = next_idx;
      }
      // return allocated memory
      return allocated->block;
    }
    free_idx_p = &(block->next_idx);
    free_idx = *free_idx_p;
  }
  return NULL;
}

static inline uint8_t get_vm_id(mrb_vm *vm)
{
  if( vm==0 ) return 0;
  return vm->vm_id;
}


// memory allocation
uint8_t *mrbc_alloc(mrb_vm *vm, int size)
{
  int page;
  int vm_id = get_vm_id(vm);
  int empty_page = -1;
  struct _ALLOC_INFO *info;
  for( page=0 ; page<_ALLOC_PAGE_NUM ; page++ ){
    // get info
    info = page_to_info(page);
    // skip empty
    if( info->owner == 0xff ){
      if( empty_page < 0 ) empty_page = page;
      continue;
    }
    // skip not owner
    if( info->owner != vm_id ) continue;
    // try to allocate
    uint8_t *ptr = _page_alloc(page, info, size); 
    if( ptr != NULL ) return ptr;
  }
  // can not allocate from vm's page
  // new page for vm_id
  if( empty_page >= 0 ){
    _alloc_init_page(empty_page);
    info = page_to_info(empty_page);
    info->owner = vm_id;
    return _page_alloc(empty_page, page_to_info(empty_page), size); 
  }

  // out of memory
  return NULL;
}

// free an allocated memory block
// linear search is used...
void mrbc_free(mrb_vm *vm, void *ptr)
{
  int vm_id = get_vm_id(vm);
  uint8_t page = address_to_page(ptr);
  struct _ALLOC_INFO *info = page_to_info(page);
  // check owner
  if( info->owner != vm_id ) return;
  // get target block
  uint8_t target_idx = address_to_idx(ptr);
  uint8_t *idx_p = &info->top_idx;
  uint8_t idx;
  while( (idx = *idx_p) ){
    struct _ALLOC_BLOCK *block = idx_to_block(page, idx);
    if( idx == target_idx ){
      // target block -> free list 
      uint8_t original_free_idx = info->free_idx;
      info->free_idx = idx;
      // used list update
      *(idx_p) = block->next_idx;
      // free list update
      block->next_idx = original_free_idx;
      return;
    }
    idx_p = &(block->next_idx);
  }

  // if here, ptr is a wrong pointer
  return;
}

// bit set
static inline void _page_compact_set_bit(uint8_t map[], uint8_t idx)
{
  uint8_t *p = &map[idx / 8];
  *p |= 0x01 << (idx % 8);
}

// get bit
static inline int _page_compact_get_bit(uint8_t map[], uint8_t idx)
{
  uint8_t *p = &map[idx / 8];
  return  *p & (0x01 << (idx % 8));
}

// compaction a page
static void _page_compact(uint8_t page, struct _ALLOC_INFO *info)
{
  uint8_t map[32];  // free map
  int i;
  // clear bits
  for( i=0 ; i<32 ; i++ ) map[i] = 0x00;
  // set free bits
  uint8_t idx = info->free_idx;
  while( idx ){
    struct _ALLOC_BLOCK *block = idx_to_block(page, idx);
    uint8_t i;
    for( i=0 ; i<block->size ; i++ ){
      _page_compact_set_bit(map, idx+i);
    }
    idx = block->next_idx;
  }
  // find free block, compaction
  info->free_idx = 0;
  idx = 0xff;
  while( 1 ){
    uint8_t top_idx, end_idx;  
    // find bottom of free block
    while( !_page_compact_get_bit(map, idx) ){
      idx--;
      if( idx == 0x00 ) return;
    }
    end_idx = idx--;
    // find top of free block
    while( _page_compact_get_bit(map, idx) ){
      idx--;
      if( idx == 0x00 ) return;
    }
    top_idx = idx+1;
    // free block
    struct _ALLOC_BLOCK *block = idx_to_block(page, top_idx);
    block->next_idx = info->free_idx;
    info->free_idx = top_idx;
    block->size = end_idx - top_idx + 1 - sizeof(struct _ALLOC_BLOCK)/_ALLOC_STEP;
  }
}



// compaction
void mrbc_compact(int vm_id)
{
  int page;
  for( page=0 ; page<_ALLOC_PAGE_NUM ; page++ ){
    struct _ALLOC_INFO *info;
    // get info
    info = page_to_info(page);
    // skip empty
    if( info->owner == 0xff ) continue;
    // skip not owner
    if( info->owner != vm_id ) continue;
    // compaction
    _page_compact(page, info);
  }
}


// reference counter inc
void mrbc_inc_ref(uint8_t *ptr)
{
  if( ptr == NULL ) return;
  uint8_t idx = address_to_idx(ptr);
  uint8_t page = address_to_page(ptr);
  struct _ALLOC_BLOCK *block = idx_to_block(page, idx);
  block->count++;
}

// reference counter dec
void mrbc_dec_ref(uint8_t *ptr)
{
  if( ptr == NULL ) return;
  uint8_t idx = address_to_idx(ptr);
  uint8_t page = address_to_page(ptr);
  struct _ALLOC_BLOCK *block = idx_to_block(page, idx);
  block->count--;
  if( block->count == 0 ){
    // free
    struct _ALLOC_INFO *info = page_to_info(page);
    //    mrbc_free(info->owner, ptr);
  }
}


// for dubug use
void disp_page(int page)
{
  struct _ALLOC_INFO *info = page_to_info(page);
  printf("Page %d ", page);
  if( info->owner == 0xff ){
    printf("(empty)\n");
    return;
  }    
  printf("(owner=%d)\n", info->owner);
  uint8_t idx = info->top_idx;
  printf("* ALOC SIZE COUNT\n");
  while( idx ){
    struct _ALLOC_BLOCK *block = idx_to_block(page, idx);
    printf("   %3d %4d %5d\n", idx, block->size, block->count);
    idx = block->next_idx;
  }
  printf("* FREE SIZE\n");
  idx = info->free_idx;
  while( idx ){
    struct _ALLOC_BLOCK *block = idx_to_block(page, idx);
    printf("   %3d %4d\n", idx, block->size);
    idx = block->next_idx;
  }
}

void disp_memory(void)
{
  int page;
  for( page=0 ; page<_ALLOC_PAGE_NUM ; page++ ){
    disp_page(page);
  }
}

