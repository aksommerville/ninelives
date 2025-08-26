/* sprite_corpse.c
 * A dead cat that you can stand on.
 */
 
#include "main/game.h"

#define VX sprite->iv[0] /* Tile position relative to (x,y). */
#define VY sprite->iv[1]

static void _corpse_init(struct sprite *sprite) {
  sprite->tileid=0x15;
  sprite->xform=0;
  sprite->xbgr=0xff000000;
  sprite->solid=1;
}

static void _corpse_render(struct sprite *sprite) {
  int srcx=(sprite->tileid&0x0f)*TILESIZE;
  int srcy=(sprite->tileid>>4)*TILESIZE;
  int dstx=sprite->x+VX;
  int dsty=sprite->y+VY;
  r1b_img32_blit_img1(&g.fb,&g.graphics,dstx,dsty,srcx,srcy,TILESIZE,TILESIZE,0,sprite->xbgr,sprite->xform);
  //TODO gore
}

const struct sprite_type sprite_type_corpse={
  .name="corpse",
  .init=_corpse_init,
  .render=_corpse_render,
};

/* Setup.
 */

void sprite_corpse_setup(struct sprite *sprite,int dx,int dy) {
  if (dx<0) {
    sprite->xform=R1B_XFORM_SWAP;
    sprite->w=2;
    sprite->h=8;
    VX=-6;
    VY=0;
  } else if (dx>0) {
    sprite->xform=R1B_XFORM_SWAP|R1B_XFORM_YREV;
    sprite->w=2;
    sprite->h=8;
    VX=0;
    VY=0;
  } else if (dy>0) {
    sprite->xform|=R1B_XFORM_YREV;
    sprite->w=8;
    sprite->h=2;
    VX=0;
    VY=0;
    sprite->y-=1; // When binding to ceiling spikes, cheat up a little so the hero can still pass under.
  } else {
    sprite->w=8;
    sprite->h=2;
    VX=0;
    VY=-6;
  }
  sprite->x-=(sprite->w>>1);
  sprite->y-=(sprite->h>>1);
}
