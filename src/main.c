#include "mrubyc.h"
#include <stdint.h>
#include <genesis.h>
#include "resources.h"
#include <bmp.h> // drawline

#define int8_t s8

#define VFLIP 1
#define VNOFLIP 0
#define HFLIP 1
#define HNOFLIP 0
#define HIPRIO 1
#define LOPRIO 0

// sound effects
#define SE_TEST 64

typedef char s8;

// extern const char *wordlist[];
extern const char *answerlist[];

// Sprites in code

const u32 top_left[8] =
  {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00011111,
    0x00011111,
    0x00011000,
    0x00011000,
    0x00011000
  };

const u32 horizontal[8] =
  {
    0x00000000,
    0x00000000,
    0x00000000,
    0x11111111,
    0x11111111,
    0x00000000,
    0x00000000,
    0x00000000
  };

const u32 vertical[8] =
  {
    0x00011000,
    0x00011000,
    0x00011000,
    0x00011000,
    0x00011000,
    0x00011000,
    0x00011000,
    0x00011000
  };

const u32 cursor[8] =
  {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000030,
    0x00000333,
    0x00000030
  };

const u32 tick[8] =
  {
    0x00000000,
    0x00000011,
    0x00000110,
    0x00000110,
    0x01101110,
    0x00111100,
    0x00011000,
    0x00000000
  };

const u32 bg_full[8] =
  {
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222
  };

const u32 bg_left[8] =
  {
    0x00002222,
    0x00002222,
    0x00002222,
    0x00002222,
    0x00002222,
    0x00002222,
    0x00002222,
    0x00002222
  };

const u32 bg_top_left[8] =
  {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00002222,
    0x00002222,
    0x00002222,
    0x00002222
  };

const u32 bg_top[8] =
  {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x22222222,
    0x22222222,
    0x22222222,
    0x22222222
  };

const u32 blank[8] =
  {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000
  };

const u32 arrow_r[8] =
  {
    0x00000000,
    0x00001100,
    0x00000110,
    0x11111111,
    0x11111111,
    0x00000110,
    0x00001100,
    0x00000000
  };

const u32 arrow_u[8] =
  {
    0x00011000,
    0x00111100,
    0x01111110,
    0x01011010,
    0x00011000,
    0x00011000,
    0x00011000,
    0x00011000
  };

const u32 t_right[8] =
  {
    0x00011000,
    0x00011000,
    0x00011000,
    0x00011111,
    0x00011111,
    0x00011000,
    0x00011000,
    0x00011000
  };

const u32 t_upright[8] =
  {
    0x00000000,
    0x00000000,
    0x00000000,
    0x11111111,
    0x11111111,
    0x00011000,
    0x00011000,
    0x00011000
  };

enum in_code_tiles { tl, horiz, vert, csr, tck, bgfull, bgl, bgtl, bgt, blnk, arwr, arwu, trght, t, last = t };

void load_tiles() {
  VDP_loadTileData(top_left, TILE_USERINDEX + tl, 1, 0);
  VDP_loadTileData(horizontal, TILE_USERINDEX + horiz, 1, 0);
  VDP_loadTileData(vertical, TILE_USERINDEX + vert, 1, 0);

  VDP_loadTileData(cursor, TILE_USERINDEX + csr, 1, 0);
  VDP_loadTileData(tick, TILE_USERINDEX + tck, 1, 0);
  VDP_loadTileData(bg_full, TILE_USERINDEX + bgfull, 1, 0);
  VDP_loadTileData(bg_left, TILE_USERINDEX + bgl, 1, 0);
  VDP_loadTileData(bg_top_left, TILE_USERINDEX + bgtl, 1, 0);
  VDP_loadTileData(bg_top, TILE_USERINDEX + bgt, 1, 0);
  VDP_loadTileData(blank, TILE_USERINDEX + blnk, 1, 0);
  VDP_loadTileData(arrow_r, TILE_USERINDEX + arwr, 1, 0);
  VDP_loadTileData(arrow_u, TILE_USERINDEX + arwu, 1, 0);
  VDP_loadTileData(t_right, TILE_USERINDEX + trght, 1, 0);
  VDP_loadTileData(t_upright, TILE_USERINDEX + t, 1, 0);
}

