/* sprite_corpse.c
 * A dead cat that you can stand on.
 */
 
#include "main/game.h"

static void _corpse_init(struct sprite *sprite) {
  sprite->tileid=0x55;
  sprite->xform=0;
  sprite->xbgr=0xff000000;
  sprite->solid=1;
  
  // We'll be initialized to the size of a cell. Make us smaller, and anchor to the bottom edge.
  sprite->h=2;
  sprite->y+=TILESIZE-sprite->h;
}

static void _corpse_render(struct sprite *sprite) {
  int dstx=sprite->x;
  int dsty=sprite->y-TILESIZE+sprite->h;
  int srcx=(sprite->tileid&0x0f)*TILESIZE;
  int srcy=(sprite->tileid>>4)*TILESIZE;
  r1b_img32_blit_img1(&g.fb,&g.graphics,dstx,dsty,srcx,srcy,TILESIZE,TILESIZE,0,sprite->xbgr,sprite->xform);
  //TODO gore
}

const struct sprite_type sprite_type_corpse={
  .name="corpse",
  .init=_corpse_init,
  .render=_corpse_render,
};
