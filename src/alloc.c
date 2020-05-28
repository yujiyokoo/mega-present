/*! @file
  @brief
  mrubyc memory management.

  <pre>
  Copyright (C) 2015-2020 Kyushu Institute of Technology.
  Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Memory management for objects in mruby/c.

  STRATEGY
   Using TLSF and FistFit algorithm.

  MEMORY BLOCK LINK
      with USED flag and PREV_IN_USE flag in size member's bit 0 and 1.

   |  USED_BLOCK     |  FREE_BLOCK                     |  USED_BLOCK     |...
   |size: (contents) |size,*next,*prev: (empty)   :*top|size: (contents) |
 USED  1:            |   0            :           :    |   1:            |
 PREV  1:            |   1            :           :    |   0:            |

    Sentinel block at the link tail.
      ... |  USED_BLOCK     |
          |size: (contents) |
 USED     |   1:            |
 PREV     |   ?:            |

    size : block size.
    *next: linked list, pointer to the next free block of same block size.
    *prev: linked list, pointer to the previous free block of same block size.
    *top : pointer to this block's top.

  </pre>
*/

#if !defined(MRBC_ALLOC_LIBC)

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
#include "vm_config.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/***** Local headers ********************************************************/
#include "vm.h"
#include "alloc.h"
#include "hal_selector.h"

/***** Constant values ******************************************************/
/*
  Layer 1st(f) and 2nd(s) model
  last 4bit is ignored

 FLI range      SLI0  1     2     3     4     5     6     7         BlockSize
  0  0000-007f  0000- 0010- 0020- 0030- 0040- 0050- 0060- 0070-007f   16
  1  0080-00ff  0080- 0090- 00a0- 00b0- 00c0- 00d0- 00e0- 00f0-00ff   16
  2  0100-01ff  0100- 0120- 0140- 0160- 0180- 01a0- 01c0- 01e0-01ff   32
  3  0200-03ff  0200- 0240- 0280- 02c0- 0300- 0340- 0380- 03c0-03ff   64
  4  0400-07ff  0400- 0480- 0500- 0580- 0600- 0680- 0700- 0780-07ff  128
  5  0800-0fff  0800- 0900- 0a00- 0b00- 0c00- 0d00- 0e00- 0f00-0fff  256
  6  1000-1fff  1000- 1200- 1400- 1600- 1800- 1a00- 1c00- 1e00-1fff  512
  7  2000-3fff  2000- 2400- 2800- 2c00- 3000- 3400- 3800- 3c00-3fff 1024
  8  4000-7fff  4000- 4800- 5000- 5800- 6000- 6800- 7000- 7800-7fff 2048
  9  8000-ffff  8000- 9000- a000- b000- c000- d000- e000- f000-ffff 4096
*/

#ifndef MRBC_ALLOC_FLI_BIT_WIDTH	// 0000 0000 0000 0000
# define MRBC_ALLOC_FLI_BIT_WIDTH 9	// ~~~~~~~~~~~
#endif
#ifndef MRBC_ALLOC_SLI_BIT_WIDTH	// 0000 0000 0000 0000
# define MRBC_ALLOC_SLI_BIT_WIDTH 3	//            ~~~
#endif
#ifndef MRBC_ALLOC_IGNORE_LSBS		// 0000 0000 0000 0000
# define MRBC_ALLOC_IGNORE_LSBS	  4	//                ~~~~
#endif


/***** Macros ***************************************************************/
#define FLI(x) ((x) >> MRBC_ALLOC_SLI_BIT_WIDTH)
#define SLI(x) ((x) & ((1 << MRBC_ALLOC_SLI_BIT_WIDTH) - 1))

/*
   Minimum memory block size parameter.
   Choose large one From sizeof(FREE_BLOCK) or (1 << MRBC_ALLOC_IGNORE_LSBS)
*/
#if !defined(MRBC_MIN_MEMORY_BLOCK_SIZE)
#define MRBC_MIN_MEMORY_BLOCK_SIZE sizeof(FREE_BLOCK)
// #define MRBC_MIN_MEMORY_BLOCK_SIZE (1 << MRBC_ALLOC_IGNORE_LSBS)
#endif