// C functions to be called from mruby
static void c_megamrbc_draw_text(mrb_vm *vm, mrb_value *v, int argc)
{
  char *str = mrbc_string_cstr(&v[1]);
  char x = mrbc_integer(v[2]);
  char y = mrbc_integer(v[3]);
  KLog(str);
  VDP_drawText(str, x, y);
}

static void c_megamrbc_draw_top_left(mrb_vm *vm, mrb_value *v, int argc)
{
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_USERINDEX + tl, x, y);
}

static void c_megamrbc_draw_horizontal(mrb_vm *vm, mrb_value *v, int argc)
{
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_USERINDEX + horiz, x, y);
}

static void c_megamrbc_draw_top_right(mrb_vm *vm, mrb_value *v, int argc)
{
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0,HIPRIO,VNOFLIP,HFLIP, TILE_USERINDEX + tl), x, y);
}

static void c_megamrbc_draw_vertical(mrb_vm *vm, mrb_value *v, int argc)
{
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_USERINDEX + vert, x, y);
}

static void c_megamrbc_draw_bottom_left(mrb_vm *vm, mrb_value *v, int argc)
{
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0,HIPRIO,VFLIP,HNOFLIP, TILE_USERINDEX + tl), x, y);
}

static void c_megamrbc_draw_bottom_right(mrb_vm *vm, mrb_value *v, int argc)
{
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(0,HIPRIO,VFLIP,HFLIP, TILE_USERINDEX + tl), x, y);
}

static void c_megamrbc_wait_vblank(mrb_vm *vm, mrb_value *v, int argc) {
  // SPR_update();
  SYS_doVBlankProcess();
}

static void c_megamrbc_show_cursor(mrb_vm *vm, mrb_value *v, int argc) {
  uint8_t x = mrbc_integer(v[1]);
  uint8_t y = mrbc_integer(v[2]);
  uint16_t x_px = x * 8;
  uint16_t y_px = y * 8;
  VDP_setSpriteFull(0, x_px, y_px, SPRITE_SIZE(1,1), TILE_ATTR_FULL(0,HIPRIO,VNOFLIP,HNOFLIP,TILE_USERINDEX + csr), 1);
  VDP_setSpriteFull(1, x_px+16, y_px, SPRITE_SIZE(1,1), TILE_ATTR_FULL(0,HIPRIO,VNOFLIP,HFLIP,TILE_USERINDEX + csr), 2);
  VDP_setSpriteFull(2, x_px, y_px+16, SPRITE_SIZE(1,1), TILE_ATTR_FULL(0,HIPRIO,VFLIP,HNOFLIP,TILE_USERINDEX + vert + 1), 3);
  VDP_setSprite(3, x_px+16, y_px+16, SPRITE_SIZE(1,1), TILE_ATTR_FULL(0,HIPRIO,VFLIP,HFLIP,TILE_USERINDEX + csr));
  VDP_updateSprites(4, 1);
}

static void c_megamrbc_show_tick(mrb_vm *vm, mrb_value *v, int argc) {
  uint8_t x = mrbc_integer(v[1]);
  uint8_t y = mrbc_integer(v[2]);
  uint16_t x_px = (x + 1) * 8;
  uint16_t y_px = (y + 1) * 8;
  VDP_setTileMapXY(BG_B, TILE_USERINDEX + tck, x, y);
}

