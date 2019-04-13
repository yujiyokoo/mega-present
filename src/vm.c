/*! @file
  @brief
  mruby bytecode executor.

  <pre>
  Copyright (C) 2015-2019 Kyushu Institute of Technology.
  Copyright (C) 2015-2019 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Fetch mruby VM bytecodes, decode and execute.

  </pre>
*/

#include "vm_config.h"
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "vm.h"
#include "alloc.h"
#include "load.h"
#include "static.h"
#include "global.h"
#include "opcode.h"
#include "class.h"
#include "symbol.h"
#include "console.h"

#include "c_string.h"
#include "c_range.h"
#include "c_array.h"
#include "c_hash.h"


static uint32_t free_vm_bitmap[MAX_VM_COUNT / 32 + 1];
#define FREE_BITMAP_WIDTH 32
#define Num(n) (sizeof(n)/sizeof((n)[0]))


//================================================================
/*! Number of leading zeros.

  @param	x	target (32bit unsined)
  @retval	int	nlz value
*/
static inline int nlz32(uint32_t x)
{
  if( x == 0 ) return 32;

  int n = 1;
  if((x >> 16) == 0 ) { n += 16; x <<= 16; }
  if((x >> 24) == 0 ) { n +=  8; x <<=  8; }
  if((x >> 28) == 0 ) { n +=  4; x <<=  4; }
  if((x >> 30) == 0 ) { n +=  2; x <<=  2; }
  return n - (x >> 31);
}


//================================================================
/*! cleanup
*/
void mrbc_cleanup_vm(void)
{
  memset(free_vm_bitmap, 0, sizeof(free_vm_bitmap));
}


//================================================================
/*! get sym[n] from symbol table in irep

  @param  p	Pointer to IREP SYMS section.
  @param  n	n th
  @return	symbol name string
*/
const char * mrbc_get_irep_symbol( const uint8_t *p, int n )
{
  int cnt = bin_to_uint32(p);
  if( n >= cnt ) return 0;
  p += 4;
  while( n > 0 ) {
    uint16_t s = bin_to_uint16(p);
    p += 2+s+1;   // size(2 bytes) + symbol len + '\0'
    n--;
  }
  return (char *)p+2;  // skip size(2 bytes)
}


//================================================================
/*! get callee name

  @param  vm	Pointer to VM
  @return	string
*/
const char *mrbc_get_callee_name( struct VM *vm )
{
  // uint32_t code = bin_to_uint32(vm->pc_irep->code + (vm->pc - 1) * 4);
  // int rb = GETARG_B(code);  // index of method sym
  int rb = 0;
  return mrbc_get_irep_symbol(vm->pc_irep->ptr_to_sym, rb);
}


//================================================================
/*!@brief

*/
static void not_supported(void)
{
  console_printf("Not supported!\n");
}


//================================================================
/*! mrbc_irep allocator

  @param  vm	Pointer to VM.
  @return	Pointer to allocated memory or NULL.
*/
mrbc_irep *mrbc_irep_alloc(struct VM *vm)
{
  mrbc_irep *p = (mrbc_irep *)mrbc_alloc(vm, sizeof(mrbc_irep));
  if( p )
    memset(p, 0, sizeof(mrbc_irep));	// caution: assume NULL is zero.
  return p;
}


//================================================================
/*! release mrbc_irep holds memory

  @param  irep	Pointer to allocated mrbc_irep.
*/
void mrbc_irep_free(mrbc_irep *irep)
{
  int i;

  // release pools.
  for( i = 0; i < irep->plen; i++ ) {
    mrbc_raw_free( irep->pools[i] );
  }
  if( irep->plen ) mrbc_raw_free( irep->pools );

  // release child ireps.
  for( i = 0; i < irep->rlen; i++ ) {
    if( irep->reps[i]->ref_count == 0 ) {
      mrbc_irep_free( irep->reps[i] );
    }

  }
  if( irep->rlen ) mrbc_raw_free( irep->reps );

  mrbc_raw_free( irep );
}


//================================================================
/*! Push current status to callinfo stack

*/
void mrbc_push_callinfo( struct VM *vm, mrbc_sym mid, int n_args )
{
  mrbc_callinfo *callinfo = mrbc_alloc(vm, sizeof(mrbc_callinfo));
  if( !callinfo ) return;

  callinfo->current_regs = vm->current_regs;
  callinfo->pc_irep = vm->pc_irep;
  callinfo->pc = vm->pc;
  callinfo->inst = vm->inst;
  callinfo->mid = mid;
  callinfo->n_args = n_args;
  callinfo->target_class = vm->target_class;
  callinfo->prev = vm->callinfo_tail;
  vm->callinfo_tail = callinfo;
}


