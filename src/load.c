#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifdef __MACH__ //to be sure that loader compiles with clang too
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include "vm.h"
#include "vm_config.h"
#include "load.h"
#include "errorcode.h"
#include "static.h"
#include "value.h"


int load_header(struct VM *vm, char **pos)
{
  char *p = *pos;

  if( !check_str_4(p+4, "0003") ){
    return LOAD_FILE_HEADER_ERROR_VERSION;
  }

  /* Ignore CRC */

  /* Ignore size */
  // int sz = get_int_4(p+10);
  // printf("[%d]", sz);

  if( !check_str_4(p+14, "MATZ") ){
    return LOAD_FILE_HEADER_ERROR_MATZ;
  }
  if( !check_str_4(p+18, "0000") ){
    return LOAD_FILE_HEADER_ERROR_VERSION;
  }

  *pos += 22;
  return NO_ERROR;
}

int load_irep(struct VM *vm, char **pos)
{
  char *p = *pos;
  int i;

  p += 4;
  int sz = get_int_4(p);  p += 4;

  if( !check_str_4(p, "0000") ){
    return LOAD_FILE_IREP_ERROR_VERSION;
  }
  p += 4;

  int cnt = 0;
  while( cnt < sz ){
    int sz2 = get_int_4(p);  p += 4;

    // new irep
    mrb_irep *irep = new_irep();
    if( irep == 0 ){
      return LOAD_FILE_IREP_ERROR_ALLOCATION;
    }

    // add irep into vm->irep (at tail)
    // TODO: Optimize this process
    if( vm->irep == 0 ){
      vm->irep = irep;
    } else {
      mrb_irep *p = vm->irep;
      while( p->next != 0 ){
        p = p->next;
      }
      p->next = irep;
    }
    irep->next = 0;

    // nlocals,nregs,rlen
    irep->nlocals = get_int_2(p);  p += 2;
    irep->nregs = get_int_2(p);    p += 2;
    irep->rlen = get_int_2(p);     p += 2;
    irep->ilen = get_int_4(p);     p += 4;

    // padding (align=4 ?)
    p = (char *)( ( (unsigned long)p + 3 ) & ~3L );

    // code
    irep->code = p;
    p += irep->ilen * 4;

    // pool
    irep->ptr_to_pool = 0;
    int plen = get_int_4(p);    p += 4;
    for( i=0 ; i<plen ; i++ ){
      int tt = (int)*p++;
      int obj_size = get_int_2(p);   p += 2;
      mrb_object *ptr = mrb_obj_alloc(MRB_TT_FALSE);
      if( ptr == 0 ){
        return LOAD_FILE_IREP_ERROR_ALLOCATION;
      }
      switch( tt ){
#if MRUBYC_USE_FLOAT
        case 2: { // IREP_TT_FLOAT
          char buf[obj_size+1];
          memcpy(buf, p, obj_size);
          buf[obj_size] = '\0';
          sscanf(buf, "%lf", &ptr->value.d);
          ptr->tt = MRB_TT_FLOAT;
        } break;
#endif
        default:
          break;
      }
      if( irep->ptr_to_pool == 0 ){
        irep->ptr_to_pool = ptr;
      } else {
        mrb_object *pp = irep->ptr_to_pool;
        while( pp->next != 0 ) pp = pp->next;
        pp->next = ptr;
      }
      p += obj_size;
    }

    //  syms
    irep->ptr_to_sym = p;
    int slen = get_int_4(p);    p += 4;
    for( i=0 ; i<slen ; i++ ){
      int s = get_int_2(p);     p += 2;
      p += s+1;
    }

    // cnt
    cnt += sz2 + 8;
  }

  /* TODO: size */
  *pos += sz;
  return NO_ERROR;
}


int load_lvar(struct VM *vm, char **pos)
{
  char *p = *pos;

  /* size */
  int sz = get_int_4(p+4);
  *pos += sz;

  return NO_ERROR;
}


int load_mrb(struct VM *vm)
{
  /* setup mrb program */
  int ret = UNKNOWN_ERROR;
  char *pos = vm->mrb;
  do {
    if( check_str_4(pos, "RITE") ){
      ret = load_header(vm, &pos);
    } else if( check_str_4(pos, "IREP") ){
      ret = load_irep(vm, &pos);
    } else if( check_str_4(pos, "LVAR") ){
      ret = load_lvar(vm, &pos);
    } else if( check_str_4(pos, "END\0") ){
      break;
    } else {
      ret = UNKNOWN_ERROR;
    }
  } while( ret == NO_ERROR );

  return ret;
}


int loca_mrb_array(struct VM *vm, char *ptr)
{
  vm->mrb = ptr;
  return load_mrb(vm);
}

#ifdef MRUBYC_USE_FILEIO
int load_mrb_file(struct VM *vm, char *fn)
{
  /* load to memory */
  FILE *fp = fopen(fn, "rb");
  if( fp==NULL ) return LOAD_FILE_ERROR_NOFILE;
  fseek( fp, 0, SEEK_END );
  int sz = ftell( fp );
  fseek( fp, 0, SEEK_SET );
  vm->mrb = (char *)malloc(sizeof(char)*sz);
  if( vm->mrb == NULL ){
    return LOAD_FILE_ERROR_MALLOC;
  }
  fread(vm->mrb, sizeof(char), sz, fp);
  fclose(fp);

  return load_mrb(vm);
}
#endif