static void draw_colour_square(uint8_t palette_num, mrb_vm *vm, mrb_value *v, int argc) {
  uint8_t x = mrbc_integer(v[1]);
  uint8_t y = mrbc_integer(v[2]);
  uint16_t x_px = (x + 1) * 8;
  uint16_t y_px = (y + 1) * 8;
  VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette_num, LOPRIO, VNOFLIP, HNOFLIP, TILE_USERINDEX + bgtl), x-1, y-1);
  VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette_num, LOPRIO, VNOFLIP, HNOFLIP, TILE_USERINDEX + bgt), x, y-1);
  VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette_num, LOPRIO, VNOFLIP, HFLIP, TILE_USERINDEX + bgtl), x+1, y-1);
  VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette_num, LOPRIO, VNOFLIP, HNOFLIP, TILE_USERINDEX + bgl), x-1, y);
  VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette_num, LOPRIO, VNOFLIP, HNOFLIP, TILE_USERINDEX + bgfull), x, y);
  VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette_num, LOPRIO, VNOFLIP, HFLIP, TILE_USERINDEX + bgl), x+1, y);
  VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette_num, LOPRIO, VFLIP, HNOFLIP, TILE_USERINDEX + bgtl), x-1, y+1);
  VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette_num, LOPRIO, VFLIP, HNOFLIP, TILE_USERINDEX + bgt), x, y+1);
  VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(palette_num, LOPRIO, VFLIP, HFLIP, TILE_USERINDEX + bgtl), x+1, y+1);
}

static void c_megamrbc_draw_green_square(mrb_vm *vm, mrb_value *v, int argc) {
  draw_colour_square(0, vm, v, argc);
}

static void c_megamrbc_draw_yellow_square(mrb_vm *vm, mrb_value *v, int argc) {
  draw_colour_square(1, vm, v, argc);
}

static void c_megamrbc_draw_grey_square(mrb_vm *vm, mrb_value *v, int argc) {
  draw_colour_square(2, vm, v, argc);
}

static void c_megamrbc_clear_screen(mrb_vm *vm, mrb_value *v, int argc) {
  uint8_t x = 0;
  uint8_t y = 0;

// some "base" colours
  PAL_setColor(15, 0x0FFF); // default text colour
  PAL_setColor(31, 0x02F2); // TODO: sort out colour setting
  VDP_setTextPalette(0);
  PAL_setColor(0, 0x0000);

  // also reset scroll offset
  VDP_setHorizontalScroll(BG_B, 0);
  VDP_setHorizontalScroll(BG_A, 0);

  for(y = 0; y < 28; y++) {
    for(x = 0; x < 40; x++) {
      VDP_setTileMapXY(BG_B, TILE_USERINDEX + blnk, x, y);
      VDP_setTileMapXY(BG_A, TILE_USERINDEX + blnk, x, y);
    }
  }
}

/*
static void c_megamrbc_is_word(mrb_vm *vm, mrb_value *v, int argc)
{
  char *word = mrbc_string_cstr(&v[1]);
  char **words = wordlist;
  int isWord = 0;
  uint8_t value = 0;

  while(*words != NULL) {
    if(strncmp(*words, word, 5) == 0) {
      value = 1;
    }
    words++;
  }
  SET_BOOL_RETURN(value);
}
*/

static void c_megamrbc_random_answer(mrb_vm *vm, mrb_value *v, int argc) {
  uint16_t len = 686; // hardcoded answer list size for now

  uint16_t idx = rand() % len;

  // Uncomment if you want to show answer
  // VDP_drawText(answerlist[idx], 0, 11);

  SET_RETURN( mrbc_string_new_cstr( vm, answerlist[idx] ) );
}

static void c_megamrbc_call_rand(mrb_vm *vm, mrb_value *v, int argc) {
  rand();
}

static void c_test_func(mrb_vm *vm, mrb_value *v, int argc) {
  PAL_setPaletteDMA(PAL0, sky_bg.palette->data);
  VDP_drawImageEx(
    BG_B, &sky_bg,
    TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, TILE_USERINDEX + last + 1),
    0, 0, FALSE, TRUE
  );

  PAL_setPaletteDMA(PAL1, main_logo.palette->data);
  VDP_drawImageEx(
    BG_A, &main_logo,
    TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, TILE_USERINDEX + last + 1 + sky_bg.tileset->numTile),
    5, 10, FALSE, TRUE
  );

  // reset_text_colours();

/*
  PAL_setPaletteDMA(PAL2, main_logo.palette->data);
  Sprite* logo0 = SPR_addSprite(&main_logo,0,0,TILE_ATTR(PAL2,0, FALSE, FALSE));
  Sprite* logo1 = SPR_addSprite(&main_logo,40,0,TILE_ATTR(PAL2,0, FALSE, FALSE));
  */
}

