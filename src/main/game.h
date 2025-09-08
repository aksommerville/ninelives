#ifndef GAME_H
#define GAME_H

#define FBW 160 /* Must be exactly COLC*TILESIZE */
#define FBH 88 /* Must be exactly ROWC*TILESIZE */
#define TILESIZE 8
#define COLC 20
#define ROWC 11

#define TILE_EMPTY 0
#define TILE_WALL 1
#define TILE_SPIKES 2
#define TILE_LASER 3
#define TILE_HERO 4
#define TILE_DOOR 5

#include <stdint.h>
#include "shovel/shovel.h"
#include "opt/r1b/r1b.h"
#include "sprite/sprite.h"

extern struct g {
  struct r1b_img32 fb;
  struct r1b_img1 graphics;
  struct r1b_img32 bgbits; // Size of fb and minimum stride -- safe to just memcpy onto fb.
  
  int input,pvinput;
  
  /* Logical map, with a 1-cell border.
   * Values are TILE_*.
   */
  uint8_t lmap[(COLC+2)*(ROWC+2)];
  int mapid;

  struct sprite spritev[SPRITE_LIMIT];
  int spritec;
  int eggc; // Starts at 8, how many remain to be laid. Eggs in the field don't count.
  double term; // <0 to fail, >0 to pass. Counts toward zero before committing.
  int gameover;
  struct score {
    double playtime;
    int eggc; // Eggs laid.
    int catc; // Cats hatched, including the initial one.
    int restartc; // How many times did you die with no eggs?
  } score;
} g;

int render_init();

int load_map(int id);

// [noteida 0..63,noteidz 0..63,level 0..31,duration 16ms]
#define SFX(tag) sh_ms("\x01"SFX_##tag,5)
#define SFX_jump   "\x20\x28\x10\x10"
#define SFX_flap   "\x18\x14\x08\x0c"
#define SFX_layegg "\x24\x22\x0c\x14"

#if USE_native
  #include <stdio.h>
  #include <string.h>
#else
  #define stderr 0
  static inline void fprintf(void *f,const char *fmt,...) {}
  void *memset(void *s,int n,long c);
  void *memcpy(void *dst,const void *src,long c);
  void *memmove(void *dst,const void *src,long c);
#endif

#endif
