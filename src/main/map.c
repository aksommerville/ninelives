#include "game.h"

#define FOR_EACH_MAP \
  _(m1)
  
#define _(tag) \
  extern const unsigned char tag[]; \
  extern const int tag##_len;
FOR_EACH_MAP
#undef _

static void fill_lmap(int x,int y,int w,int h,uint8_t v) {
  uint8_t *row=g.lmap+y*(COLC+2)+x;
  for (;h-->0;row+=COLC+2) {
    uint8_t *p=row;
    int xi=w;
    for (;xi-->0;p++) *p=v;
  }
}

static int load_map_bin(const uint8_t *v,int c) {

  // Default (lmap).
  memset(g.lmap,TILE_WALL,sizeof(g.lmap));
  
  // Decode (v) into (lmap).
  int dstx=1,dsty=1,srcp=0;
  while (srcp<c) {
    uint8_t cmd=v[srcp++];
    uint8_t tileid,count=1;
         if ((cmd&0xc0)==0x00) { tileid=TILE_EMPTY; count=(cmd&0x3f)+1; }
    else if ((cmd&0xc0)==0x40) { tileid=TILE_WALL; count=(cmd&0x3f)+1; }
    else if ((cmd&0xf0)==0x80) { tileid=TILE_SPIKES; count=(cmd&0x0f)+1; }
    else if (cmd==0xc0) tileid=TILE_HERO;
    else if (cmd==0xc1) tileid=TILE_LASER;
    else {
      fprintf(stderr,"Unknown map command 0x%02x.\n",cmd);
      return -2;
    }
    while (count-->0) {
      if (dstx>=COLC+1) {
        dstx=1;
        dsty++;
        if (dsty>=ROWC+1) goto _done_tiles_;
      }
      g.lmap[dsty*(COLC+2)+dstx]=tileid;
      dstx++;
    }
  }
 _done_tiles_:;

  return 0;
}

int load_map(int id) {
  if (id<1) return -1;
  #define _(tag) if (!--id) return load_map_bin(tag,tag##_len);
  FOR_EACH_MAP
  #undef _
  return -1;
}