//================================================================
/*! Pop current status to callinfo stack

*/
void mrbc_pop_callinfo( struct VM *vm )
{
  mrbc_callinfo *callinfo = vm->callinfo_tail;
  vm->callinfo_tail = callinfo->prev;
  vm->current_regs = callinfo->current_regs;
  vm->pc_irep = callinfo->pc_irep;
  vm->pc = callinfo->pc;
  vm->target_class = callinfo->target_class;

  mrbc_free(vm, callinfo);
}





//================================================================
/*!@brief
  Execute OP_NOP

  No operation

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_nop( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_Z();
  return 0;
}


//================================================================
/*!@brief
  Execute OP_MOVE

  R(a) = R(b)

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_move( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_BB();
  mrbc_release(&regs[a]);
  mrbc_dup(&regs[b]);
  regs[a] = regs[b];
  return 0;
}


//================================================================
/*!@brief
  Execute OP_LOADI

  R(a) = mrb_int(b)

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_loadi( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_BB();

  mrbc_release(&regs[a]);
  regs[a] = mrbc_fixnum_value(b);
  return 0;
}




//================================================================
/*!@brief
  Execute OP_LOADI_n (n=-1,0,1..7)

  R(a) = R(a)+mrb_int(n)

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval -1  No error and exit from vm.
*/
static inline int op_loadi_n( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_B();

  // get n
  int opcode = vm->inst[-2];
  int n = opcode - OP_LOADI_0;

  mrbc_release(&regs[a]);
  regs[a] = mrbc_fixnum_value(n);

  return 0;
}



//================================================================
/*!@brief
  Execute OP_LOADSELF

  R(a) = self

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_loadself( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_B();
  
  mrbc_release(&regs[a]);
  mrbc_dup(&regs[0]);
  regs[a] = regs[0];
  return 0;
}



//================================================================
/*!@brief
  Execute OP_SEND

  R(a) = call(R(a),Syms(b),R(a+1),...,R(a+c))

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_send( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_BBB();

  mrbc_value recv = regs[a];

  // Block param
  int bidx = a + c + 1;

  mrbc_release( &regs[bidx] );
  regs[bidx].tt = MRBC_TT_NIL;

  const char *sym_name = mrbc_get_irep_symbol(vm->pc_irep->ptr_to_sym, b);
  mrbc_sym sym_id = str_to_symid(sym_name);
  mrbc_proc *m = find_method(vm, &recv, sym_id);

  if( m == 0 ) {
    mrb_class *cls = find_class_by_object( vm, &recv );
    console_printf("No method. Class:%s Method:%s\n",
		   symid_to_str(cls->sym_id), sym_name );
    return 0;
  }

  // m is C func
  if( m->c_func ) {
    m->func(vm, regs + a, c);
    if( m->func == c_proc_call ) return 0;

    int release_reg = a+1;
    while( release_reg <= bidx ) {
      mrbc_release(&regs[release_reg]);
      release_reg++;
    }
    return 0;
  }

  // m is Ruby method.
  // callinfo
  mrbc_push_callinfo(vm, sym_id, c);

  // target irep
  vm->pc = 0;
  vm->pc_irep = m->irep;
  vm->inst = m->irep->code;

  // new regs
  vm->current_regs += a;

  return 0;
}



//================================================================
/*!@brief
  Execute OP_ENTER

  arg setup according to flags (23=m5:o5:r1:m5:k5:d1:b1)

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_enter( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_W();

  return 0;
}



//================================================================
/*!@brief
  Execute OP_RETURN

  return R(a) (normal)

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_return( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_B();

  mrbc_release(&regs[0]);
  regs[0] = regs[a];
  regs[a].tt = MRBC_TT_EMPTY;
  
  // nregs to release
  int nregs = vm->pc_irep->nregs;

  // restore irep,pc,regs
  mrbc_callinfo *callinfo = vm->callinfo_tail;
  if( callinfo ){
    vm->callinfo_tail = callinfo->prev;
    vm->current_regs = callinfo->current_regs;
    vm->pc_irep = callinfo->pc_irep;
    vm->pc = callinfo->pc;
    vm->inst = callinfo->inst;
    vm->target_class = callinfo->target_class;
  }
  
  // clear stacked arguments
  int i;
  for( i = 1; i < nregs; i++ ) {
    mrbc_release( &regs[i] );
  }

  // release callinfo
  if( callinfo ) mrbc_free(vm, callinfo);
  
  return 0;
}



//================================================================
/*!@brief
  Execute OP_ADDI

  R(a) = R(a)+mrb_int(b)

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_addi( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_BB();

  if( regs[a].tt == MRBC_TT_FIXNUM ) {
    regs[a].i += b;
    return 0;
  }

  #if MRBC_USE_FLOAT
  if( regs[a].tt == MRBC_TT_FLOAT ) {
    regs[a].d += b;
    return 0;
  }
  #endif

  not_supported();

  return 0;
}



//================================================================
/*!@brief
  Execute OP_MUL

  R(a) = R(a)*R(a+1)

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_mul( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_B();

  if( regs[a].tt == MRBC_TT_FIXNUM ) {
    if( regs[a+1].tt == MRBC_TT_FIXNUM ) {     // in case of Fixnum, Fixnum
      regs[a].i *= regs[a+1].i;
      return 0;
    }
    #if MRBC_USE_FLOAT
    if( regs[a+1].tt == MRBC_TT_FLOAT ) {      // in case of Fixnum, Float
      regs[a].tt = MRBC_TT_FLOAT;
      regs[a].d = regs[a].i * regs[a+1].d;
      return 0;
    }
  }
  if( regs[a].tt == MRBC_TT_FLOAT ) {
    if( regs[a+1].tt == MRBC_TT_FIXNUM ) {     // in case of Float, Fixnum
      regs[a].d *= regs[a+1].i;
      return 0;
    }
    if( regs[a+1].tt == MRBC_TT_FLOAT ) {      // in case of Float, Float
      regs[a].d *= regs[a+1].d;
      return 0;
    }
    #endif
  }
  
  // other case
  //op_send(vm, code, regs);
  mrbc_release(&regs[a+1]);

  return 0;
}






//================================================================
/*!@brief
  Execute OP_STRING

  R(a) = str_dup(Lit(b))

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_string( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_BB();

#if MRBC_USE_STRING
  mrbc_object *pool_obj = vm->pc_irep->pools[b];

  /* CAUTION: pool_obj->str - 2. see IREP POOL structure. */
  int len = bin_to_uint16(pool_obj->str - 2);
  mrbc_value value = mrbc_string_new(vm, pool_obj->str, len);
  if( value.string == NULL ) return -1;         // ENOMEM

  mrbc_release(&regs[a]);
  regs[a] = value;

  #else
  not_supported();
  #endif

  return 0;
}



