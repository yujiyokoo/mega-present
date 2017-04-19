/*! @file
  @brief
  Realtime multitask monitor for mruby/c

  <pre>
  Copyright (C) 2016 Kyushu Institute of Technology.
  Copyright (C) 2016 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
#include <stdint.h>
#include <string.h>
#include <assert.h>


/***** Local headers ********************************************************/
#include "alloc.h"
#include "static.h"
#include "load.h"
#include "class.h"
#include "vm.h"
#include "console.h"
#include "rrt0.h"
#include "hal/hal.h"


/***** Constat values *******************************************************/
const int TIMESLICE_TICK = 10; // 10 * 1ms(HardwareTimer)  255 max


/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
/***** Function prototypes **************************************************/
/***** Local variables ******************************************************/
static MrbcTcb *q_domant_;
static MrbcTcb *q_ready_;
static MrbcTcb *q_waiting_;
static MrbcTcb *q_suspended_;
static volatile uint32_t tick_;

/***** Global variables *****************************************************/
/***** Signal catching functions ********************************************/
/***** Local functions ******************************************************/

//================================================================
/*! Insert to task queue

  @param        Pointer of target TCB

  引数で指定されたタスク(TCB)を、状態別Queueに入れる。
  TCBはフリーの状態でなければならない。（別なQueueに入っていてはならない）
  Queueはpriority_preemption順にソート済みとなる。
  挿入するTCBとQueueに同じpriority_preemption値がある場合は、同値の最後に挿入される。

 */
static void q_insert_task(MrbcTcb *p_tcb)
{
  MrbcTcb **pp_q;

  switch( p_tcb->state ) {
  case TASKSTATE_DOMANT: pp_q = &q_domant_; break;
  case TASKSTATE_READY:
  case TASKSTATE_RUNNING: pp_q   = &q_ready_; break;
  case TASKSTATE_WAITING: pp_q   = &q_waiting_; break;
  case TASKSTATE_SUSPENDED: pp_q = &q_suspended_; break;
  default:
    assert(!"Wrong task state.");
    return;
  }

  // case insert on top.
  if((*pp_q == NULL) ||
     (p_tcb->priority_preemption < (*pp_q)->priority_preemption)) {
    p_tcb->next = *pp_q;
    *pp_q       = p_tcb;
    assert(p_tcb->next != p_tcb);
    return;
  }

  // find insert point in sorted linked list.
  MrbcTcb *p = *pp_q;
  while( 1 ) {
    if((p->next == NULL) ||
       (p_tcb->priority_preemption < p->next->priority_preemption)) {
      p_tcb->next = p->next;
      p->next     = p_tcb;
      assert(p->next != p);
      return;
    }

    p = p->next;
  }
}


//================================================================
/*! Delete from task queue

  @param        Pointer of target TCB

  Queueからタスク(TCB)を取り除く。

 */
static void q_delete_task(MrbcTcb *p_tcb)
{
  MrbcTcb **pp_q;

  switch( p_tcb->state ) {
  case TASKSTATE_DOMANT: pp_q = &q_domant_; break;
  case TASKSTATE_READY:
  case TASKSTATE_RUNNING: pp_q   = &q_ready_; break;
  case TASKSTATE_WAITING: pp_q   = &q_waiting_; break;
  case TASKSTATE_SUSPENDED: pp_q = &q_suspended_; break;
  default:
    assert(!"Wrong task state.");
    return;
  }

  if( *pp_q == NULL ) return;
  if( *pp_q == p_tcb ) {
    *pp_q       = p_tcb->next;
    p_tcb->next = NULL;
    return;
  }

  MrbcTcb *p = *pp_q;
  while( p ) {
    if( p->next == p_tcb ) {
      p->next     = p_tcb->next;
      p_tcb->next = NULL;
      return;
    }

    p = p->next;
  }
}


//================================================================
/*! Find requested task

  @param        Pointer of vm
  @return       Pointer of MrbcTcb. zero is not found.
 */
static inline MrbcTcb* find_requested_task(mrb_vm *vm)
{
  MrbcTcb *tcb;

  for( tcb = q_ready_; tcb != NULL; tcb = tcb->next ) {
    if( tcb->vm == vm ) break;
  }

  return tcb;
}


//================================================================
/*! 一定時間停止（cruby互換）

*/
static void c_sleep(mrb_vm *vm, mrb_value *v)
{
  MrbcTcb *tcb = find_requested_task(vm);

  if( tcb == NULL ) return;

  switch( v[1].tt ) {
  case MRB_TT_FIXNUM:
    mrbc_sleep_ms(tcb, GET_INT_ARG(0) * 1000);
    break;

  case MRB_TT_FLOAT:
    mrbc_sleep_ms(tcb, (uint32_t)(GET_FLOAT_ARG(0) * 1000));
    break;

  default:

    // TODO 引数なしの場合は永久停止
    break;
  }
}


//================================================================
/*! 一定時間停止（ms単位）

*/
static void c_sleep_ms(mrb_vm *vm, mrb_value *v)
{
  MrbcTcb *tcb = find_requested_task(vm);

  if( tcb == NULL ) return;

  mrbc_sleep_ms(tcb, GET_INT_ARG(0));
}


