/*! @file
  @brief
  mruby bytecode executor.

  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  Fetch mruby VM bytecodes, decode and execute.

  </pre>
*/

#include <stdio.h>
#include <stdint.h>
#include "vm.h"
#include "static.h"
#include "vm_config.h"
#include "opcode.h"
#include "class.h"
#include "symbol.h"


//================================================================
/*!@brief
  find sym[n] from symbol table in irep

  @param  p
  @param  n
  @return  symbol string
*/
char *find_irep_symbol( void *p, int n )
{
  int cnt = get_int_4(p);
  if( n >= cnt ) return 0;
  p += 4;
  while( n > 0 ) {
    int s = get_int_2(p);
    p += 2+s+1;   // size(2 bytes) + symbol len + '\0'
    n--;
  }
  return p+2;  // skip size(2 bytes)
}






//================================================================
/*!@brief

*/
void not_supported(void)
{
  printf("Not supported!\n");
}



//================================================================
/*!@brief
  Execute NOP

  No operation

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_nop( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  return 0;
}


//================================================================
/*!@brief
  Execute MOVE

  R(A) := R(B)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_move( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  regs[GETARG_A(code)] = regs[GETARG_B(code)];
  return 0;
}


//================================================================
/*!@brief
  Execute LOADL

  R(A) := Pool(Bx)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_loadl( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rb = GETARG_Bx(code);
  mrb_object *ptr = vm->pc_irep->ptr_to_pool;
  while( rb > 0 ){
    ptr = ptr->next;
    rb--;
  }
  regs[GETARG_A(code)] = *ptr;
  return 0;
}


//================================================================
/*!@brief
  Execute LOADI

  R(A) := sBx

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_loadi( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  regs[GETARG_A(code)].value.i = GETARG_sBx(code);
  regs[GETARG_A(code)].tt = MRB_TT_FIXNUM;
  return 0;
}


//================================================================
/*!@brief
  Execute LOADNIL

  R(A) := nil

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_loadnil( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  regs[GETARG_A(code)].tt = MRB_TT_FALSE;
  return 0;
}


//================================================================
/*!@brief
  Execute LOADSELF

  R(A) := self

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_loadself( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  regs[GETARG_A(code)] = regs[0];
  return 0;
}


//================================================================
/*!@brief
  Execute LOADT

  R(A) := true

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_loadt( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  regs[GETARG_A(code)].tt = MRB_TT_TRUE;
  return 0;
}


//================================================================
/*!@brief
  Execute LOADF

  R(A) := false

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_loadf( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  regs[GETARG_A(code)].tt = MRB_TT_FALSE;
  return 0;
}


//================================================================
/*!@brief
  Execute GETGLOBAL

  R(A) := getglobal(Syms(Bx))

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_getglobal( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int ra = GETARG_A(code);
  int rb = GETARG_B(code);
  char *sym = find_irep_symbol(vm->pc_irep->ptr_to_sym, rb);
  mrb_sym sym_id = add_sym(sym);
  regs[ra] = global_object_get(sym_id);
  return 0;
}


//================================================================
/*!@brief
  Execute SETGLOBAL

  setglobal(Syms(Bx), R(A))

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_setglobal( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int ra = GETARG_A(code);
  int rb = GETARG_B(code);
  char *sym = find_irep_symbol(vm->pc_irep->ptr_to_sym, rb);
  mrb_sym sym_id = add_sym(sym);
  global_object_add(sym_id, &regs[ra]);
  return 0;
}


//================================================================
/*!@brief
  Execute JMP

  pc += sBx

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_jmp( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  vm->pc += GETARG_sBx(code) - 1;
  return 0;
}


//================================================================
/*!@brief
  Execute JMPIF

  if R(A) pc += sBx

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_jmpif( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  if( regs[GETARG_A(code)].tt != MRB_TT_FALSE ) {
    vm->pc += GETARG_sBx(code) - 1;
  }
  return 0;
}


//================================================================
/*!@brief
  Execute JMPNOT

  if not R(A) pc += sBx

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_jmpnot( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  if( regs[GETARG_A(code)].tt == MRB_TT_FALSE ) {
    vm->pc += GETARG_sBx(code) - 1;
  }
  return 0;
}


//================================================================
/*!@brief
  Execute SEND

  R(A) := call(R(A),Syms(B),R(A+1),...,R(A+C))

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_send( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  mrb_value recv = regs[GETARG_A(code)];
  int rb = GETARG_B(code);
  char *sym = find_irep_symbol(vm->pc_irep->ptr_to_sym, rb);
  mrb_sym sym_id = str_to_symid(sym);
  mrb_proc *m = find_method(vm, recv, sym_id);

  if( m == 0 ) {
    printf("no method(%s)!\n", sym);
  } else {
    if( m->c_func == 0 ) {
      // Ruby method
      // callinfo
      mrb_callinfo *callinfo = vm->callinfo + vm->callinfo_top;
      callinfo->reg_top = vm->reg_top;
      callinfo->pc_irep = vm->pc_irep;
      callinfo->pc = vm->pc;
      vm->callinfo_top++;
      // target irep
      vm->pc = 0;
      vm->pc_irep = m->func.irep;
      // new regs
      vm->reg_top += GETARG_A(code);
    } else {
      // C func
      m->func.func(vm, regs+GETARG_A(code));
    }
  }
  return 0;
}


//================================================================
/*!@brief
  Execute ENTER

  arg setup according to flags (23=5:5:1:5:5:1:1)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_enter( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  // not implemented....
  return 0;
}


//================================================================
/*!@brief
  Execute RETURN

  return R(A) (B=normal,in-block return/break)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_return( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  // return value
  mrb_value v = regs[GETARG_A(code)];
  regs[0] = v;
  // restore irep,pc,regs
  vm->callinfo_top--;
  mrb_callinfo *callinfo = vm->callinfo + vm->callinfo_top;
  vm->reg_top = callinfo->reg_top;
  vm->pc_irep = callinfo->pc_irep;
  vm->pc = callinfo->pc;
  return 0;
}


//================================================================
/*!@brief
  Execute ADD

  R(A) := R(A)+R(A+1) (Syms[B]=:+,C=1)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_add( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rr = GETARG_A(code);

  // support Fixnum + Fixnum
  if( regs[rr].tt == MRB_TT_FIXNUM && regs[rr+1].tt == MRB_TT_FIXNUM ) {
    regs[rr].value.i += regs[rr+1].value.i;
#if MRUBYC_USE_FLOAT
  } else if( regs[rr].tt == MRB_TT_FLOAT ){
    if( regs[rr+1].tt == MRB_TT_FIXNUM ){
      regs[rr].value.d += regs[rr+1].value.i;
    } else if( regs[rr+1].tt == MRB_TT_FLOAT ){
      regs[rr].value.d += regs[rr+1].value.d;
    } else {
      op_send(vm, code, regs);
    }
#endif
  } else {
    op_send(vm, code, regs);
  }

  return 0;
}


//================================================================
/*!@brief
  Execute ADDI

  R(A) := R(A)+C (Syms[B]=:+)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_addi( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rr = GETARG_A(code);

  // support Fixnum + (value)
  if( regs[rr].tt == MRB_TT_FIXNUM ) {
    regs[rr].value.i += GETARG_C(code);
  } else {
    not_supported();
  }

  return 0;
}


//================================================================
/*!@brief
  Execute ADD

  R(A) := R(A)-R(A+1) (Syms[B]=:-,C=1)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_sub( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rr = GETARG_A(code);

  // support Fixnum + Fixnum
  if( regs[rr].tt == MRB_TT_FIXNUM && regs[rr+1].tt == MRB_TT_FIXNUM ) {
    regs[rr].value.i -= regs[rr+1].value.i;
#if MRUBYC_USE_FLOAT
  } else if( regs[rr].tt == MRB_TT_FLOAT ){
    if( regs[rr+1].tt == MRB_TT_FIXNUM ){
      regs[rr].value.d -= regs[rr+1].value.i;
    } else if( regs[rr+1].tt == MRB_TT_FLOAT ){
      regs[rr].value.d -= regs[rr+1].value.d;
    } else {
      not_supported();
    }
#endif
  } else {
    not_supported();
  }

  return 0;
}


//================================================================
/*!@brief
  Execute

  R(A) := R(A)-C (Syms[B]=:-)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_subi( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rr = GETARG_A(code);

  // support Fixnum + (value)
  if( regs[rr].tt == MRB_TT_FIXNUM ) {
    regs[rr].value.i -= GETARG_C(code);
  } else {
    not_supported();
  }

  return 0;
}


//================================================================
/*!@brief
  Execute EQ

  R(A) := R(A)==R(A+1)  (Syms[B]=:==,C=1)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_eq( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rr = GETARG_A(code);
  int result;

  // support Fixnum + Fixnum
  if( regs[rr].tt == MRB_TT_FIXNUM && regs[rr+1].tt == MRB_TT_FIXNUM ) {
    result = (regs[rr].value.i == regs[rr+1].value.i);
#if MRUBYC_USE_FLOAT
  if( regs[rr].tt == MRB_TT_FLOAT ) {
    if( regs[rr+1].tt == MRB_TT_FIXNUM ){
      result = (regs[rr].value.d == regs[rr+1].value.i );
    } else if( regs[rr+1].tt == MRB_TT_FLOAT ){
      result = (regs[rr].value.d == regs[rr+1].value.d );
    } else {
      result = 0;
    }
  }
#endif
  } else if( regs[rr].tt == MRB_TT_TRUE ){
    result = regs[rr+1].tt == MRB_TT_TRUE;
  } else if( regs[rr].tt == MRB_TT_FALSE ){
    result = regs[rr+1].tt == MRB_TT_FALSE;
  } else {
    result = 0;
    op_send(vm,code,regs);
  }
  if( result ) {
    regs[rr].tt = MRB_TT_TRUE;
  } else {
    regs[rr].tt = MRB_TT_FALSE;
  }

  return 0;
}


//================================================================
/*!@brief
  Execute LT

  R(A) := R(A)<R(A+1)  (Syms[B]=:<,C=1)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_lt( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rr = GETARG_A(code);
  int result;

  // support Fixnum + Fixnum
  if( regs[rr].tt == MRB_TT_FIXNUM && regs[rr+1].tt == MRB_TT_FIXNUM ) {
    result = regs[rr].value.i < regs[rr+1].value.i;
#if MRUBYC_USE_FLOAT
  } else if( regs[rr].tt == MRB_TT_FLOAT ){
    if( regs[rr+1].tt == MRB_TT_FIXNUM ){
      result = regs[rr].value.d < regs[rr+1].value.i;
    } else if( regs[rr+1].tt == MRB_TT_FLOAT ){
      result = regs[rr].value.d < regs[rr+1].value.d;
    } else {
      result = 0;
    }
#endif
  } else {
    result = 0;
    not_supported();
  }

  if( result ) {
    regs[rr].tt = MRB_TT_TRUE;
  } else {
    regs[rr].tt = MRB_TT_FALSE;
  }

  return 0;
}


//================================================================
/*!@brief
  Execute LE

  R(A) := R(A)<=R(A+1)  (Syms[B]=:<=,C=1)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_le( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rr = GETARG_A(code);
  int result;

  // support Fixnum + Fixnum
  if( regs[rr].tt == MRB_TT_FIXNUM && regs[rr+1].tt == MRB_TT_FIXNUM ) {
    result = regs[rr].value.i <= regs[rr+1].value.i;
#if MRUBYC_USE_FLOAT
  } else if( regs[rr].tt == MRB_TT_FLOAT ){
    if( regs[rr+1].tt == MRB_TT_FIXNUM ){
      result = regs[rr].value.d <= regs[rr+1].value.i;
    } else if( regs[rr+1].tt == MRB_TT_FLOAT ){
      result = regs[rr].value.d <= regs[rr+1].value.d;
    } else {
      result = 0;
    }
#endif
  } else {
    result = 0;
    not_supported();
  }
  if( result ) {
    regs[rr].tt = MRB_TT_TRUE;
  } else {
    regs[rr].tt = MRB_TT_FALSE;
  }

  return 0;
}


//================================================================
/*!@brief
  Execute GE

  R(A) := R(A)>=R(A+1) (Syms[B]=:>=,C=1)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_gt( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rr = GETARG_A(code);
  int result;

  // support Fixnum + Fixnum
  if( regs[rr].tt == MRB_TT_FIXNUM && regs[rr+1].tt == MRB_TT_FIXNUM ) {
    result = regs[rr].value.i > regs[rr+1].value.i;
#if MRUBYC_USE_FLOAT
  } else if( regs[rr].tt == MRB_TT_FLOAT ){
    if( regs[rr+1].tt == MRB_TT_FIXNUM ){
      result = regs[rr].value.d > regs[rr+1].value.i;
    } else if( regs[rr+1].tt == MRB_TT_FLOAT ){
      result = regs[rr].value.d > regs[rr+1].value.d;
    } else {
      result = 0;
    }
#endif
  } else {
    result = 0;
    not_supported();
  }
  if( result ) {
    regs[rr].tt = MRB_TT_TRUE;
  } else {
    regs[rr].tt = MRB_TT_FALSE;
  }

  return 0;
}


//================================================================
/*!@brief
  Execute GE

  R(A) := R(A)>=R(A+1) (Syms[B]=:>=,C=1)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_ge( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int rr = GETARG_A(code);
  int result;

  // support Fixnum + Fixnum
  if( regs[rr].tt == MRB_TT_FIXNUM && regs[rr+1].tt == MRB_TT_FIXNUM ) {
    result = regs[rr].value.i >= regs[rr+1].value.i;
#if MRUBYC_USE_FLOAT
  } else if( regs[rr].tt == MRB_TT_FLOAT ){
    if( regs[rr+1].tt == MRB_TT_FIXNUM ){
      result = regs[rr].value.d >= regs[rr+1].value.i;
    } else if( regs[rr+1].tt == MRB_TT_FLOAT ){
      result = regs[rr].value.d >= regs[rr+1].value.d;
    } else {
      result = 0;
    }
#endif
  } else {
    result = 0;
    not_supported();
  }
  if( result ) {
    regs[rr].tt = MRB_TT_TRUE;
  } else {
    regs[rr].tt = MRB_TT_FALSE;
  }

  return 0;
}


//================================================================
/*!@brief
  Make Array

  R(A) := ary_new(R(B),R(B+1)..R(B+C))

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_array( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int arg_a = GETARG_A(code);
  int arg_b = GETARG_B(code);
  int arg_c = GETARG_C(code);
  mrb_object *ptr;

  mrb_value v;
  v.tt = MRB_TT_ARRAY;
  v.value.obj = 0;

  if( arg_c > 0 ){
    ptr = mrb_obj_alloc( regs[arg_b].tt );
    v.value.obj = ptr;
    ptr->value = regs[arg_b].value;
    ptr->next = 0;
    arg_c--;
    arg_b++;

    while( arg_c > 0 ){
      ptr->next = mrb_obj_alloc( regs[arg_b].tt );
      ptr = ptr->next;
      ptr->value = regs[arg_b].value;
      ptr->next = 0;
      arg_c--;
      arg_b++;
    }
  }

  regs[arg_a] = v;

  return 0;
}


//================================================================
/*!@brief
  Execute  LAMBDA

  R(A) := lambda(SEQ[Bz],Cz)

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_lambda( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  // int c = GETARG_C(code); // TODO: Add flags support for OP_LAMBDA
  int b = GETARG_b(code); // sequence position in irep list
  mrb_proc *proc = mrb_rproc_alloc("(lambda)");
  mrb_irep *current = vm->irep;
  mrb_irep *p = current->next; //starting from next for current sequence;
  // code length is p->ilen * sizeof(uint32_t);
  for (int i=0; i < b; i++) {
    p = p->next;
  }
  proc->c_func = 0;
  proc->func.irep = p;
  int a = GETARG_A(code);
  regs[a].tt = MRB_TT_PROC;
  regs[a].value.proc = proc;
  return 0;
}


//================================================================
/*!@brief
  Execute CLASS

    R(A) := newclass(R(A),Syms(B),R(A+1))
    Syms(B): class name
    R(A+1): super class

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_class( mrb_vm *vm, uint32_t code, mrb_value *regs )
{


  return 0;
}




//================================================================
/*!@brief
  Execute METHOD

  R(A).newmethod(Syms(B),R(A+1))

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_method( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  int a = GETARG_A(code);
  mrb_proc *rproc = regs[a+1].value.proc;

  if( regs[a].tt == MRB_TT_CLASS ) {
    mrb_class *cls = regs[a].value.cls;
    int b = GETARG_B(code);
    // sym_id : method name
    mrb_irep *cur_irep = vm->pc_irep;
    char *sym = find_irep_symbol(cur_irep->ptr_to_sym, b);
    int sym_id = add_sym( sym );
    mrb_define_method_proc(cls, sym_id, rproc);
  }

  return 0;
}


//================================================================
/*!@brief
  Execute TCLASS

  R(A) := target_class

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_tclass( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  regs[GETARG_A(code)].tt = MRB_TT_CLASS;
  regs[GETARG_A(code)].value.cls = vm->target_class;

  return 0;
}


//================================================================
/*!@brief
  Execute STOP

  stop VM

  @param  vm    A pointer of VM.
  @param  code  bytecode
  @param  regs  vm->regs + vm->reg_top
  @retval 0  No error.
*/
inline static int op_stop( mrb_vm *vm, uint32_t code, mrb_value *regs )
{
  return -1;
}




