#include "diagnostic_frame.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
extern "C" int sceSystemServiceHideSplashScreen(void);

/* Platform display adapter intentionally kept separate from the GoldenGames renderer.
 * The next build step binds these three functions to the native PS5 title display. */
extern "C" int gg_platform_display_open(unsigned width,unsigned height);
extern "C" uint32_t *gg_platform_display_backbuffer(unsigned *pitch_pixels);
extern "C" int gg_platform_display_present(void);

int main(void){
 if(!gg_platform_display_open(1920,1080)) return 2;
 sceSystemServiceHideSplashScreen();
 for(;;){
  unsigned pitch=0; uint32_t *pixels=gg_platform_display_backbuffer(&pitch);
  GGSurface s={pixels,1920,1080,pitch};
  gg_draw_acceptance_frame(s);
  gg_platform_display_present();
  usleep(16000);
 }
}
