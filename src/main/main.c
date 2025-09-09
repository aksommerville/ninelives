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
  sh_ms("\x02\x02",2); // 01=nine_lives(REMOVED), 02=get_over_here, 03=caught_the_mouse(REMOVED)
  return 0;
}

/* Draw integer.
 */
 
static void draw_int(int x,int y,int v,int digitc) {
  if (v<0) v=0;
  if (digitc<1) {
    int limit=10;
    digitc=1;
    while (v>=limit) { digitc++; if (limit>214748364) break; limit*=10; }
  }
  int i=digitc;
  x+=(digitc-1)*6;
  for (;i-->0;v/=10,x-=6) r1b_img32_blit_img1(&g.fb,&g.graphics,x,y,(v%10)*5,24,5,7,0,0xffffffff,0);
}

/* Update and render, game over.
 */
 
static void gameover_update(double elapsed) {

  g.gameover++;

  // Start over? Enforce a tiny blackout period.
  if ((g.gameover>=20)&&(g.input&SH_BTN_SOUTH)&&!(g.pvinput&SH_BTN_SOUTH)) {
    g.gameover=0;
    g.score.playtime=0;
    g.score.eggc=0;
    g.score.catc=0;
    g.score.restartc=0;
    load_map(1);
    sh_ms("\x02\x02",2); // 01=nine_lives(REMOVED), 02=get_over_here, 03=caught_the_mouse(REMOVED)
  }
  
  // First time we update, draw the framebuffer. It's static, so no need to do it again after.
  if (g.gameover==2) {
    memset(g.fb.v,0,FBW*FBH*4);
    
    r1b_img32_blit_img1(&g.fb,&g.graphics,56,28,50,24,7,7,0,0xffc0c0c0,0); // clock
    r1b_img32_blit_img1(&g.fb,&g.graphics,55,36, 0, 9,8,7,0,0xffc0c0c0,R1B_XFORM_XREV); // cat
    r1b_img32_blit_img1(&g.fb,&g.graphics,57,45, 2,19,4,5,0,0xffc0c0c0,0); // egg
    r1b_img32_blit_img1(&g.fb,&g.graphics,56,52,57,24,7,7,0,0xffc0c0c0,0); // restart
    
    int ms=(int)(g.score.playtime*1000.0);
    int sec=ms/1000; ms%=1000;
    int min=sec/60; sec%=60;
    if (min>99) { min=sec=99; ms=999; }
    draw_int(66,28,min,2);
    r1b_img32_blit_img1(&g.fb,&g.graphics,78,28,64,24,1,7,0,0xffffffff,0);
    draw_int(80,28,sec,2);
    r1b_img32_blit_img1(&g.fb,&g.graphics,92,28,65,24,1,7,0,0xffffffff,0);
    draw_int(94,28,ms,3);
    
    draw_int(66,36,g.score.catc,0);
    draw_int(66,44,g.score.eggc,0);
    draw_int(66,52,g.score.restartc,0);
    
    sh_fb(g.fb.v,FBW,FBH);
  }
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
  
  /* If we're in the gameover modal, that's its own whole thing.
   */
  if (g.gameover) {
    gameover_update(elapsed);
    return;
  }
  
  /* Advance termination clock and react to its expiry.
   */
  if (g.term<0.0) {
    if ((g.term+=elapsed)>=0.0) {
      g.term=0.0;
      load_map(g.mapid);
    }
  } else if (g.term>0.0) {
    if ((g.term-=elapsed)<=0.0) {
      g.term=0.0;
      if (load_map(g.mapid+1)<0) {
        g.gameover=1;
        sh_ms("\x02\x00",2); // 01=nine_lives(REMOVED), 02=get_over_here, 03=caught_the_mouse(REMOVED)
      }
    }
  }

  /* Update model state.
   */
  g.score.playtime+=elapsed;
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
  int dstx=1;
  for (i=g.eggc;i-->0;dstx+=5) {
    r1b_img32_blit_img1(&g.fb,&g.graphics,dstx,FBH-6,2,19,4,5,0,0xffffffff,0);
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
