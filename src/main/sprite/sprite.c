#include "main/game.h"

/* Spawn sprite.
 */
 
struct sprite *sprite_spawn(const struct sprite_type *type,int x,int y) {
  if (g.spritec>=SPRITE_LIMIT) return 0;
  struct sprite *sprite=g.spritev+g.spritec++;
  memset(sprite,0,sizeof(struct sprite));
  sprite->type=type;
  sprite->x=x;
  sprite->y=y;
  sprite->w=TILESIZE;
  sprite->h=TILESIZE;
  if (type->init) type->init(sprite);
  return sprite;
}
