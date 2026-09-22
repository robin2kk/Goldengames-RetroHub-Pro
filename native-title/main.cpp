#include "diagnostic_frame.h"
#include "pad_ps5.h"
#include <unistd.h>
extern "C" int sceSystemServiceHideSplashScreen(void);
extern "C" int gg_platform_display_open(unsigned,unsigned);
extern "C" int gg_platform_display_present(void);
int main(){
 if(!gg_platform_display_open(1920,1080))for(;;)usleep(1000000);
 sceSystemServiceHideSplashScreen();
 GGSurface s={1920,1080};
 int selected=2;
 int pad_ok=gg_pad_open();
 for(;;){
   uint32_t p=pad_ok?gg_pad_pressed():0;
   if((p&GG_PAD_LEFT)&&selected>0)selected--;
   if((p&GG_PAD_RIGHT)&&selected<4)selected++;
   gg_draw_retrohub_frame(s,selected);
   if(!gg_platform_display_present())for(;;)usleep(1000000);
   usleep(16000);
 }
}
