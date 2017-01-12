#ifndef MRBC_SRC_SYMBOL_H_
#define MRBC_SRC_SYMBOL_H_

#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif


void init_sym(void);
mrb_sym add_sym(const char *str);
mrb_sym str_to_symid(const char *str);
const char *symid_to_str(mrb_sym sym_id);









#ifdef __cplusplus
}
#endif
#endif
