/*! @file
  @brief
  Selector of hardware abstraction layer

  You can select hal by defining macro.
  ex) CFLAGS=-DMRBC_USE_HAL_PSOC5LP make

  See also mrubyc/src/Makefile

  <pre>
  Copyright (C) 2016-2020 Kyushu Institute of Technology.
  Copyright (C) 2016-2020 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#if defined(MRBC_USE_HAL_USER_RESERVED)
#include "hal_user_reserved/hal.h"
#elif defined(MRBC_USE_HAL_PSOC5LP)
#include "hal_psoc5lp/hal.h"
#elif defined(MRBC_USE_HAL_ESP32)
#include "hal_esp32/hal.h"
#elif defined(MRBC_USE_HAL_PIC24)
#include "hal_pic24/hal.h"
#elif defined(MRBC_USE_HAL_RP2040)
#include "hal_rp2040/hal.h"
#else
#include "hal_posix/hal.h"
#endif /* MRBC_USE_HAL_xxx */