/***** Typedefs *************************************************************/
/*
  define memory block header
*/
#if defined(MRBC_ALLOC_16BIT)
#define MRBC_ALLOC_MEMSIZE_T	uint16_t
typedef struct USED_BLOCK {
  MRBC_ALLOC_MEMSIZE_T size;		//!< block size, header included
#if defined(MRBC_ALLOC_VMID)
  uint8_t	       vm_id;		//!< mruby/c VM ID
#endif
} USED_BLOCK;

typedef struct FREE_BLOCK {
  MRBC_ALLOC_MEMSIZE_T size;		//!< block size, header included
#if defined(MRBC_ALLOC_VMID)
  uint8_t	       vm_id;		//!< dummy
#endif

  struct FREE_BLOCK *next_free;
  struct FREE_BLOCK *prev_free;
  struct FREE_BLOCK *top_adrs;		//!< dummy for calculate sizeof(FREE_BLOCK)
} FREE_BLOCK;


#elif defined(MRBC_ALLOC_24BIT)
#define MRBC_ALLOC_MEMSIZE_T	uint32_t
typedef struct USED_BLOCK {
#if defined(MRBC_ALLOC_VMID)
  MRBC_ALLOC_MEMSIZE_T size : 24;	//!< block size, header included
  uint8_t	       vm_id : 8;	//!< mruby/c VM ID
#else
  MRBC_ALLOC_MEMSIZE_T size;
#endif
} USED_BLOCK;

typedef struct FREE_BLOCK {
#if defined(MRBC_ALLOC_VMID)
  MRBC_ALLOC_MEMSIZE_T size : 24;	//!< block size, header included
  uint8_t	       vm_id : 8;	//!< dummy
#else
  MRBC_ALLOC_MEMSIZE_T size;
#endif

  struct FREE_BLOCK *next_free;
  struct FREE_BLOCK *prev_free;
  struct FREE_BLOCK *top_adrs;		//!< dummy for calculate sizeof(FREE_BLOCK)
} FREE_BLOCK;

#else
# error 'define MRBC_ALLOC_*' required.
#endif

/*
  and operation macro
*/
#define BLOCK_SIZE(p)		(((p)->size) & ~0x03)
#define PHYS_NEXT(p)		((void *)((uint8_t *)(p) + BLOCK_SIZE(p)))
#define SET_USED_BLOCK(p)	((p)->size |=  0x01)
#define SET_FREE_BLOCK(p)	((p)->size &= ~0x01)
#define IS_USED_BLOCK(p)	((p)->size &   0x01)
#define IS_FREE_BLOCK(p)	(!IS_USED_BLOCK(p))
#define SET_PREV_USED(p)	((p)->size |=  0x02)
#define SET_PREV_FREE(p)	((p)->size &= ~0x02)
#define IS_PREV_USED(p)		((p)->size &   0x02)
#define IS_PREV_FREE(p)		(!IS_PREV_USED(p))

#if defined(MRBC_ALLOC_VMID)
#define SET_VM_ID(p,id)	(((USED_BLOCK *)((uint8_t *)(p) - sizeof(USED_BLOCK)))->vm_id = (id))
#define GET_VM_ID(p)	(((USED_BLOCK *)((uint8_t *)(p) - sizeof(USED_BLOCK)))->vm_id)

#else
#define SET_VM_ID(p,id)	((void)0)
#define GET_VM_ID(p)	0
#endif


/***** Function prototypes **************************************************/
/***** Local variables ******************************************************/
// memory pool
static uint8_t *memory_pool;
static MRBC_ALLOC_MEMSIZE_T memory_pool_size;

// free memory block index
#define SIZE_FREE_BLOCKS \
  ((MRBC_ALLOC_FLI_BIT_WIDTH + 1) * (1 << MRBC_ALLOC_SLI_BIT_WIDTH))
static FREE_BLOCK *free_blocks[SIZE_FREE_BLOCKS + 1];

// free memory bitmap
static uint16_t free_fli_bitmap;
static uint8_t  free_sli_bitmap[MRBC_ALLOC_FLI_BIT_WIDTH +1+1]; // + sentinel
#define MSB_BIT1_FLI 0x8000
#define MSB_BIT1_SLI 0x80
#define NLZ_FLI(x) nlz16(x)
#define NLZ_SLI(x) nlz8(x)


