/* sprite.h
 */
 
#ifndef SPRITE_H
#define SPRITE_H

struct sprite;
struct sprite_type;

#define SPRITE_LIMIT 32
#define SPRITE_IV_SIZE 8
#define SPRITE_FV_SIZE 8

struct sprite {
  const struct sprite_type *type;
  int x,y,w,h; // world pixels (same as framebuffer pixels)
  uint8_t tileid,xform;
  uint32_t xbgr;
  int defunct;
  int solid;
  int iv[SPRITE_IV_SIZE];
  double fv[SPRITE_FV_SIZE];
};

struct sprite_type {
  const char *name;
  void (*init)(struct sprite *sprite);
  void (*update)(struct sprite *sprite,double elapsed);
  void (*render)(struct sprite *sprite);
};

struct sprite *sprite_spawn(const struct sprite_type *type,int x,int y);

extern const struct sprite_type sprite_type_hero;
extern const struct sprite_type sprite_type_corpse;
extern const struct sprite_type sprite_type_egg;

/* Create a corpse sprite with (x,y) at its center, then set its xform, then call this.
 * (dx,dy) are a cardinal unit vector describing the edge of the spike tile we're at.
 */
void sprite_corpse_setup(struct sprite *sprite,int dx,int dy);

#endif
