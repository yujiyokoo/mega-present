/*! @file
  @brief


  <pre>
  Copyright (C) 2015 Kyushu Institute of Technology.
  Copyright (C) 2015 Shimane IT Open-Innovation Center.

  This file is distributed under BSD 3-Clause License.


  </pre>
*/

#ifndef MRUBYC_SRC_LOAD_H_
#define MRUBYC_SRC_LOAD_H_

#ifdef __cplusplus
extern "C" {
#endif


int loca_mrb_array(struct VM *vm, char *ptr);
int load_mrb(struct VM *vm);
int load_mrb_file(struct VM *vm, char *fn);


#ifdef __cplusplus
}
#endif
#endif