/***** Global variables *****************************************************/
/***** Signal catching functions ********************************************/
/***** Local functions ******************************************************/
//================================================================
/*! Number of leading zeros. 16bit version.

  @param  x	target (16bit unsigned)
  @retval int	nlz value
*/
static inline int nlz16(uint16_t x)
{
  if( x == 0 ) return 16;

  int n = 1;
  if((x >>  8) == 0 ) { n += 8; x <<= 8; }
  if((x >> 12) == 0 ) { n += 4; x <<= 4; }
  if((x >> 14) == 0 ) { n += 2; x <<= 2; }
  return n - (x >> 15);
}


//================================================================
/*! Number of leading zeros. 8bit version.

  @param  x	target (8bit unsigned)
  @retval int	nlz value
*/
static inline int nlz8(uint8_t x)
{
  if( x == 0 ) return 8;

  int n = 1;
  if((x >> 4) == 0 ) { n += 4; x <<= 4; }
  if((x >> 6) == 0 ) { n += 2; x <<= 2; }
  return n - (x >> 7);
}


//================================================================
/*! calc f and s, and returns fli,sli of free_blocks

  @param  alloc_size	alloc size
  @retval unsigned int	index of free_blocks
*/
static unsigned int calc_index(unsigned int alloc_size)
{
  // check overflow
  if( (alloc_size >> (MRBC_ALLOC_FLI_BIT_WIDTH
                      + MRBC_ALLOC_SLI_BIT_WIDTH
                      + MRBC_ALLOC_IGNORE_LSBS)) != 0) {
    return SIZE_FREE_BLOCKS;
  }

  // calculate First Level Index.
  int fli = 16 -
    nlz16( alloc_size >> (MRBC_ALLOC_SLI_BIT_WIDTH + MRBC_ALLOC_IGNORE_LSBS) );

  // calculate Second Level Index.
  int shift = (fli == 0) ? MRBC_ALLOC_IGNORE_LSBS :
			  (MRBC_ALLOC_IGNORE_LSBS - 1 + fli);

  int sli   = (alloc_size >> shift) & ((1 << MRBC_ALLOC_SLI_BIT_WIDTH) - 1);
  int index = (fli << MRBC_ALLOC_SLI_BIT_WIDTH) + sli;

  assert(fli >= 0);
  assert(fli <= MRBC_ALLOC_FLI_BIT_WIDTH);
  assert(sli >= 0);
  assert(sli <= (1 << MRBC_ALLOC_SLI_BIT_WIDTH) - 1);

  return index;
}


//================================================================
/*! Mark that block free and register it in the free index table.

  @param  target	Pointer to target block.
*/
static void add_free_block(FREE_BLOCK *target)
{
  SET_FREE_BLOCK(target);

  FREE_BLOCK **top_adrs = (FREE_BLOCK **)((uint8_t*)target + BLOCK_SIZE(target) - sizeof(FREE_BLOCK *));
  *top_adrs = target;

  unsigned int index = calc_index(BLOCK_SIZE(target)) - 1;
  int fli = FLI(index);
  int sli = SLI(index);
  assert( index < SIZE_FREE_BLOCKS );

  free_fli_bitmap      |= (MSB_BIT1_FLI >> fli);
  free_sli_bitmap[fli] |= (MSB_BIT1_SLI >> sli);

  target->prev_free = NULL;
  target->next_free = free_blocks[index];
  if( target->next_free != NULL ) {
    target->next_free->prev_free = target;
  }
  free_blocks[index] = target;

#ifdef MRBC_DEBUG
#if defined(MRBC_ALLOC_VMID)
  target->vm_id = -1;
#endif
  memset( (uint8_t *)target + sizeof(FREE_BLOCK) - sizeof(FREE_BLOCK *), 0xff,
          BLOCK_SIZE(target) - sizeof(FREE_BLOCK) );
#endif
}


