/* sprite_egg.c
 * iv[0]: Priority 1..8. Highest value is the next one to hatch. Caller sets (hero).
 */
 
#include "main/game.h"

static void _egg_init(struct sprite *sprite) {
  sprite->tileid=0x20;
  sprite->xbgr=0xffffffff;
}

const struct sprite_type sprite_type_egg={
  .name="egg",
  .init=_egg_init,
};
