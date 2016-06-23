/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_COMMON_H_
#define MRUBYC_SRC_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif


int my_strcmp(const char *s1, const char *s2);
int my_strlen(const char *s);
void my_strcpy(char *s1, const char *s2);

int check_str_4(char *s1, char *s2);
int get_int_4(void *s);
int get_int_2(void *s);


#ifdef __cplusplus
}
#endif
#endif
