#include "game.h"

#define FOR_EACH_MAP \
  _(m1)
  
#define _(tag) \
  extern const unsigned char tag[]; \
  extern const int tag##_len;
FOR_EACH_MAP
#undef _

/* We've arranged for (lmap) to have a 1-tile border, so it is always safe to read the neighbors of every cell without bounds checking.
 * Returns a mask of which neighbors have exactly the same value as (*v):
 *  80=NW 40=N 20=NE 10=W 08=E 04=SW 02=S 01=SE
 */
static uint8_t get_lmap_neighbors(const uint8_t *v) {
  uint8_t dst=0;
  if (v[-COLC-3]==*v) dst|=0x80;
  if (v[-COLC-2]==*v) dst|=0x40;
  if (v[-COLC-1]==*v) dst|=0x20;
  if (v[     -1]==*v) dst|=0x10;
  if (v[      1]==*v) dst|=0x08;
  if (v[ COLC+1]==*v) dst|=0x04;
  if (v[ COLC+2]==*v) dst|=0x02;
  if (v[ COLC+3]==*v) dst|=0x01;
  return dst;
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
    else if (cmd==0xc2) tileid=TILE_DOOR;
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
  
  // Reset sprites and such.
  g.spritec=0;
  g.eggc=8;
  //TODO Reset other map state.
 
  // Render bgbits, joining neighbors and such on the fly.
  // We'll also generate sprites in this pass.
  const uint32_t bgcolor=0xffe0b020;
  const uint8_t *srcrow=g.lmap+COLC+3;
  int row=0;
  for (dsty=0;row<ROWC;row++,dsty+=TILESIZE,srcrow+=COLC+2) {
    const uint8_t *srcp=srcrow;
    int col=0;
    for (dstx=0;col<COLC;col++,dstx+=TILESIZE,srcp++) {
      switch (*srcp) {
      
        // EMPTY is a flat color, not even a tile.
        case TILE_EMPTY: _EMPTY_: {
            r1b_img32_fill_rect(&g.bgbits,dstx,dsty,TILESIZE,TILESIZE,bgcolor);
          } break;
        
        // WALL is the most complex: Render 4 subtiles based on neighbors.
        // The two corner tiles can't perform every transform due to stippling, so there's two version of each.
        case TILE_WALL: {
            uint8_t neighbors=get_lmap_neighbors(srcp);
            #define HALFTILE(dx,dy,sx,sy,xform) \
              r1b_img32_blit_img1( \
                &g.bgbits,&g.graphics, \
                dstx+dx*(TILESIZE>>1),dsty+dy*(TILESIZE>>1), \
                sx*(TILESIZE>>1),sy*(TILESIZE>>1), \
                TILESIZE>>1,TILESIZE>>1, \
                0xff186020,0xff38c040,xform \
              ); \
              r1b_img32_blit_img1( \
                &g.bgbits,&g.graphics, \
                dstx+dx*(TILESIZE>>1),dsty+dy*(TILESIZE>>1), \
                (3+sx)*(TILESIZE>>1),sy*(TILESIZE>>1), \
                TILESIZE>>1,TILESIZE>>1, \
                0,0xff000000,xform \
              );
            switch (neighbors&0xd0) {
              case 0xd0: HALFTILE(0,0,2,1,0) break;
              case 0x50: HALFTILE(0,0,0,1,0) break;
              case 0x40: case 0xc0: HALFTILE(0,0,2,0,R1B_XFORM_SWAP) break;
              case 0x10: case 0x90: HALFTILE(0,0,2,0,0) break;
              default: HALFTILE(0,0,0,0,0) break;
            }
            switch (neighbors&0x68) {
              case 0x68: HALFTILE(1,0,2,1,0) break;
              case 0x48: HALFTILE(1,0,1,1,0) break;
              case 0x40: case 0x60: HALFTILE(1,0,2,0,R1B_XFORM_SWAP|R1B_XFORM_YREV|R1B_XFORM_XREV) break;
              case 0x08: case 0x28: HALFTILE(1,0,2,0,0) break;
              default: HALFTILE(1,0,1,0,0) break;
            }
            switch (neighbors&0x16) {
              case 0x16: HALFTILE(0,1,2,1,0) break;
              case 0x12: HALFTILE(0,1,1,1,R1B_XFORM_XREV|R1B_XFORM_YREV) break;
              case 0x02: case 0x06: HALFTILE(0,1,2,0,R1B_XFORM_SWAP) break;
              case 0x10: case 0x14: HALFTILE(0,1,2,0,R1B_XFORM_XREV|R1B_XFORM_YREV) break;
              default: HALFTILE(0,1,1,0,R1B_XFORM_XREV|R1B_XFORM_YREV) break;
            }
            switch (neighbors&0x0b) {
              case 0x0b: HALFTILE(1,1,2,1,0) break;
              case 0x0a: HALFTILE(1,1,0,1,R1B_XFORM_XREV|R1B_XFORM_YREV) break;
              case 0x02: case 0x03: HALFTILE(1,1,2,0,R1B_XFORM_SWAP|R1B_XFORM_XREV|R1B_XFORM_YREV) break;
              case 0x08: case 0x09: HALFTILE(1,1,2,0,R1B_XFORM_XREV|R1B_XFORM_YREV) break;
              default: HALFTILE(1,1,0,0,R1B_XFORM_XREV|R1B_XFORM_YREV) break;
            }
            #undef HALFTILE
          } break;
          
        // SPIKES are the fourth tile, pointing up naturally.
        // Each SPIKE tile must have one EMPTY cardinal neighbor opposite a WALL one.
        case TILE_SPIKES: {
            uint8_t xform;
            if ((srcp[-COLC-2]==TILE_EMPTY)&&(srcp[COLC+2]==TILE_WALL)) xform=0; // up
            else if ((srcp[-1]==TILE_EMPTY)&&(srcp[1]==TILE_WALL)) xform=R1B_XFORM_SWAP|R1B_XFORM_XREV; // left
            else if ((srcp[1]==TILE_EMPTY)&&(srcp[-1]==TILE_WALL)) xform=R1B_XFORM_SWAP|R1B_XFORM_YREV; // right
            else if ((srcp[COLC+2]==TILE_EMPTY)&&(srcp[-COLC-2]==TILE_WALL)) xform=R1B_XFORM_XREV|R1B_XFORM_YREV; // down
            else {
              fprintf(stderr,"Spike must have cardinal WALL and EMPTY neighbors opposite each other.\n");
              return -1;
            }
            r1b_img32_blit_img1(&g.bgbits,&g.graphics,dstx,dsty,24,0,TILESIZE,TILESIZE,bgcolor,0xfffff8f0,xform);
          } break;
          
        // HERO becomes EMPTY after we note the position.
        case TILE_HERO: {
            struct sprite *sprite=sprite_spawn(&sprite_type_hero,col*TILESIZE,row*TILESIZE);
            if (!sprite) return -1;
            goto _EMPTY_;
          }
        
        // LASER is the sixth tile, pointing up naturally.
        // There must be another LASER on one of our axes; we'll point toward it.
        // Record the position for a sprite.
        case TILE_LASER: {
            r1b_img32_blit_img1(&g.bgbits,&g.graphics,dstx,dsty, 8,8,TILESIZE,TILESIZE,0xff00ffff,0xff008080,0); //TODO
          } break;
        
        // DOOR: Simple tile.
        case TILE_DOOR: {
            r1b_img32_blit_img1(&g.bgbits,&g.graphics,dstx,dsty,32,0,TILESIZE,TILESIZE,bgcolor,0xff203050,0);
          } break;
      }
    }
  }

  return 0;
}

int load_map(int id) {
  if (id<1) return -1;
  #define _(tag) if (!--id) return load_map_bin(tag,tag##_len);
  FOR_EACH_MAP
  #undef _
  return -1;
}