//================================================================
/*! just remove the free_block *target from index

  @param  target	pointer to target block.
*/
static void remove_free_block(FREE_BLOCK *target)
{
  // top of linked list?
  if( target->prev_free == NULL ) {
    unsigned int index = calc_index(BLOCK_SIZE(target)) - 1;

    free_blocks[index] = target->next_free;
    if( target->next_free == NULL ) {
      int fli = FLI(index);
      int sli = SLI(index);
      free_sli_bitmap[fli] &= ~(MSB_BIT1_SLI >> sli);
      if( free_sli_bitmap[fli] == 0 ) free_fli_bitmap &= ~(MSB_BIT1_FLI >> fli);
    }
  }
  else {
    target->prev_free->next_free = target->next_free;
  }

  if( target->next_free != NULL ) {
    target->next_free->prev_free = target->prev_free;
  }
}


//================================================================
/*! Split block by size

  @param  target	pointer to target block
  @param  size		size
  @retval NULL		no split.
  @retval FREE_BLOCK *	pointer to splitted free block.
*/
static inline FREE_BLOCK* split_block(FREE_BLOCK *target, unsigned int size)
{
  assert( BLOCK_SIZE(target) >= size );
  if( (BLOCK_SIZE(target) - size) <= MRBC_MIN_MEMORY_BLOCK_SIZE ) return NULL;

  // split block, free
  FREE_BLOCK *split = (FREE_BLOCK *)((uint8_t *)target + size);

  split->size  = BLOCK_SIZE(target) - size;
  target->size = size | (target->size & 0x03);	// copy a size with flags.

  return split;
}


//================================================================
/*! merge target and next block.
    next will disappear

  @param  target	pointer to free block 1
  @param  next	pointer to free block 2
*/
static inline void merge_block(FREE_BLOCK *target, FREE_BLOCK *next)
{
  assert(target < next);

  // merge target and next
  target->size += BLOCK_SIZE(next);		// copy a size but save flags.
}


/***** Global functions *****************************************************/
//================================================================
/*! initialize

  @param  ptr	pointer to free memory block.
  @param  size	size. (max 64KB. see MRBC_ALLOC_MEMSIZE_T)
*/
void mrbc_init_alloc(void *ptr, unsigned int size)
{
  assert( MRBC_MIN_MEMORY_BLOCK_SIZE >= sizeof(FREE_BLOCK) );
  assert( MRBC_MIN_MEMORY_BLOCK_SIZE >= (1 << MRBC_ALLOC_IGNORE_LSBS) );
  assert( size != 0 );
  assert( size <= (MRBC_ALLOC_MEMSIZE_T)(~0) );
  if( memory_pool != NULL ) return;

  size &= ~0x03;
  memory_pool      = ptr;
  memory_pool_size = size;

  // initialize memory pool
  //  large free block + zero size used block (sentinel).
  unsigned int sentinel_size = sizeof(USED_BLOCK);
  sentinel_size += (-sentinel_size & 3);	// align 4 byte.
  unsigned int free_size = memory_pool_size - sentinel_size;

  FREE_BLOCK *free  = (FREE_BLOCK *)memory_pool;
  free->size        = free_size | 0x02;		// flag prev=1, used=0

  USED_BLOCK *used  = (USED_BLOCK *)(memory_pool + free_size);
  used->size        = sentinel_size | 0x01;	// flag prev=0, used=1

  add_free_block(free);
}


//================================================================
/*! cleanup memory pool
*/
void mrbc_cleanup_alloc(void)
{
  memory_pool = NULL;
  memset( free_blocks, 0, sizeof(free_blocks) );
  free_fli_bitmap = 0;
  memset( free_sli_bitmap, 0, sizeof(free_sli_bitmap) );
}


//================================================================
/*! allocate memory sub function.
*/
static inline void * mrbc_raw_alloc_ff_sub(unsigned int alloc_size, unsigned int index)
{
  FREE_BLOCK *target = free_blocks[--index];

  while(1) {
    if( target == NULL ) return NULL;
    if( BLOCK_SIZE(target) >= alloc_size ) break;
    target = target->next_free;
  }

  remove_free_block( target );

  return target;
}


