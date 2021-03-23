/*! @file
  @brief
  Hardware abstraction layer
        for RP2040

  <pre>
  Copyright (C) 2016-2018 Kyushu Institute of Technology.
  Copyright (C) 2016-2018 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
/***** Local headers ********************************************************/
#include "hal.h"

/***** Constat values *******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
/***** Function prototypes **************************************************/
/***** Local variables ******************************************************/
struct repeating_timer timer;

/***** Global variables *****************************************************/
/***** Signal catching functions ********************************************/
/***** Local functions ******************************************************/
/***** Global functions *****************************************************/
#ifndef MRBC_NO_TIMER

//================================================================
/*!@brief
  timer alarm irq

*/
bool alarm_irq(struct repeating_timer *t) {
    mrbc_tick();
}

//================================================================
/*!@brief
  initialize

*/
void hal_init(void){
    add_repeating_timer_ms(1, alarm_irq, NULL, &timer);
    clocks_hw->sleep_en0 = 0;
    clocks_hw->sleep_en1 = CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_BITS | CLOCKS_SLEEP_EN1_CLK_SYS_USBCTRL_BITS | CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_BITS;
}

//================================================================
/*!@brief
  Write

  @param  fd    dummy, but 1.
  @param  buf   pointer of buffer.
  @param  nbytes        output byte length.
*/
int hal_write(int fd, const void *buf, int nbytes){
    printf(buf);
    return nbytes;
}

//================================================================
/*!@brief
  Flush write baffer

  @param  fd    dummy, but 1.
*/
int hal_flush(int fd) {
    return 0;
}

#endif /* ifndef MRBC_NO_TIMER */