// currently unused
/*
static void c_draw_tile(mrb_vm *vm, mrb_value *v, int argc) {
  VDP_drawText("writing tile", 1, 1);
  VDP_loadTileSet(bgtile.tileset,1,DMA);
  VDP_setPalette(PAL1, bgtile.palette->data);
  VDP_setTileMapXY(BG_B,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,1),2,2);
}
*/

static void c_megamrbc_read_content(mrb_vm *vm, mrb_value *v, int argc) {
  SET_RETURN( mrbc_string_new_cstr( vm, content ) );
}

static void c_megamrbc_set_pal_colour(mrb_vm *vm, mrb_value *v, int argc) {
  uint16_t colour_id = mrbc_integer(v[1]);
  uint16_t colour_val = mrbc_integer(v[2]);
  char buf[40];
  // sprintf(buf, "colour_id: %d, colour_val: %d", colour_id, colour_val);
  // KLog(buf);
  PAL_setColor(colour_id, colour_val);
}

static void c_megamrbc_set_txt_pal(mrb_vm *vm, mrb_value *v, int argc) {
  uint8_t pal = mrbc_integer(v[1]);
  VDP_setTextPalette(pal);
}

static void c_megamrbc_read_content_line(mrb_vm *vm, mrb_value *v, int argc) {
  char buf[201];
  char *buf_cpy = buf;
  static char *content_ptr = content;
  static int turn = 0;

  // FIXME: currently line is limited to 200 chars max
  memcpy(buf, content_ptr, 200);
  while(*content_ptr != '\0' && *content_ptr != '\r' && *content_ptr != '\n') {
    content_ptr++;
    buf_cpy++;
  }
  if(*content_ptr != '\0') content_ptr++; // advance pointer if line break

  *buf_cpy = '\0';
  VDP_drawText(buf, 1, 4+turn);
  VDP_drawText("done", 1, 8+turn);
  turn++;

  SET_RETURN( mrbc_string_new_cstr( vm, buf ) );
}

// globals for joypad input
u16 joy_pressed = 0;

static void c_megamrbc_read_joypad(mrb_vm *vm, mrb_value *v, int argc) {
  SET_INT_RETURN(joy_pressed);
}

static void c_megamrbc_scroll_one_step(mrb_vm *vm, mrb_value *v, int argc) {
  static int offset_a = 0;
  static int offset_b = 0;
  static uint8_t scrollspeed_a = 2;
  static uint8_t scrollspeed_b = 1;
  VDP_setHorizontalScroll(BG_B, offset_b -= scrollspeed_b);
  // VDP_setHorizontalScroll(BG_A, offset_a -= scrollspeed_a);
}

static void c_megamrbc_draw_image(mrb_vm *vm, mrb_value *v, int argc) {
  uint8_t x = mrbc_integer(v[1]);
  uint8_t y = mrbc_integer(v[2]);
  char *img_name = mrbc_string_cstr(&v[3]);

  char buf[40];
  VDP_drawText(buf, 2, 3);

  Image *image;

  // TODO: some sort of auto mapping here would be great...
  if(strncmp(img_name, "australia", sizeof("australia")) == 0) {
    image = &australia;
  } else if(strncmp(img_name, "yuji", sizeof("yuji")) == 0) {
    image = &yuji;
  } else if(strncmp(img_name, "ruby", sizeof("ruby")) == 0) {
    image = &ruby;
  } else if(strncmp(img_name, "rubykaigi", sizeof("rubykaigi")) == 0) {
    image = &rubykaigi;
  }

  PAL_setPaletteDMA(PAL3, image->palette->data);
  VDP_drawImageEx(
    BG_A, image,
    TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, TILE_USERINDEX + last + 1),
    x, y, FALSE, TRUE
  );
}

static void c_megamrbc_draw_bg(mrb_vm *vm, mrb_value *v, int argc) {
  char *str = mrbc_string_cstr(&v[1]);
  char bgpal_num = mrbc_integer(v[2]);
  char x = mrbc_integer(v[3]);
  char y = mrbc_integer(v[4]);
  int len = strlen(str);

  // do loop for str len
  for(int i = 0; i < len; i++) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bgpal_num, LOPRIO, VNOFLIP, HNOFLIP, TILE_USERINDEX + bgfull), x + i, y);
  }
}