//================================================================
/*! allocate memory

  @param  size	request size.
  @return void * pointer to allocated memory.
  @retval NULL	error.
*/
void * mrbc_raw_alloc(unsigned int size)
{
  unsigned int alloc_size = size + sizeof(USED_BLOCK);

  // align 4 byte
  alloc_size += (-alloc_size & 3);

  // check minimum alloc size.
  if( alloc_size < MRBC_MIN_MEMORY_BLOCK_SIZE ) alloc_size = MRBC_MIN_MEMORY_BLOCK_SIZE;

  // find free memory block.
  unsigned int index = calc_index(alloc_size);
  int fli = FLI(index);
  int sli = SLI(index);

  FREE_BLOCK *target = free_blocks[index];
  if( target != NULL ) goto FOUND_TARGET_BLOCK;

  // check in SLI bitmap table.
  uint16_t masked = free_sli_bitmap[fli] & ((MSB_BIT1_SLI >> sli) - 1);
  if( masked != 0 ) {
    sli = NLZ_SLI( masked );
    goto FOUND_FLI_SLI;
  }

  // check in FLI bitmap table.
  masked = free_fli_bitmap & ((MSB_BIT1_FLI >> fli) - 1);
  if( masked != 0 ) {
    fli = NLZ_FLI( masked );
    sli = NLZ_SLI( free_sli_bitmap[fli] );
    goto FOUND_FLI_SLI;
  }

  // Change strategy to First-fit.
  target = mrbc_raw_alloc_ff_sub( alloc_size, index );
  if( target ) goto SPLIT_BLOCK;

  // else out of memory
  static const char msg[] = "Fatal error: Out of memory.\n";
  hal_write(1, msg, sizeof(msg)-1);
  return NULL;  // ENOMEM


 FOUND_FLI_SLI:
  assert(fli >= 0);
  assert(fli <= MRBC_ALLOC_FLI_BIT_WIDTH);
  assert(sli >= 0);
  assert(sli <= (1 << MRBC_ALLOC_SLI_BIT_WIDTH) - 1);

  index = (fli << MRBC_ALLOC_SLI_BIT_WIDTH) + sli;
  target = free_blocks[index];
  assert( target != NULL );

 FOUND_TARGET_BLOCK:
  assert(BLOCK_SIZE(target) >= alloc_size);

  // remove free_blocks index
  free_blocks[index] = target->next_free;
  if( target->next_free == NULL ) {
    free_sli_bitmap[fli] &= ~(MSB_BIT1_SLI >> sli);
    if( free_sli_bitmap[fli] == 0 ) free_fli_bitmap &= ~(MSB_BIT1_FLI >> fli);
  }
  else {
    target->next_free->prev_free = NULL;
  }

 SPLIT_BLOCK: {
    FREE_BLOCK *release = split_block(target, alloc_size);
    if( release != NULL ) {
      SET_PREV_USED(release);
      add_free_block(release);
    } else {
      FREE_BLOCK *next = PHYS_NEXT(target);
      SET_PREV_USED(next);
    }
  }

  SET_USED_BLOCK(target);
#if defined(MRBC_ALLOC_VMID)
  target->vm_id = 0;
#endif

#ifdef MRBC_DEBUG
  memset( (uint8_t *)target + sizeof(USED_BLOCK), 0xaa,
          BLOCK_SIZE(target) - sizeof(USED_BLOCK) );
#endif

  return (uint8_t *)target + sizeof(USED_BLOCK);
}