//================================================================
/*! 実行権を手放す

*/
static void c_relinquish(mrb_vm *vm, mrb_value *v)
{
  MrbcTcb *tcb = find_requested_task(vm);

  if( tcb == NULL ) return;

  mrbc_relinquish(tcb);
}


//================================================================
/*! プライオリティー変更

*/
static void c_change_priority(mrb_vm *vm, mrb_value *v)
{
  MrbcTcb *tcb = find_requested_task(vm);

  if( tcb == NULL ) return;

  mrbc_change_priority(tcb, GET_INT_ARG(0));
}


//================================================================
/*! 実行停止

*/
static void c_suspend_task(mrb_vm *vm, mrb_value *v)
{
  MrbcTcb *tcb = find_requested_task(vm);

  if( tcb == NULL ) return;

  mrbc_suspend_task(tcb);
}


//================================================================
/*! 実行再開

*/
static void c_resume_task(mrb_vm *vm, mrb_value *v)
{
  MrbcTcb *tcb = find_requested_task(vm);

  if( tcb == NULL ) return;

  // TODO: 未デバグ。引数で与えられたTCBのタスクを実行再開する。
  mrbc_resume_task(tcb);
}


//================================================================
/*! TCBを得る

*/
static void c_get_tcb(mrb_vm *vm, mrb_value *v)
{
  MrbcTcb *tcb = find_requested_task(vm);

  if( tcb == NULL ) return;

  // TODO: 未実装。TCBポインタをオブジェクトとして返す。
}


/***** Global functions *****************************************************/

//================================================================
/*! Tick timer interrupt handler.

*/
void mrbc_tick(void)
{
  MrbcTcb *tcb;
  int flag_preemption = 0;

  tick_++;

  // 実行中タスクのタイムスライス値を減らす
  tcb = q_ready_;
  if((tcb != NULL) &&
     (tcb->state == TASKSTATE_RUNNING) &&
     (tcb->timeslice > 0)) {
    tcb->timeslice--;
    if( tcb->timeslice == 0 ) tcb->vm->flag_preemption = 1;
  }

  // 待ちタスクキューから、ウェイクアップすべきタスクを探す
  tcb = q_waiting_;
  while( tcb != NULL ) {
    MrbcTcb *t = tcb;
    tcb = tcb->next;

    if( t->wakeup_tick == tick_ ) {
      q_delete_task(t);
      t->state     = TASKSTATE_READY;
      t->timeslice = TIMESLICE_TICK;
      q_insert_task(t);
      flag_preemption = 1;
    }
  }

  if( flag_preemption ) {
    tcb = q_ready_;
    while( tcb != NULL ) {
      if( tcb->state == TASKSTATE_RUNNING ) tcb->vm->flag_preemption = 1;
      tcb = tcb->next;
    }
  }
}


//================================================================
/*! initialize

*/
void mrbc_init(uint8_t *ptr, unsigned int size )
{
  mrbc_init_alloc(ptr, size);
  init_static();
  hal_init();


  // TODO 関数呼び出しが、c_XXX => mrbc_XXX の daisy chain になっている。
  //      不要な複雑さかもしれない。要リファクタリング。
  mrbc_define_method(0, mrbc_class_object, "sleep",           c_sleep);
  mrbc_define_method(0, mrbc_class_object, "sleep_ms",        c_sleep_ms);
  mrbc_define_method(0, mrbc_class_object, "relinquish",      c_relinquish);
  mrbc_define_method(0, mrbc_class_object, "change_priority", c_change_priority);
  mrbc_define_method(0, mrbc_class_object, "suspend_task",    c_suspend_task);
  mrbc_define_method(0, mrbc_class_object, "resume_task",     c_resume_task);
}


//================================================================
/*! specify running VM code.

  @param        vm_code pointer of VM byte code.
  @param        tcb	Task control block with parameter, or NULL.
  @retval       Pointer of MrbcTcb.
  @retval       NULL is error.

*/
MrbcTcb* mrbc_create_task(const uint8_t *vm_code, MrbcTcb *tcb)
{
  // allocate Task Control Block
  if( tcb == NULL ) {
    tcb = (MrbcTcb*)mrbc_raw_alloc( sizeof(MrbcTcb) );
    if( tcb == NULL ) return NULL;	// ENOMEM

    static const MrbcTcb init_val = MRBC_TCB_INITIALIZER;
    *tcb = init_val;
  }
  tcb->timeslice           = TIMESLICE_TICK;
  tcb->priority_preemption = tcb->priority;

  // assign VM on TCB
  if( tcb->state != TASKSTATE_DOMANT ) {
    tcb->vm = mrbc_vm_open();
    if( !tcb->vm ) return 0;    // error. can't open VM.
				// NOTE: memory leak MrbcTcb. but ignore.

    if( mrbc_load_mrb(tcb->vm, vm_code) != 0 ) {
      console_printf("Error: Illegal bytecode.\n");
      mrbc_vm_close(tcb->vm);
      return 0;
    }
    mrbc_vm_begin(tcb->vm);
  }

  hal_disable_irq();
  q_insert_task(tcb);
  hal_enable_irq();

  return tcb;
}