static void c_megamrbc_klog(mrb_vm *vm, mrb_value *v, int argc) {
  char *str = mrbc_string_cstr(&v[1]);

  KLog(str);
}

static void c_megamrbc_show_progress(mrb_vm *vm, mrb_value *v, int argc) {
  int pos = mrbc_integer(v[1]);
  int size = mrbc_integer(v[2]);

  int x = ((float)pos / (float)size * 304) + 8;

  VDP_setSpriteFull(0, x, 220, SPRITE_SIZE(1,1), TILE_ATTR_FULL(0,HIPRIO,VNOFLIP,HNOFLIP,TILE_USERINDEX + vert), 1);
  VDP_updateSprites(4, 1);
}

static void c_megamrbc_show_timer(mrb_vm *vm, mrb_value *v, int argc) {
  char buf[8];
  int x = getTick() / 300;
  sprintf(buf, "%d", x);
  // KLog(buf);
  VDP_drawText(buf,  35, 26);
}

// sleep, but time is specificed in 1/300s
static void c_megamrbc_sleep(mrb_vm *vm, mrb_value *v, int argc) {
  int s = mrbc_integer(v[1]);
  int start = getTick();

  // FIXME: wrap-around not supported
  while( getTick() - start < s) {
    SYS_doVBlankProcess();
  }
}

static void c_megamrbc_set_bg_colour(mrb_vm *vm, mrb_value *v, int argc) {
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  char bg_pal = mrbc_integer(v[3]);
  char *bg_region = mrbc_symbol_cstr(&v[4]);

  // TODO: improve readability
  if(strncmp(bg_region, "bottom", sizeof("bottom")) == 0) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bg_pal, LOPRIO, VNOFLIP, HNOFLIP, TILE_USERINDEX + bgt), x, y);
  } else if(strncmp(bg_region, "top", sizeof("top")) == 0) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bg_pal, LOPRIO, VFLIP, HNOFLIP, TILE_USERINDEX + bgt), x, y);
  } else if(strncmp(bg_region, "left", sizeof("left")) == 0) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bg_pal, LOPRIO, VNOFLIP, HFLIP, TILE_USERINDEX + bgl), x, y);
  } else if(strncmp(bg_region, "right", sizeof("right")) == 0) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bg_pal, LOPRIO, VNOFLIP, HNOFLIP, TILE_USERINDEX + bgl), x, y);
  } else if(strncmp(bg_region, "top_left", sizeof("top_left")) == 0) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bg_pal, LOPRIO, VNOFLIP, HNOFLIP, TILE_USERINDEX + bgtl), x, y);
  } else if(strncmp(bg_region, "top_right", sizeof("top_right")) == 0) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bg_pal, LOPRIO, VNOFLIP, HFLIP, TILE_USERINDEX + bgtl), x, y);
  } else if(strncmp(bg_region, "bottom_left", sizeof("bottom_left")) == 0) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bg_pal, LOPRIO, VFLIP, HNOFLIP, TILE_USERINDEX + bgtl), x, y);
  } else if(strncmp(bg_region, "bottom_right", sizeof("bottom_right")) == 0) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bg_pal, LOPRIO, VFLIP, HFLIP, TILE_USERINDEX + bgtl), x, y);
  } else if(strncmp(bg_region, "full", sizeof("full")) == 0) {
    VDP_setTileMapXY(BG_B, TILE_ATTR_FULL(bg_pal, LOPRIO, VNOFLIP, HNOFLIP, TILE_USERINDEX + bgfull), x, y);
  }
}

static void c_megamrbc_draw_arrow_r(mrb_vm *vm, mrb_value *v, int argc) {
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_USERINDEX + arwr, x, y);
}

// TODO: refactor with above
static void c_megamrbc_draw_arrow_l(mrb_vm *vm, mrb_value *v, int argc) {
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  // TODO: Is PAL0 okay?
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, LOPRIO, VNOFLIP, HFLIP, TILE_USERINDEX + arwr), x, y);
}

