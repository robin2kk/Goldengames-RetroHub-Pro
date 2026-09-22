#include "diagnostic_frame.h"
#include "console_ui.h"
#include "rom_scanner.h"
#include "pad_ps5.h"
#include <unistd.h>
extern "C" int sceSystemServiceHideSplashScreen(void);
extern "C" int gg_platform_display_open(unsigned,unsigned);
extern "C" int gg_platform_display_present(void);
int main(){
 if(!gg_platform_display_open(1920,1080))for(;;)usleep(1000000);
 sceSystemServiceHideSplashScreen();gg_pad_open();
 const char*ids[5]={"nes","snes","n64","genesis","psx"};
 GGGameList lists[5];for(int i=0;i<5;i++)gg_scan_games(ids[i],&lists[i]);
 GGSurface s={1920,1080};int system=0,game=0;
 for(;;){
  uint32_t p=gg_pad_pressed();
  if(p&GG_PAD_LEFT){if(game>0)game--;}
  if(p&GG_PAD_RIGHT){if(game+1<lists[system].count)game++;}
  if(p&GG_PAD_CIRCLE){system=(system+1)%5;game=0;}
  gg_draw_console_browser(s,system,game);
  if(!gg_platform_display_present())for(;;)usleep(1000000);
  usleep(16000);
 }
}
