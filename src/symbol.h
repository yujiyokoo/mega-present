/*! @file
  @brief
  mruby/c Symbol class

  <pre>
  Copyright (C) 2015-2020 Kyushu Institute of Technology.
  Copyright (C) 2015-2020 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_SYMBOL_H_
#define MRBC_SRC_SYMBOL_H_

#ifdef __cplusplus
extern "C" {
#endif

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
/***** Local headers ********************************************************/
#include "value.h"

/***** Constant values ******************************************************/
/***** Macros ***************************************************************/
/***** Typedefs *************************************************************/
/***** Global variables *****************************************************/
/***** Function prototypes **************************************************/
void mrbc_cleanup_symbol(void);
mrbc_sym mrbc_str_to_symid(const char *str);
const char *mrbc_symid_to_str(mrbc_sym sym_id);
mrbc_sym mrbc_search_symid(const char *str);
mrbc_value mrbc_symbol_new(struct VM *vm, const char *str);
void mrbc_init_class_symbol(struct VM *vm);
void mrbc_symbol_statistics(int *total_used);


/***** Inline functions *****************************************************/

//================================================================
/*! get c-language string (char *)
*/
static inline const char * mrbc_symbol_cstr(const mrbc_value *v)
{
  return mrbc_symid_to_str(v->i);
}


// for legacy compatibility.
static inline mrbc_sym str_to_symid(const char *str) {
  return mrbc_str_to_symid(str);
}

static inline const char *symid_to_str(mrbc_sym sym_id) {
  return mrbc_symid_to_str(sym_id);
}


#ifdef __cplusplus
}
#endif
#endif
