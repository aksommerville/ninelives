#include "game.h"

struct g g={0};

/* Quit.
 */
 
void shm_quit(int status) {
}

/* Init.
 */

int shm_init() {
  if (render_init()<0) return -1;
  if (load_map(1)<0) return -1;
  sh_ms("\x02\x02",2); // 01=nine_lives, 02=get_over_here, 03=caught_the_mouse
  return 0;
}

/* Update.
 */

void shm_update(double elapsed) {
  int i;
  struct sprite *sprite;

  /* Acquire input and react to global inputs.
   */
  g.pvinput=g.input;
  g.input=sh_in();
  if (g.input!=g.pvinput) {
    if ((g.input&SH_BTN_AUX1)&&!(g.pvinput&SH_BTN_AUX1)) { sh_term(0); return; }
  }

  /* Update model state.
   */
  for (sprite=g.spritev,i=g.spritec;i-->0;sprite++) {
    if (sprite->type->update) sprite->type->update(sprite,elapsed);
  }
  
  /* Reap defunct sprites.
   */
  for (i=g.spritec,sprite=g.spritev+g.spritec-1;i-->0;sprite--) {
    if (sprite->defunct) {
      g.spritec--;
      memmove(sprite,sprite+1,sizeof(struct sprite)*(g.spritec-i));
    }
  }
  
  /* Render.
   */
  memcpy(g.fb.v,g.bgbits.v,FBW*FBH*4);
  for (sprite=g.spritev,i=g.spritec;i-->0;sprite++) {
    if (sprite->type->render) {
      sprite->type->render(sprite);
    } else {
      int srcx=(sprite->tileid&0x0f)*TILESIZE;
      int srcy=(sprite->tileid>>4)*TILESIZE;
      r1b_img32_blit_img1(&g.fb,&g.graphics,sprite->x,sprite->y,srcx,srcy,TILESIZE,TILESIZE,0,sprite->xbgr,sprite->xform);
    }
  }
  sh_fb(g.fb.v,FBW,FBH);
}

/* Annoying clang glue.
 * clang inserts calls to memset and memcpy despite our having told it "nostdlib".
 * Whatever, we can implement them.
 */

#if USE_web
void *memset(void *s, int n, long c) {
  unsigned char *p=s;
  for (;c-->0;p++) *p=n;
  return s;
}
void *memcpy(void *dst,const void *src,long c) {
  uint8_t *dstp=dst;
  const uint8_t *srcp=src;
  for (;c-->0;dstp++,srcp++) *dstp=*srcp;
  return dst;
}
void *memmove(void *dst,const void *src,long c) {
  if (dst<src) {
    uint8_t *dstp=dst;
    const uint8_t *srcp=src;
    for (;c-->0;dstp++,srcp++) *dstp=*srcp;
  } else {
    uint8_t *dstp=dst+c-1;
    const uint8_t *srcp=src+c-1;
    for (;c-->0;dstp--,srcp--) *dstp=*srcp;
  }
  return dst;
}
#endif
