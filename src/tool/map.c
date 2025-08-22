#include "tool_internal.h"

#define COLC 20
#define ROWC 11

// Values for our intermediate map.
#define TILE_EMPTY 0
#define TILE_WALL 1
#define TILE_SPIKES 2
#define TILE_LASER 3
#define TILE_HERO 4

/* Compile map.
 */
 
int tool_convert_mapbin_maptext(struct sr_encoder *dst,const char *src,int srcc,const char *path) {

  /* Read text into an intermediate map.
   */
  int expectlen=((COLC*2)+1)*ROWC;
  if (srcc!=expectlen) {
    fprintf(stderr,"%s: Map must be exactly %d bytes (%d columns, %d rows). Found %d.\n",path,expectlen,COLC,ROWC,srcc);
    return -2;
  }
  uint8_t map[COLC*ROWC];
  uint8_t *mapp=map;
  int row=0;
  for (;row<ROWC;row++) {
    int col=0;
    for (;col<COLC;col++,mapp++,src+=2) {
           if ((src[0]=='.')&&(src[1]==',')) *mapp=TILE_EMPTY;
      else if ((src[0]=='X')&&(src[1]=='x')) *mapp=TILE_WALL;
      else if ((src[0]=='S')&&(src[1]=='s')) *mapp=TILE_SPIKES;
      else if ((src[0]=='L')&&(src[1]=='l')) *mapp=TILE_LASER;
      else if ((src[0]=='H')&&(src[1]=='h')) *mapp=TILE_HERO;
      else {
        fprintf(stderr,"%s:%d: Unknown tile '%.2s'.\n",path,row+1,src);
        return -2;
      }
    }
    if (*src!=0x0a) {
      fprintf(stderr,"%s:%d: Expected LF.\n",path,row+1);
      return -2;
    }
    src++;
  }
  
  /* Encode that intermediate map with our bespoke RLE and whatnot.
   */
  int maplen=sizeof(map);
  while (maplen&&(map[maplen-1]==TILE_WALL)) maplen--; // No need to encode trailing walls.
  int dstc0=dst->c;
  int mp=0;
  while (mp<maplen) {
    int tileid=map[mp++];
    int c=1;
    while ((mp<maplen)&&(map[mp]==tileid)) { c++; mp++; }
    switch (tileid) {
      case TILE_EMPTY: {
          while (c>=64) {
            sr_encode_u8(dst,0x3f);
            c-=64;
          }
          if (c>0) {
            sr_encode_u8(dst,c-1);
          }
        } break;
      case TILE_WALL: {
          while (c>=64) {
            sr_encode_u8(dst,0x3f);
            c-=64;
          }
          if (c>0) {
            sr_encode_u8(dst,c-1);
          }
        } break;
      case TILE_SPIKES: {
          while (c>=16) {
            sr_encode_u8(dst,0x8f);
            c-=16;
          }
          if (c>0) {
            sr_encode_u8(dst,0x80|(c-1));
          }
        } break;
      case TILE_LASER: {
          while (c-->0) {
            sr_encode_u8(dst,0xc1);
          }
        } break;
      case TILE_HERO: {
          while (c-->0) {
            sr_encode_u8(dst,0xc0);
          }
        } break;
      default: {
          fprintf(stderr,"%s:%d: map tile 0x%02x\n",__FILE__,__LINE__,tileid);
          return -2;
        }
    }
  }

  if (1) {
    int len=dst->c-dstc0;
    fprintf(stderr,"%s: %d cells in %d bytes\n",path,(int)sizeof(map),len);
  }
  return 0;
}

/* Compile map to C.
 */
 
int tool_convert_c_maptext(struct sr_encoder *dst,const char *src,int srcc,const char *path,const char *dstpath) {
  struct sr_encoder bin={0};
  int err=tool_convert_mapbin_maptext(&bin,src,srcc,path);
  if (err<0) {
    sr_encoder_cleanup(&bin);
    return err;
  }
  err=tool_convert_c_any(dst,bin.v,bin.c,path,dstpath);
  sr_encoder_cleanup(&bin);
  return err;
}
