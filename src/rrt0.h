/*! @file
  @brief
  Realtime multitask monitor for mruby/c

  <pre>
  Copyright (C) 2016-2017 Kyushu Institute of Technology.
  Copyright (C) 2016-2017 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#ifndef MRBC_SRC_RRT0_H_
#define MRBC_SRC_RRT0_H_

#ifdef __cplusplus
extern "C" {
#endif

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
#include <stdint.h>

/***** Local headers ********************************************************/
#include "vm.h"

/***** Constant values ******************************************************/

//================================================
/*!@brief
  Task state
*/
enum MrbcTaskState {
  TASKSTATE_DOMANT    = 0x00,
  TASKSTATE_READY     = 0x01,
  TASKSTATE_RUNNING   = 0x03,
  TASKSTATE_WAITING   = 0x04,
  TASKSTATE_SUSPENDED = 0x08,
};


/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/

//================================================
/*!@brief
  Task control block
*/
typedef struct MrbcTcb {
  struct MrbcTcb *next;
  uint8_t         priority;
  uint8_t         priority_preemption;
  uint8_t         timeslice;
  uint8_t         state; //!< enum MrbcTaskState
  union {
    uint32_t wakeup_tick;
  };
  struct VM vm;
} MrbcTcb;



/***** Global variables *****************************************************/
/***** Function prototypes **************************************************/
void mrbc_tick(void);
void mrbc_init(uint8_t *ptr, unsigned int size );
void mrbc_init_tcb(MrbcTcb *tcb);
MrbcTcb *mrbc_create_task(const uint8_t *vm_code, MrbcTcb *tcb);
int mrbc_start_task(MrbcTcb *tcb);
int mrbc_run(void);
void mrbc_sleep_ms(MrbcTcb *tcb, uint32_t ms);
void mrbc_relinquish(MrbcTcb *tcb);
void mrbc_change_priority(MrbcTcb *tcb, int priority);
void mrbc_suspend_task(MrbcTcb *tcb);
void mrbc_resume_task(MrbcTcb *tcb);


/***** Inline functions *****************************************************/


#ifdef __cplusplus
}
#endif
#endif // ifndef MRBC_SRC_RRT0_H_
