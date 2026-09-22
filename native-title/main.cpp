#include "diagnostic_frame.h"
#include "pad_ps5.h"
#include <unistd.h>
extern "C" int sceSystemServiceHideSplashScreen(void);extern "C" int gg_platform_display_open(unsigned,unsigned);extern "C" int gg_platform_display_present(void);
int main(){if(!gg_platform_display_open(1920,1080))for(;;)usleep(1000000);sceSystemServiceHideSplashScreen();gg_pad_open();GGSurface s={1920,1080};int selected=2;for(;;){uint32_t p=gg_pad_pressed();if((p&GG_PAD_LEFT)&&selected>0)selected--;if((p&GG_PAD_RIGHT)&&selected<4)selected++;gg_draw_retrohub_frame(s,selected);if(!gg_platform_display_present())for(;;)usleep(1000000);usleep(16000);}}
