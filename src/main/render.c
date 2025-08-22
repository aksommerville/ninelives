#include "game.h"

#define GLYPHW 5 /* max */
#define GLYPHH 8 /* constant */
#define SPACEW 3

static uint32_t fb[FBW*FBH];
static uint32_t bgbits[FBW*FBH];

R1B_DECL(graphics)

// Includes space, though we don't use that.
static uint8_t width_by_glyph[0x40];

/* Measure width of one glyph.
 * Only happens during init, caller caches them.
 * (glyphid) is 0..0x3f, ASCII-0x20.
 */
 
static int column_in_use(int x,int y,int h) {
  const uint8_t *p=g.graphics.v+y*g.graphics.stride+(x>>3);
  uint8_t mask=0x80>>(x&7);
  for (;h-->0;p+=g.graphics.stride) if ((*p)&mask) return 1;
  return 0;
}
 
static int measure_glyph(int glyphid) {
  int x=(glyphid&15)*GLYPHW;
  int y=(glyphid>>4)*GLYPHH;
  int w=GLYPHW;
  while ((w>1)&&!column_in_use(x+w-1,y,GLYPHH)) w--;
  return w;
}

/* Init.
 */
 
int render_init() {
  
  g.fb.v=fb;
  g.fb.w=FBW;
  g.fb.h=FBH;
  g.fb.stridewords=FBW;
  
  g.bgbits.v=bgbits;
  g.bgbits.w=FBW;
  g.bgbits.h=FBH;
  g.bgbits.stridewords=FBW;
  
  g.graphics.w=graphics_w;
  g.graphics.h=graphics_h;
  g.graphics.stride=graphics_stride;
  g.graphics.v=(void*)graphics_pixels;
  
  uint8_t *p=width_by_glyph;
  int row=0; for (;row<4;row++) {
    int col=0; for (;col<16;col++,p++) {
      *p=measure_glyph((row<<4)|col);
    }
  }
  
  return 0;
}
