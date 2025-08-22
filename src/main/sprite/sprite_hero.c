#include "main/game.h"

#define ANIMFRAME sprite->iv[0]
#define ANIMCLOCK sprite->fv[0]

static void _hero_init(struct sprite *sprite) {
  sprite->tileid=0x50;
  sprite->xform=0;
  sprite->xbgr=0xff000000;
}

static void _hero_update(struct sprite *sprite,double elapsed) {

  /* Motion.
   */
  int dx=0;
  switch (g.input&(SH_BTN_LEFT|SH_BTN_RIGHT)) {
    case SH_BTN_LEFT: {
        sprite->xform=0;
        dx=-1;
      } break;
    case SH_BTN_RIGHT: {
        sprite->xform=R1B_XFORM_XREV;
        dx=1;
      } break;
  }
  if (dx) {
    sprite->x+=dx;
    if ((ANIMCLOCK-=elapsed)<=0.0) {
      ANIMCLOCK+=0.200;
      if (++(ANIMFRAME)>=2) ANIMFRAME=0;
    }
  } else {
    ANIMFRAME=0;
    ANIMCLOCK=0.0;
  }
}

static void _hero_render(struct sprite *sprite) {
  uint8_t tileid=sprite->tileid;
  if (ANIMFRAME) tileid++;
  int dstx=sprite->x-(TILESIZE>>1);
  int dsty=sprite->y-(TILESIZE>>1);
  int srcx=(tileid&0x0f)*TILESIZE;
  int srcy=(tileid>>4)*TILESIZE;
  r1b_img32_blit_img1(&g.fb,&g.graphics,dstx,dsty,srcx,srcy,TILESIZE,TILESIZE,0,sprite->xbgr,sprite->xform);
}

const struct sprite_type sprite_type_hero={
  .name="hero",
  .init=_hero_init,
  .update=_hero_update,
  .render=_hero_render,
};