static void c_megamrbc_draw_arrow_u(mrb_vm *vm, mrb_value *v, int argc) {
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_USERINDEX + arwu, x, y);
}

static void c_megamrbc_draw_arrow_d(mrb_vm *vm, mrb_value *v, int argc) {
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, LOPRIO, VFLIP, HNOFLIP, TILE_USERINDEX + arwu), x, y);
}

static void c_megamrbc_draw_right_t(mrb_vm *vm, mrb_value *v, int argc) {
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_USERINDEX + trght, x, y);
}

static void c_megamrbc_draw_left_t(mrb_vm *vm, mrb_value *v, int argc) {
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, LOPRIO, VNOFLIP, HFLIP, TILE_USERINDEX + trght), x, y);
}

static void c_megamrbc_draw_upright_t(mrb_vm *vm, mrb_value *v, int argc) {
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_USERINDEX + t, x, y);
}

static void c_megamrbc_draw_flipped_t(mrb_vm *vm, mrb_value *v, int argc) {
  char x = mrbc_integer(v[1]);
  char y = mrbc_integer(v[2]);
  VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL0, LOPRIO, VFLIP, HNOFLIP, TILE_USERINDEX + t), x, y);
}

static void c_megamrbc_play_se(mrb_vm *vm, mrb_value *v, int argc) {
  XGM_startPlayPCM(SE_TEST,1,SOUND_PCM_CH2);
}

static void c_megamrbc_set_bg_num(mrb_vm *vm, mrb_value *v, int argc) {
  char bg_num = mrbc_integer(v[1]);
  KLog("setting bg colour");
  PAL_setColor(0, bg_num);
}

void make_class(mrb_vm *vm)
{
  mrb_class *cls = mrbc_define_class(vm, "MegaMrbc", mrbc_class_object);
  mrbc_define_method(vm, cls, "draw_text", c_megamrbc_draw_text);
  mrbc_define_method(vm, cls, "draw_top_left", c_megamrbc_draw_top_left);
  mrbc_define_method(vm, cls, "draw_horizontal", c_megamrbc_draw_horizontal);
  mrbc_define_method(vm, cls, "draw_top_right", c_megamrbc_draw_top_right);
  mrbc_define_method(vm, cls, "draw_vertical", c_megamrbc_draw_vertical);
  mrbc_define_method(vm, cls, "draw_bottom_left", c_megamrbc_draw_bottom_left);
  mrbc_define_method(vm, cls, "draw_bottom_right", c_megamrbc_draw_bottom_right);
  mrbc_define_method(vm, cls, "read_joypad", c_megamrbc_read_joypad);
  mrbc_define_method(vm, cls, "wait_vblank", c_megamrbc_wait_vblank);
  mrbc_define_method(vm, cls, "show_cursor", c_megamrbc_show_cursor);
  mrbc_define_method(vm, cls, "show_tick", c_megamrbc_show_tick);
  mrbc_define_method(vm, cls, "draw_green_square", c_megamrbc_draw_green_square);
  mrbc_define_method(vm, cls, "draw_yellow_square", c_megamrbc_draw_yellow_square);
  mrbc_define_method(vm, cls, "draw_grey_square", c_megamrbc_draw_grey_square);
  mrbc_define_method(vm, cls, "clear_screen", c_megamrbc_clear_screen);
  // mrbc_define_method(vm, cls, "is_word?", c_megamrbc_is_word);
  mrbc_define_method(vm, cls, "random_answer", c_megamrbc_random_answer);
  mrbc_define_method(vm, cls, "call_rand", c_megamrbc_call_rand);
  mrbc_define_method(vm, cls, "test_func", c_test_func);
  // Maybe not needed???
  mrbc_define_method(vm, cls, "read_content_line", c_megamrbc_read_content_line);
  mrbc_define_method(vm, cls, "read_content", c_megamrbc_read_content);
  mrbc_define_method(vm, cls, "set_pal_colour", c_megamrbc_set_pal_colour);
  mrbc_define_method(vm, cls, "set_txt_pal", c_megamrbc_set_txt_pal);
  mrbc_define_method(vm, cls, "scroll_one_step", c_megamrbc_scroll_one_step);
  mrbc_define_method(vm, cls, "draw_image", c_megamrbc_draw_image);
  mrbc_define_method(vm, cls, "draw_bg", c_megamrbc_draw_bg);
  mrbc_define_method(vm, cls, "klog", c_megamrbc_klog);
  mrbc_define_method(vm, cls, "show_progress", c_megamrbc_show_progress);
  mrbc_define_method(vm, cls, "show_timer", c_megamrbc_show_timer);
  mrbc_define_method(vm, cls, "sleep_raw", c_megamrbc_sleep);
  mrbc_define_method(vm, cls, "set_bg_colour", c_megamrbc_set_bg_colour);
  mrbc_define_method(vm, cls, "draw_arrow_r", c_megamrbc_draw_arrow_r);
  mrbc_define_method(vm, cls, "draw_arrow_l", c_megamrbc_draw_arrow_l);
  mrbc_define_method(vm, cls, "draw_arrow_u", c_megamrbc_draw_arrow_u);
  mrbc_define_method(vm, cls, "draw_arrow_d", c_megamrbc_draw_arrow_d);
  mrbc_define_method(vm, cls, "draw_right_t", c_megamrbc_draw_right_t);
  mrbc_define_method(vm, cls, "draw_left_t", c_megamrbc_draw_left_t);
  mrbc_define_method(vm, cls, "draw_upright_t", c_megamrbc_draw_upright_t);
  mrbc_define_method(vm, cls, "draw_flipped_t", c_megamrbc_draw_flipped_t);
  mrbc_define_method(vm, cls, "play_se", c_megamrbc_play_se);
  mrbc_define_method(vm, cls, "set_bg_num", c_megamrbc_set_bg_num);
}