//================================================================
/*!@brief
  Execute OP_METHOD

  R(a) = lambda(SEQ[b],L_METHOD)

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_method( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_BB();

  mrbc_release(&regs[a]);

  // new proc
  mrbc_proc *proc = mrbc_rproc_alloc(vm, "");
  if( !proc ) return 0;	// ENOMEM
  proc->c_func = 0;
  proc->sym_id = -1;
  proc->next = NULL;
  proc->irep = vm->pc_irep->reps[b];

  regs[a].tt = MRBC_TT_PROC;
  regs[a].proc = proc;

  return 0;
}



//================================================================
/*!@brief
  Execute OP_DEF

  R(a).newmethod(Syms(b),R(a+1))

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_def( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_BB();

  assert( regs[a].tt == MRBC_TT_CLASS );
  assert( regs[a+1].tt == MRBC_TT_PROC );

  mrbc_class *cls = regs[a].cls;
  const char *sym_name = mrbc_get_irep_symbol(vm->pc_irep->ptr_to_sym, b);
  mrbc_sym sym_id = str_to_symid(sym_name);

  mrbc_proc *proc = regs[a+1].proc;
  proc->sym_id = sym_id;

#ifdef MRBC_DEBUG
  proc->names = sym_name;
#endif

  proc->ref_count++;
  proc->next = cls->procs;
  cls->procs = proc;

  return 0;
}



//================================================================
/*!@brief
  Execute OP_TCLASS

  R(a) = target_class

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval 0  No error.
*/
static inline int op_tclass( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_B();

  mrbc_release(&regs[a]);
  regs[a].tt = MRBC_TT_CLASS;
  regs[a].cls = vm->target_class;

  return 0;
}



//================================================================
/*!@brief
  Execute OP_STOP

  stop VM

  @param  vm    pointer of VM.
  @param  inst  pointer to instruction
  @param  regs  pointer to regs
  @retval -1  No error and exit from vm.
*/
static inline int op_stop( mrbc_vm *vm, mrbc_value *regs )
{
  FETCH_Z();

  //  if( GET_OPCODE(code) == OP_STOP ) {
  //   int i;
  //  for( i = 0; i < MAX_REGS_SIZE; i++ ) {
  //    mrbc_release(&vm->regs[i]);
  //  }
  // }

  vm->flag_preemption = 1;

  return -1;
}


