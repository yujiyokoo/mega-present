/*! @file
  @brief
  mruby bytecode loader.

  <pre>
  Copyright (C) 2015-2021 Kyushu Institute of Technology.
  Copyright (C) 2015-2021 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.

  </pre>
*/

#ifndef MRBC_SRC_LOAD_H_
#define MRBC_SRC_LOAD_H_

/***** Feature test switches ************************************************/
/***** System headers *******************************************************/
#include <stdint.h>

/***** Local headers ********************************************************/
#include "value.h"

#ifdef __cplusplus
extern "C" {
#endif
/***** Constat values *******************************************************/
/***** Macros ***************************************************************/
// mrbc_irep manipulate macro.
//! get a symbol id table pointer.
#define mrbc_irep_tbl_syms(irep)  ((mrbc_sym *)(irep)->data)

//! get a n'th symbol id in irep
#define mrbc_irep_symbol_id(vm,n) mrbc_irep_tbl_syms((vm)->pc_irep)[(n)]

//! get a n'th symbol string in irep
#define mrbc_irep_symbol_cstr(vm,n) mrbc_symid_to_str( mrbc_irep_symbol_id(vm,n) )


//! get a pool data offset table pointer.
#define mrbc_irep_tbl_pools(irep) ((uint16_t *)(	\
  (irep)->data + sizeof(mrbc_sym) * (irep)->slen ))

//! get a pointer to n'th pool data.
#define mrbc_irep_pool_ptr(vm,n) (			\
  (vm)->pc_irep->pool + mrbc_irep_tbl_pools( (vm)->pc_irep )[(n)] )


//! get a child irep table pointer.
#define mrbc_irep_tbl_ireps(irep) ((mrbc_irep **)(	\
  (irep)->data + sizeof(mrbc_sym) * (irep)->slen	\
	       + sizeof(uint16_t) * (irep)->plen ))

//! get a n'th child irep
#define mrbc_irep_child_irep(vm,n) (			\
  mrbc_irep_tbl_ireps((vm)->pc_irep)[(n)] )


/***** Typedefs *************************************************************/
/***** Global variables *****************************************************/
/***** Function prototypes **************************************************/
int mrbc_load_mrb(struct VM *vm, const uint8_t *bin);
void mrbc_irep_free(struct IREP *irep);
mrbc_value mrbc_irep_pool_value(struct VM *vm, int n);

/***** Inline functions *****************************************************/

#ifdef __cplusplus
}
#endif
#endif