//================================================================
/*! allocate memory that cannot free and realloc

  @param  size	request size.
  @return void * pointer to allocated memory.
  @retval NULL	error.
*/
void * mrbc_raw_alloc_no_free(unsigned int size)
{
  unsigned int alloc_size = size + (-size & 3);		// align 4 byte

  // find the tail block
  FREE_BLOCK *tail = (FREE_BLOCK *)memory_pool;
  FREE_BLOCK *prev;
  do {
    prev = tail;
    tail = PHYS_NEXT(tail);
  } while( PHYS_NEXT(tail) < (void *)(memory_pool + memory_pool_size) );

  // can resize it block?
  if( IS_USED_BLOCK(prev) ) goto FALLBACK;
  if( (BLOCK_SIZE(prev) - sizeof(USED_BLOCK)) < size ) goto FALLBACK;

  remove_free_block( prev );
  unsigned int free_size = BLOCK_SIZE(prev) - alloc_size;

  if( free_size <= MRBC_MIN_MEMORY_BLOCK_SIZE ) {
    // no split, use all
    prev->size += BLOCK_SIZE(tail);
    SET_USED_BLOCK( prev );
    tail = prev;
  }
  else {
    // split block
    unsigned int tail_size = tail->size + alloc_size;	// w/ flags.
    tail = (FREE_BLOCK*)((uint8_t *)tail - alloc_size);
    tail->size = tail_size;
    prev->size -= alloc_size;		// w/ flags.
    add_free_block( prev );
  }

  return (uint8_t *)tail + sizeof(USED_BLOCK);

 FALLBACK:
  return mrbc_raw_alloc(alloc_size);
}


//================================================================
/*! release memory

  @param  ptr	Return value of mrbc_raw_alloc()
*/
void mrbc_raw_free(void *ptr)
{
  // get target block
  FREE_BLOCK *target = (FREE_BLOCK *)((uint8_t *)ptr - sizeof(USED_BLOCK));

  // check next block, merge?
  FREE_BLOCK *next = PHYS_NEXT(target);

  if( IS_FREE_BLOCK(next) ) {
    remove_free_block(next);
    merge_block(target, next);
  } else {
    SET_PREV_FREE(next);
  }

  if( IS_PREV_FREE(target) ) {
    FREE_BLOCK *prev = *((FREE_BLOCK **)((uint8_t*)target - sizeof(FREE_BLOCK *)));

    assert( IS_FREE_BLOCK(prev) );
    remove_free_block(prev);
    merge_block(prev, target);
    target = prev;
  }

  // target, add to index
  add_free_block(target);
}


//================================================================
/*! re-allocate memory

  @param  ptr	Return value of mrbc_raw_alloc()
  @param  size	request size
  @return void * pointer to allocated memory.
  @retval NULL	error.
*/
void * mrbc_raw_realloc(void *ptr, unsigned int size)
{
  USED_BLOCK *target = (USED_BLOCK *)((uint8_t *)ptr - sizeof(USED_BLOCK));
  unsigned int alloc_size = size + sizeof(USED_BLOCK);
  FREE_BLOCK *next;

  // align 4 byte
  alloc_size += (-alloc_size & 3);

  // check minimum alloc size.
  if( alloc_size < MRBC_MIN_MEMORY_BLOCK_SIZE ) alloc_size = MRBC_MIN_MEMORY_BLOCK_SIZE;

  // expand? part1.
  // next phys block is free and enough size?
  if( alloc_size > BLOCK_SIZE(target) ) {
    next = PHYS_NEXT(target);
    if( IS_USED_BLOCK(next) ) goto ALLOC_AND_COPY;
    if( (BLOCK_SIZE(target) + BLOCK_SIZE(next)) < alloc_size ) goto ALLOC_AND_COPY;

    remove_free_block(next);
    merge_block((FREE_BLOCK *)target, next);
  }
  next = PHYS_NEXT(target);

  // try shrink.
  FREE_BLOCK *release = split_block((FREE_BLOCK *)target, alloc_size);
  if( release != NULL ) {
    SET_PREV_USED(release);
  } else {
    SET_PREV_USED(next);
    return ptr;
  }

  // check next block, merge?
  if( IS_FREE_BLOCK(next) ) {
    remove_free_block(next);
    merge_block(release, next);
  } else {
    SET_PREV_FREE(next);
  }
  add_free_block(release);
  return ptr;


  // expand part2.
  // new alloc and copy
 ALLOC_AND_COPY: {
    uint8_t *new_ptr = mrbc_raw_alloc(size);
    if( new_ptr == NULL ) return NULL;  // ENOMEM

    memcpy(new_ptr, ptr, BLOCK_SIZE(target) - sizeof(USED_BLOCK));
    SET_VM_ID(new_ptr, target->vm_id);

    mrbc_raw_free(ptr);

    return new_ptr;
  }
}