//================================================================
/*!@brief
  Open the VM.

  @param vm     Pointer to mrbc_vm or NULL.
  @return	Pointer to mrbc_vm.
  @retval NULL	error.
*/
mrbc_vm *mrbc_vm_open( struct VM *vm_arg )
{
  mrbc_vm *vm;
  if( (vm = vm_arg) == NULL ) {
    // allocate memory.
    vm = (mrbc_vm *)mrbc_raw_alloc( sizeof(mrbc_vm) );
    if( vm == NULL ) return NULL;
  }

  // allocate vm id.
  int vm_id = 0;
  int i;
  for( i = 0; i < Num(free_vm_bitmap); i++ ) {
    int n = nlz32( ~free_vm_bitmap[i] );
    if( n < FREE_BITMAP_WIDTH ) {
      free_vm_bitmap[i] |= (1 << (FREE_BITMAP_WIDTH - n - 1));
      vm_id = i * FREE_BITMAP_WIDTH + n + 1;
      break;
    }
  }
  if( vm_id == 0 ) {
    if( vm_arg == NULL ) mrbc_raw_free(vm);
    return NULL;
  }

  // initialize attributes.
  memset(vm, 0, sizeof(mrbc_vm));	// caution: assume NULL is zero.
  if( vm_arg == NULL ) vm->flag_need_memfree = 1;
  vm->vm_id = vm_id;

  return vm;
}



//================================================================
/*!@brief
  Close the VM.

  @param  vm  Pointer to VM
*/
void mrbc_vm_close( struct VM *vm )
{
  // free vm id.
  int i = (vm->vm_id-1) / FREE_BITMAP_WIDTH;
  int n = (vm->vm_id-1) % FREE_BITMAP_WIDTH;
  assert( i < Num(free_vm_bitmap) );
  free_vm_bitmap[i] &= ~(1 << (FREE_BITMAP_WIDTH - n - 1));

  // free irep and vm
  if( vm->irep ) mrbc_irep_free( vm->irep );
  if( vm->flag_need_memfree ) mrbc_raw_free(vm);
}



//================================================================
/*!@brief
  VM initializer.

  @param  vm  Pointer to VM
*/
void mrbc_vm_begin( struct VM *vm )
{
  vm->pc_irep = vm->irep;
  vm->inst = vm->pc_irep->code;
  vm->current_regs = vm->regs;
  memset(vm->regs, 0, sizeof(vm->regs));

  // clear regs
  int i;
  for( i = 1; i < MAX_REGS_SIZE; i++ ) {
    vm->regs[i].tt = MRBC_TT_NIL;
  }

  // set self to reg[0]
  vm->regs[0].tt = MRBC_TT_CLASS;
  vm->regs[0].cls = mrbc_class_object;

  vm->callinfo_tail = NULL;

  // target_class
  vm->target_class = mrbc_class_object;

  vm->error_code = 0;
  vm->flag_preemption = 0;
}


//================================================================
/*!@brief
  VM finalizer.

  @param  vm  Pointer to VM
*/
void mrbc_vm_end( struct VM *vm )
{
  mrbc_global_clear_vm_id();
  mrbc_free_all(vm);
}


//================================================================
/*!@brief
  Fetch a bytecode and execute

  @param  vm    A pointer of VM.
  @retval 0  No error.
*/
int mrbc_vm_run( struct VM *vm )
{
  int ret = 0;

  do {
    // regs
    mrbc_value *regs = vm->current_regs;

    // Dispatch
    uint8_t op = *vm->inst++;

    // for DEBUG
    // console_printf("(OP=%02x)\n", op);

    switch( op ) {
    case OP_NOP:        ret = op_nop       (vm, regs); break;
    case OP_MOVE:       ret = op_move      (vm, regs); break;

    case OP_LOADI:      ret = op_loadi     (vm, regs); break;

    case OP_LOADI__1:
    case OP_LOADI_0:
    case OP_LOADI_1:
    case OP_LOADI_2:
    case OP_LOADI_3:
    case OP_LOADI_4:
    case OP_LOADI_5:
    case OP_LOADI_6:
    case OP_LOADI_7:    ret = op_loadi_n   (vm, regs); break;
      
    case OP_LOADSELF:   ret = op_loadself  (vm, regs); break;

    case OP_SEND:       ret = op_send      (vm, regs); break;

    case OP_ENTER:      ret = op_enter     (vm, regs); break;

    case OP_RETURN:     ret = op_return    (vm, regs); break;
      
    case OP_ADDI:       ret = op_addi      (vm, regs); break;

    case OP_MUL:        ret = op_mul       (vm, regs); break;

    case OP_STRING:     ret = op_string    (vm, regs); break;

    case OP_METHOD:     ret = op_method    (vm, regs); break;

    case OP_DEF:        ret = op_def       (vm, regs); break;

    case OP_TCLASS:     ret = op_tclass    (vm, regs); break;

    case OP_STOP:       ret = op_stop      (vm, regs); break;
      
    default:
      console_printf("Skip OP=%02x\n", op);
      break;
    }
  } while( !vm->flag_preemption );

  vm->flag_preemption = 0;

  return ret;
}
