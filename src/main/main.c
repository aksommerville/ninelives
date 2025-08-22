#include "game.h"

struct g g={0};

/* Quit.
 */
 
void shm_quit(int status) {
}

/* Init.
 */

int shm_init() {
  if (render_init()<0) return -1;
  if (load_map(1)<0) return -1;
  sh_ms("\x02\x02",2); // 01=nine_lives, 02=get_over_here, 03=caught_the_mouse
  return 0;
}

/* Update.
 */

void shm_update(double elapsed) {
  
  memcpy(g.fb.v,g.bgbits.v,FBW*FBH*4);
  
  //draw_string(&g.fb,1,1,"The quick brown fox jumps",-1,0xffffffff);
  //draw_string(&g.fb,1,9,"over the lazy dog.",-1,0xffffffff);
  
  //r1b_img32_blit_img1(&g.fb,&g.graphics,80,50,0,40,8,8,0,0xff000000,0);
  
  sh_fb(g.fb.v,FBW,FBH);
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
