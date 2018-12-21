/*! @file
  @brief
  Hardware abstraction layer
        for Microchip PIC24

  <pre>
  Copyright (C) 2018 Kyushu Institute of Technology.
  Copyright (C) 2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#ifndef MRBC_SRC_HAL_H_
#define MRBC_SRC_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
#ifndef FCY
#define FCY (_XTAL_FREQ/2)
#endif
#include <xc.h>
#include <libpic30.h>

/***** Local headers ********************************************************/
#include "../mcc_generated_files/clock.h"

/***** Constant values ******************************************************/
/***** Macros ***************************************************************/

#if !defined(MRBC_NO_TIMER)	// use hardware timer.
# define hal_init()        ((void)0)
# define hal_enable_irq()  __builtin_disi(0x0000)
# define hal_disable_irq() __builtin_disi(0x3fff)
# define hal_idle_cpu()    Idle()

#else // MRBC_NO_TIMER
# define hal_init()        ((void)0)
# define hal_enable_irq()  ((void)0)
# define hal_disable_irq() ((void)0)
# define hal_idle_cpu()    ((__delay_ms(1)), mrbc_tick())

#endif


/***** Typedefs *************************************************************/
/***** Global variables *****************************************************/
/***** Function prototypes **************************************************/
int hal_write(int fd, const void *buf, int nbytes);
int hal_flush(int fd);


/***** Inline functions *****************************************************/


#ifdef __cplusplus
}
#endif
#endif // ifndef MRBC_SRC_HAL_H_