//================================================================
/*! execute

*/
int mrbc_run(void)
{
  while( 1 ) {
    MrbcTcb *tcb = q_ready_;
    if( tcb == NULL ) {
      // 実行すべきタスクなし
      hal_idle_cpu();
      continue;
    }

    // 実行開始
    tcb->state = TASKSTATE_RUNNING;
    int res = 0;

#ifndef MRBC_NO_TIMER
    tcb->vm->flag_preemption = 0;
    res = mrbc_vm_run(tcb->vm);

#else
    while( tcb->timeslice > 0 ) {
      tcb->vm->flag_preemption = 1;
      res = mrbc_vm_run(tcb->vm);
      tcb->timeslice--;
      if( res < 0 ) break;
      if( tcb->state != TASKSTATE_RUNNING ) break;
    }
    mrbc_tick();
#endif /* ifndef MRBC_NO_TIMER */

    // タスク終了？
    if( res < 0 ) {
      hal_disable_irq();
      q_delete_task(tcb);
      tcb->state = TASKSTATE_DOMANT;
      q_insert_task(tcb);
      hal_enable_irq();
      mrbc_vm_end(tcb->vm);
      mrbc_vm_close(tcb->vm);
      tcb->vm = 0;

      if( q_ready_ == NULL && q_waiting_ == NULL &&
          q_suspended_ == NULL ) break;
      continue;
    }

    // タスク切り替え
    hal_disable_irq();
    if( tcb->state == TASKSTATE_RUNNING ) {
      tcb->state = TASKSTATE_READY;

      // タイムスライス終了？
      if( tcb->timeslice == 0 ) {
        q_delete_task(tcb);
        tcb->timeslice = TIMESLICE_TICK;
        q_insert_task(tcb); // insert task on queue last.
      }
    }
    hal_enable_irq();
  }

  return 0;
}


//================================================================
/*! 実行一時停止

*/
void mrbc_sleep_ms(MrbcTcb *tcb, uint32_t ms)
{
  hal_disable_irq();
  q_delete_task(tcb);
  tcb->timeslice   = 0;
  tcb->state       = TASKSTATE_WAITING;
  tcb->wakeup_tick = tick_ + ms;
  q_insert_task(tcb);
  hal_enable_irq();

  tcb->vm->flag_preemption = 1;
}


//================================================================
/*! 実行権を手放す

*/
void mrbc_relinquish(MrbcTcb *tcb)
{
  tcb->timeslice           = 0;
  tcb->vm->flag_preemption = 1;
}


//================================================================
/*! プライオリティーの変更
  TODO: No check, yet.
*/
void mrbc_change_priority(MrbcTcb *tcb, int priority)
{
  tcb->priority            = (uint8_t)priority;
  tcb->priority_preemption = (uint8_t)priority;
  tcb->timeslice           = 0;
  tcb->vm->flag_preemption = 1;
}


//================================================================
/*! 実行停止

*/
void mrbc_suspend_task(MrbcTcb *tcb)
{
  hal_disable_irq();
  q_delete_task(tcb);
  tcb->state = TASKSTATE_SUSPENDED;
  q_insert_task(tcb);
  hal_enable_irq();

  tcb->vm->flag_preemption = 1;
}


//================================================================
/*! 実行再開

*/
void mrbc_resume_task(MrbcTcb *tcb)
{
  hal_disable_irq();

  MrbcTcb *t = q_ready_;
  while( t != NULL ) {
    if( t->state == TASKSTATE_RUNNING ) t->vm->flag_preemption = 1;
    t = t->next;
  }

  q_delete_task(tcb);
  tcb->state = TASKSTATE_READY;
  q_insert_task(tcb);
  hal_enable_irq();
}


#ifdef MRBC_DEBUG

//================================================================
/*! DEBUG print queue

 */
void pq(MrbcTcb *p_tcb)
{
  MrbcTcb *p;

  p = p_tcb;
  while( p != NULL ) {
    console_printf("%08x ", (int)((uint64_t)p & 0xffffffff));
    p = p->next;
  }
  console_printf("\n");

  p = p_tcb;
  while( p != NULL ) {
    console_printf(" pri: %2d ", p->priority_preemption);
    p = p->next;
  }
  console_printf("\n");

  p = p_tcb;
  while( p != NULL ) {
    console_printf(" nx:%04x ", (int)((uint64_t)p->next & 0xffff));
    p = p->next;
  }
  console_printf("\n");
}


void pqall(void)
{
  //  console_printf("<<<<< DOMANT >>>>>\n");
  //  pq(q_domant_);
  console_printf("<<<<< READY >>>>>\n");
  pq(q_ready_);
  console_printf("<<<<< WAITING >>>>>\n");
  pq(q_waiting_);
  console_printf("<<<<< SUSPENDED >>>>>\n");
  pq(q_suspended_);
}
#endif