void mrubyc(const uint8_t *mrbbuf)
{
  mrbc_init_global();
  mrbc_init_class();

  mrbc_vm *vm = mrbc_vm_open(NULL);
  if( vm == 0 ) {
    return;
  }

  if( mrbc_load_mrb(vm, mrbbuf) != 0 ) {
    return;
  }

  mrbc_vm_begin(vm);

  make_class(vm);

  mrbc_vm_run(vm);
  mrbc_vm_end(vm);
  mrbc_vm_close(vm);
}

extern const uint8_t mrbsrc[];

void reset_text_colours() {
  PAL_setColor(15, 0x0FFF);
  PAL_setColor(31, 0x02F2);
  PAL_setColor(47, 0x022F);
  PAL_setColor(63, 0x0FF2);
}

void init_screen() {
  // Initialise screen
  // SPR_init(0,0,0); // this causes background mountains not to show?
  VDP_setScreenWidth320();
  VDP_setHInterrupt(0);
  VDP_setHilightShadow(0);
  reset_text_colours();
  VDP_setTextPalette(0);
  PAL_setColor(0, 0x0000);
  VDP_resetSprites();
}

/*
void set_up_colours() {
  VDP_setPaletteColor(1, RGB24_TO_VDPCOLOR(0x222222)); // dark grey

  // background colours
  VDP_setPaletteColor(2, RGB24_TO_VDPCOLOR(0x6AAA64)); // green
  VDP_setPaletteColor(2 + 16, RGB24_TO_VDPCOLOR(0xC9B458)); // yellow
  VDP_setPaletteColor(2 + 32, RGB24_TO_VDPCOLOR(0x888C8E)); // grey

  VDP_setPaletteColor(3, RGB24_TO_VDPCOLOR(0x2222AA)); // blue
}
*/

static void joy_event_handler(u16 pad_num, u16 changed, u16 state) {
  char buf[40];
  static int count = 0;
  // VDP_drawText(buf, 1, 18);
  if(pad_num == JOY_1) joy_pressed = changed & state;
  else joy_pressed = 0;
}

int main(void) {
  init_screen();
  JOY_init();
  JOY_setEventHandler(&joy_event_handler);
  XGM_setPCM(SE_TEST, se_test, sizeof(se_test));

  // set_up_colours();
  load_tiles();

  if( mrbsrc == 0 ) return 1;

  mrubyc( mrbsrc );

  return 0;
}
