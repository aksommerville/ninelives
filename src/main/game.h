#ifndef GAME_H
#define GAME_H

#define FBW 160
#define FBH 90

#include "shovel/shovel.h"
#include "opt/r1b/r1b.h"
#include <stdint.h>

extern struct g {
  struct r1b_img32 fb;
  struct r1b_img1 graphics;
} g;

int render_init();

/* (dstx,dsty) is the top-left corner of the first glyph.
 * Single row only.
 * Returns next X position.
 * Glyphs are all 7 pixels high, though technically they could be 8.
 * Codepoints outside G0 are quietly skipped.
 */
int draw_string(struct r1b_img32 *dst,int dstx,int dsty,const char *src,int srcc,int xbgr);

#endif
