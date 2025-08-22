#ifndef GAME_H
#define GAME_H

#define FBW 160 /* Must be exactly COLC*TILESIZE */
#define FBH 88 /* Must be exactly ROWC*TILESIZE */
#define TILESIZE 8
#define COLC 20
#define ROWC 11

#define TILE_EMPTY 0
#define TILE_WALL 1
#define TILE_SPIKES 2
#define TILE_LASER 3
#define TILE_HERO 4

#include "shovel/shovel.h"
#include "opt/r1b/r1b.h"
#include <stdint.h>

extern struct g {
  struct r1b_img32 fb;
  struct r1b_img1 graphics;
  
  /* Logical map, with a 1-cell border.
   * Values are TILE_*.
   */
  uint8_t lmap[(COLC+2)*(ROWC+2)];

} g;

int render_init();

/* (dstx,dsty) is the top-left corner of the first glyph.
 * Single row only.
 * Returns next X position.
 * Glyphs are all 7 pixels high, though technically they could be 8.
 * Codepoints outside G0 are quietly skipped.
 */
int draw_string(struct r1b_img32 *dst,int dstx,int dsty,const char *src,int srcc,int xbgr);

int load_map(int id);

#if USE_native
  #include <stdio.h>
  #include <string.h>
#else
  #define stderr 0
  static inline void fprintf(void *f,const char *fmt,...) {}
  void *memset(void *s, int n, long c);
#endif

#endif