//================================================================
/*!@brief


  @param  vm  Pointer of VM.
  @param  irep
  @return
*/
void debug_irep(mrb_vm *vm, mrb_irep *irep)
{
  if( irep->unused == 1 ) {
    printf("  not used.\n");
    return;
  }
  printf("  code:0x%x\n", (int)((char *)irep->code - (char *)vm->mrb));
  printf("  regs:%d\n", irep->nregs);
  printf("  locals:%d\n", irep->nlocals);
}



//================================================================
/*!@brief


  @param  vm  Pointer of VM.
  @param  irep
  @return
*/
mrb_irep *new_irep(void)
{
  mrb_irep *p = static_pool_irep;
  if( p != 0 ){
    static_pool_irep = p->next;
    p->next = 0;
  }
  return p;
}



//================================================================
/*!@brief
  VM initializer.

  Get a VM from static heap.

  @return  Pointer of struct VM in static area.

  @code
  init_static();
  struct VM *vm = vm_open();
  @endcode
*/
struct VM *vm_open(void)
{
  mrb_vm *p = static_pool_vm;
  static_pool_vm = p->next;

  p->priority = 1;
  p->pc = 0;
  p->callinfo_top = 0;
  return p;
}



//================================================================
/*!@brief
  VM finalizer.

  @param  vm  Pointer of VM
*/
void vm_close(struct VM *vm)
{
  vm->priority = -1;
  vm->next = static_pool_vm;
  static_pool_vm = vm;
}



