/* audio/main.c
 *
 * Message format. Each message is identified by its leading byte.
 *   0x01 NOTE: [0x01,noteida 0..63,noteidz 0..63,level 0..31,duration 16ms]
 *   0x02 SONG: [0x02,songid]
 */

#include "shovel/shovel.h"
#include "opt/synmin/synmin.h"

extern const unsigned char nine_lives[];
extern const int nine_lives_len;

extern const unsigned char get_over_here[];
extern const int get_over_here_len;

extern const unsigned char caught_the_mouse[];
extern const int caught_the_mouse_len;

static float buffer[1024];
 
int sha_init(int rate,int chanc) {
  if (synmin_init(rate,chanc)<0) return -1;
  sh_spcm(0,buffer,sizeof(buffer)/sizeof(buffer[0]));
  return 0;
}
 
void sha_update(int framec) {
  unsigned char msg[256];
  int msgc;
  while ((msgc=sh_mr(msg,sizeof(msg)))>0) {
    switch (msg[0]) {
      case 0x01: if (msgc>=5) {
          synmin_note(msg[1],msg[2],msg[3],msg[4]);
        } break;
      case 0x02: if (msgc>=2) {
          const void *src=0;
          int srcc=0;
          switch (msg[1]) {
            case 0: break; // Explicitly empty.
            //case 1: src=nine_lives; srcc=nine_lives_len; break;
            case 2: src=get_over_here; srcc=get_over_here_len; break;
            //case 3: src=caught_the_mouse; srcc=caught_the_mouse_len; break;
          }
          synmin_song(src,srcc,0,1);
        } break;
    }
  }
  synmin_update(buffer,framec);
}

/* Annoying clang glue.
 * clang inserts calls to memset and memcpy despite our having told it "nostdlib".
 * Whatever, we can implement them.
 */

#if USE_web
void *memset(void *s, int n, long c) {
  unsigned char *p=s;
  for (;c-->0;p++) *p=n;
  return s;
}
#endif