//================================================================
/*! Check if the pointer points allocated memory.

  @param  tgt	Pointer to check.
  @retval int	result in boolean.
*/
int is_allocated_memory(void *tgt)
{
  // check simply.
  return ((void *)memory_pool <= tgt) &&
    (tgt < (void *)(memory_pool + memory_pool_size));
}


#if defined(MRBC_ALLOC_VMID)
//================================================================
/*! allocate memory

  @param  vm	pointer to VM.
  @param  size	request size.
  @return void * pointer to allocated memory.
  @retval NULL	error.
*/
void * mrbc_alloc(const struct VM *vm, unsigned int size)
{
  uint8_t *ptr = mrbc_raw_alloc(size);
  if( ptr == NULL ) return NULL;	// ENOMEM

  if( vm ) SET_VM_ID(ptr, vm->vm_id);

  return ptr;
}


//================================================================
/*! release memory, vm used.

  @param  vm	pointer to VM.
*/
void mrbc_free_all(const struct VM *vm)
{
  USED_BLOCK *target = (USED_BLOCK *)memory_pool;
  USED_BLOCK *next;
  int vm_id = vm->vm_id;

  while( target < (USED_BLOCK *)(memory_pool + memory_pool_size) ) {
    next = PHYS_NEXT(target);
    if( IS_USED_BLOCK(target) && (target->vm_id == vm_id) ) {
      mrbc_raw_free( (uint8_t *)target + sizeof(USED_BLOCK) );
    }
    target = next;
  }
}


//================================================================
/*! set vm id

  @param  ptr	Return value of mrbc_alloc()
  @param  vm_id	vm id
*/
void mrbc_set_vm_id(void *ptr, int vm_id)
{
  SET_VM_ID(ptr, vm_id);
}


//================================================================
/*! get vm id

  @param  ptr	Return value of mrbc_alloc()
  @return int	vm id
*/
int mrbc_get_vm_id(void *ptr)
{
  return GET_VM_ID(ptr);
}
#endif	// defined(MRBC_ALLOC_VMID)


#if defined(MRBC_DEBUG)
#include "stdio.h"
//================================================================
/*! statistics

  @param  total		returns total memory.
  @param  used		returns used memory.
  @param  free		returns free memory.
  @param  fragmentation	returns memory fragmentation
*/
void mrbc_alloc_statistics(int *total, int *used, int *free, int *fragmentation)
{
  *total = memory_pool_size;
  *used = 0;
  *free = 0;
  *fragmentation = 0;

  USED_BLOCK *block = (USED_BLOCK *)memory_pool;
  int flag_used_free = IS_USED_BLOCK(block);
  while( (uint8_t *)block < (memory_pool + memory_pool_size) ) {
    if( IS_FREE_BLOCK(block) ) {
      *free += BLOCK_SIZE(block);
    } else {
      *used += BLOCK_SIZE(block);
    }
    if( flag_used_free != IS_USED_BLOCK(block) ) {
      (*fragmentation)++;
      flag_used_free = IS_USED_BLOCK(block);
    }
    block = PHYS_NEXT(block);
  }
}


//================================================================
/*! print memory block for debug.

*/
void mrbc_alloc_print_memory_pool( void )
{
  FREE_BLOCK *block = (FREE_BLOCK *)memory_pool;

  while( block < (FREE_BLOCK *)(memory_pool + memory_pool_size) ) {
    printf("%p", block );
#if defined(MRBC_ALLOC_VMID)
    printf(" id:%02x", block->vm_id );
#endif
    printf(" size:%5d+%d(%04x) prv:%d use:%d ",
	   block->size & ~0x03, block->size & 0x03, block->size,
	   !!(block->size & 0x02), !!(block->size & 0x01) );

    if( IS_FREE_BLOCK(block) ) {
      unsigned int index = calc_index(BLOCK_SIZE(block)) - 1;
      printf(" fli:%d sli:%d pf:%p nf:%p",
	     FLI(index), SLI(index), block->prev_free, block->next_free);
    }

    printf( "\n" );
    block = PHYS_NEXT(block);
  }
}

#endif // defined(MRBC_DEBUG)
#endif // !defined(MRBC_ALLOC_LIBC)