//================================================================
/*!@brief
  Boot the VM.

  @param  vm  Pointer of VM
*/
// init vm
void vm_boot(struct VM *vm)
{
  vm->pc_irep = vm->irep;
  vm->pc = 0;
  vm->reg_top = 0;
  // set self to reg[0]
  vm->top_self = mrb_obj_alloc(MRB_TT_OBJECT);
  vm->top_self->value.cls = static_class_object;
  vm->regs[0].tt = MRB_TT_OBJECT;
  vm->regs[0].value.obj = vm->top_self;
  // target_class
  vm->target_class = vm->top_self->value.cls;
}



//================================================================
/*!@brief
  Fetch a bytecode and execute 1 step.

  @param  vm    A pointer of VM.
  @retval 0  No error.
*/
int vm_run_step( mrb_vm *vm )
{
  int ret = 0;

  // get one bytecode
  uint8_t *p = (uint8_t *)(vm->pc_irep->code+vm->pc*4);
  uint32_t code = *p++;
  code = code << 8 | *p++;
  code = code << 8 | *p++;
  code = code << 8 | *p;

  // next PC
  vm->pc += 1;

  // regs
  mrb_value *regs = vm->regs + vm->reg_top;

  // Dispatch
  enum OPCODE opcode = GET_OPCODE(code);
  switch( opcode ) {
    case OP_NOP:        ret = op_nop       (vm, code, regs); break;
    case OP_MOVE:       ret = op_move      (vm, code, regs); break;
    case OP_LOADL:      ret = op_loadl     (vm, code, regs); break;
    case OP_LOADI:      ret = op_loadi     (vm, code, regs); break;
    case OP_LOADNIL:    ret = op_loadnil   (vm, code, regs); break;
    case OP_LOADSELF:   ret = op_loadself  (vm, code, regs); break;
    case OP_LOADT:      ret = op_loadt     (vm, code, regs); break;
    case OP_LOADF:      ret = op_loadf     (vm, code, regs); break;
    case OP_GETGLOBAL:  ret = op_getglobal (vm, code, regs); break;
    case OP_SETGLOBAL:  ret = op_setglobal (vm, code, regs); break;
    case OP_JMP:        ret = op_jmp       (vm, code, regs); break;
    case OP_JMPIF:      ret = op_jmpif     (vm, code, regs); break;
    case OP_JMPNOT:     ret = op_jmpnot    (vm, code, regs); break;
    case OP_SEND:       ret = op_send      (vm, code, regs); break;
    case OP_ENTER:      ret = op_enter     (vm, code, regs); break;
    case OP_RETURN:     ret = op_return    (vm, code, regs); break;
    case OP_ADD:        ret = op_add       (vm, code, regs); break;
    case OP_ADDI:       ret = op_addi      (vm, code, regs); break;
    case OP_SUB:        ret = op_sub       (vm, code, regs); break;
    case OP_SUBI:       ret = op_subi      (vm, code, regs); break;
    case OP_EQ:         ret = op_eq        (vm, code, regs); break;
    case OP_LT:         ret = op_lt        (vm, code, regs); break;
    case OP_LE:         ret = op_le        (vm, code, regs); break;
    case OP_GT:         ret = op_gt        (vm, code, regs); break;
    case OP_GE:         ret = op_ge        (vm, code, regs); break;
    case OP_ARRAY:      ret = op_array     (vm, code, regs); break;
    case OP_LAMBDA:     ret = op_lambda    (vm, code, regs); break;
    case OP_CLASS:      ret = op_class     (vm, code, regs); break;
    case OP_METHOD:     ret = op_method    (vm, code, regs); break;
    case OP_TCLASS:     ret = op_tclass    (vm, code, regs); break;
    case OP_STOP:       ret = op_stop      (vm, code, regs); break;
    default:
      printf("Skip OP=%02x\n", GET_OPCODE(code));
      break;
  }

  return ret;
}
