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

struct RClass;
extern struct RClass *mrbc_class_tbl[];
#define mrbc_class_nil		mrbc_class_tbl[ MRBC_TT_NIL ]
#define mrbc_class_false	mrbc_class_tbl[ MRBC_TT_FALSE ]
#define mrbc_class_true		mrbc_class_tbl[ MRBC_TT_TRUE ]
#define mrbc_class_fixnum	mrbc_class_tbl[ MRBC_TT_FIXNUM ]
#define mrbc_class_float	mrbc_class_tbl[ MRBC_TT_FLOAT ]
#define mrbc_class_symbol	mrbc_class_tbl[ MRBC_TT_SYMBOL ]
#define mrbc_class_proc		mrbc_class_tbl[ MRBC_TT_PROC ]
#define mrbc_class_array	mrbc_class_tbl[ MRBC_TT_ARRAY ]
#define mrbc_class_string	mrbc_class_tbl[ MRBC_TT_STRING ]
#define mrbc_class_range	mrbc_class_tbl[ MRBC_TT_RANGE ]
#define mrbc_class_hash		mrbc_class_tbl[ MRBC_TT_HASH ]

extern struct RClass *mrbc_class_object;
extern struct RClass *mrbc_class_math;
extern struct RClass *mrbc_class_exception;
extern struct RClass *mrbc_class_standarderror;
extern struct RClass *mrbc_class_runtimeerror;
extern struct RClass *mrbc_class_zerodivisionerror;
extern struct RClass *mrbc_class_argumenterror;
extern struct RClass *mrbc_class_indexerror;
extern struct RClass *mrbc_class_typeerror;

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
