/*! @file
  @brief
  Declare static data.

  <pre>
  Copyright (C) 2015-2019 Kyushu Institute of Technology.
  Copyright (C) 2015-2019 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.
  </pre>
*/

#ifndef MRBC_SRC_STATIC_H_
#define MRBC_SRC_STATIC_H_


#ifdef __cplusplus
extern "C" {
#endif

// Builtin classes.
struct RClass;
extern struct RClass *mrbc_class_object;
extern struct RClass *mrbc_class_nil;
extern struct RClass *mrbc_class_false;
extern struct RClass *mrbc_class_true;
extern struct RClass *mrbc_class_symbol;
extern struct RClass *mrbc_class_fixnum;
extern struct RClass *mrbc_class_float;
extern struct RClass *mrbc_class_string;
extern struct RClass *mrbc_class_array;
extern struct RClass *mrbc_class_range;
extern struct RClass *mrbc_class_hash;
extern struct RClass *mrbc_class_proc;
extern struct RClass *mrbc_class_math;
extern struct RClass *mrbc_class_exception;

void mrbc_init_static(void);
void mrbc_cleanup_static(void);


// for compatibility.
static inline void init_static(void) {
  mrbc_init_static();
}


#ifdef __cplusplus
}
#endif
#endif
