/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_SYSLOG_H_
#define MRUBYC_SRC_SYSLOG_H_

#ifdef __cplusplus
extern "C" {
#endif

  void syslog_putchar(const char c);
  void syslog_print(const char *str);
  void syslog_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif
