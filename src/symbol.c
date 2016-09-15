#include <string.h>
#include "symbol.h"
#include "static.h"

struct SYM_INDEX {
  uint16_t hash;
  char *pos;
};

static int sym_index_pos;
static struct SYM_INDEX sym_index[MAX_SYMBOLS_COUNT];
static char *sym_table_tail;
static char sym_table[MAX_SYMBOLS_SIZE];

void init_sym(void)
{
  int i;
  for( i=0 ; i<MAX_SYMBOLS_COUNT ; i++ ){
    sym_index[i].pos = 0;
  }
  sym_index_pos = 0;
  sym_table_tail = sym_table;
}

static uint16_t calc_hash(const char *str)
{
  uint16_t h = 0;
  while( *str != '\0' ){
    h = h * 37 + *str;
    str++;
  }
  return h;
}


mrb_sym add_sym(const char *str)
{
  mrb_sym sym_id = str_to_symid(str);
  if( sym_id < 0 ){
    uint16_t h = calc_hash(str);
    sym_index[sym_index_pos].hash = h;
    sym_index[sym_index_pos].pos = sym_table_tail;
    sym_id = sym_index_pos;
    sym_index_pos++;
    strcpy(sym_table_tail, str);
    sym_table_tail += strlen(str)+1;
  }
  return sym_id;
}


mrb_sym str_to_symid(const char *str)
{
  uint16_t h = calc_hash(str);
  int i;
  for( i=0 ; i<sym_index_pos ; i++ ){
    if( sym_index[i].hash == h ){
      if( strcmp(str, sym_index[i].pos) == 0 ){
        return i;
      }
    }
  }
  return -1;
}

const char *symid_to_str(mrb_sym sym_id)
{
  return sym_index[sym_id].pos;
}
