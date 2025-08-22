#include "main/game.h"

#define ANIMFRAME sprite->iv[0]
#define JUMPP sprite->iv[1]
#define WINGS sprite->iv[2]
#define SEATED sprite->iv[3]
#define ANIMCLOCK sprite->fv[0]
#define FLAPCLOCK sprite->fv[1]

/* Definition of the jump curve.
 * This may be aborted anywhere.
 * Should end with a bunch of zeroes. Those are meaningful, they define a period before gravity kicks in.
 */
static const int jumpv[]={
  -3,-3,-3,-2,-2,-2,-2,-1,-1,-1,-1,-1,0,0,0,0,0,
};
static const int jumpc=sizeof(jumpv)/sizeof(jumpv[0]);

static const int flyv[]={
  -2,-2,-2,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
static const int flyc=sizeof(flyv)/sizeof(flyv[0]);

static void _hero_init(struct sprite *sprite) {
  sprite->tileid=0x50;
  sprite->xform=0;
  sprite->xbgr=0xff000000;
  sprite->solid=1;
  WINGS=1;//XXX TEMP
}

/* Physics tests.
 */
 
static int hero_lmcell_impassable(uint8_t tile) {
  switch (tile) {
    case TILE_WALL:
    case TILE_LASER:
      return 1;
  }
  return 0;
}

/* Resolve collisions.
 * Caller must move one axis at a time, and provide the normalized direction of motion.
 * Nonzero if position was adjusted.
 */
 
static int hero_collide(struct sprite *sprite,int dx,int dy) {
  int result=0;

  /* First, hard limit, can't leave the screen.
   * Don't return immediately from this, since we're not necessarily correcting against the direction of travel.
   */
  if (sprite->x<0) { sprite->x=0; result=1; }
  else if (sprite->x>FBW-sprite->w) { sprite->x=FBW-sprite->w; result=1; }
  if (sprite->y<0) { sprite->y=0; result=1; }
  else if (sprite->y>FBH-sprite->h) { sprite->y=FBH-sprite->h; result=1; }
  
  /* Check map.
   */
  int cola=sprite->x/TILESIZE;
  int colz=(sprite->x+sprite->w-1)/TILESIZE;
  int rowa=sprite->y/TILESIZE;
  int rowz=(sprite->y+sprite->h-1)/TILESIZE;
  if (cola<0) cola=0; else if (colz>=COLC) colz=COLC-1;
  if (rowa<0) rowa=0; else if (rowz>=ROWC) rowz=ROWC-1;
  const uint8_t *lmrow=g.lmap+(rowa+1)*(COLC+2)+cola+1;
  int row=rowa; for (;row<=rowz;row++,lmrow+=COLC+2) {
    const uint8_t *lmp=lmrow;
    int col=cola; for (;col<=colz;col++,lmp++) {
      if (hero_lmcell_impassable(*lmp)) {
             if (dx<0) sprite->x=(col+1)*TILESIZE;
        else if (dx>0) sprite->x=col*TILESIZE-sprite->w;
        else if (dy<0) sprite->y=(row+1)*TILESIZE;
        else if (dy>0) sprite->y=row*TILESIZE-sprite->h;
        return 1;
      }
    }
  }
  
  /* Check solid sprites.
   */
  struct sprite *other=g.spritev;
  int i=g.spritec;
  for (;i-->0;other++) {
    if (!other->solid||other->defunct||(other==sprite)) continue;
    if (other->x>=sprite->x+sprite->w) continue;
    if (other->y>=sprite->y+sprite->h) continue;
    if (sprite->x>=other->x+other->w) continue;
    if (sprite->y>=other->y+other->h) continue;
         if (dx<0) sprite->x=other->x+other->w;
    else if (dx>0) sprite->x=other->x-sprite->w;
    else if (dy<0) sprite->y=other->y+other->h;
    else if (dy>0) sprite->y=other->y-sprite->h;
    return 1;
  }
  
  return result;
}

/* Update.
 */

static void _hero_update(struct sprite *sprite,double elapsed) {

  /* Vertical motion: Jumping, flying, and gravity.
   */
  SEATED=0; // Until gravity says otherwise.
  if (WINGS) { // Flying maybe.
    if (g.input&SH_BTN_SOUTH) {
      if (FLAPCLOCK<=0.0) SFX(flap);
      FLAPCLOCK+=elapsed;
      if (JUMPP<flyc) {
        sprite->y+=flyv[JUMPP];
        hero_collide(sprite,0,-1);
        JUMPP++;
      } else {
        sprite->y++;
        if (hero_collide(sprite,0,1)) {
          SEATED=1;
        }
      }
    } else {
      JUMPP=0;
      FLAPCLOCK=0.0;
      sprite->y++;
      if (hero_collide(sprite,0,1)) {
        SEATED=1;
      }
    }
  } else if ((g.input&SH_BTN_SOUTH)&&(JUMPP<jumpc)) { // Jumping.
    if (!JUMPP) SFX(jump);
    sprite->y+=jumpv[JUMPP];
    hero_collide(sprite,0,-1);
    JUMPP++;
  } else { // Falling.
    JUMPP=jumpc;
    sprite->y++;
    if (hero_collide(sprite,0,1)) {
      SEATED=1;
      if (!(g.input&SH_BTN_SOUTH)) { // Restore jump power.
        JUMPP=0;
      }
    }
  }

  //TODO Lay eggs.
  //TODO Breath fire.
  //TODO Take damage.
  //TODO Win level.

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
    hero_collide(sprite,dx,0);
    if ((ANIMCLOCK-=elapsed)<=0.0) {
      ANIMCLOCK+=0.200;
      if (++(ANIMFRAME)>=2) ANIMFRAME=0;
    }
  } else {
    ANIMFRAME=0;
    ANIMCLOCK=0.0;
  }
}

/* Render.
 */

static void _hero_render(struct sprite *sprite) {
  uint8_t tileid=sprite->tileid;
  if (ANIMFRAME&&SEATED) tileid++;
  int dstx=sprite->x;
  int dsty=sprite->y;
  int srcx=(tileid&0x0f)*TILESIZE;
  int srcy=(tileid>>4)*TILESIZE;
  r1b_img32_blit_img1(&g.fb,&g.graphics,dstx,dsty,srcx,srcy,TILESIZE,TILESIZE,0,sprite->xbgr,sprite->xform);
  if (WINGS) {
    uint8_t wtileid=0x61;
         if (FLAPCLOCK>0.100) wtileid=0x63;
    else if (FLAPCLOCK>0.000) wtileid=0x62;
    srcx=(wtileid&0x0f)*TILESIZE;
    srcy=(wtileid>>4)*TILESIZE;
    int wdstx=dstx;
    int wdsty=dsty;
    if (sprite->xform&R1B_XFORM_XREV) wdstx-=2; else wdstx+=2;
    r1b_img32_blit_img1(&g.fb,&g.graphics,wdstx,wdsty,srcx,srcy,TILESIZE,TILESIZE,0,0xffffffff,sprite->xform);
  }
}

/* Type definition.
 */

const struct sprite_type sprite_type_hero={
  .name="hero",
  .init=_hero_init,
  .update=_hero_update,
  .render=_hero_render,
};
