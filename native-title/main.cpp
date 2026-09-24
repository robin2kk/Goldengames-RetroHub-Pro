#include "diagnostic_frame.h"
#include "console_ui.h"
#include "rom_scanner.h"
#include "pad_ps5.h"
#include "websrv_launch.h"
#include <unistd.h>
#include <stdio.h>

extern "C" int sceSystemServiceHideSplashScreen(void);
extern "C" int gg_platform_display_open(unsigned,unsigned);
extern "C" int gg_platform_display_present(void);

static const char* kSystems[]={"nes","snes","n64","genesis","psx"};
static const int kSystemCount=5;
static int launch_status=0;
static const char* kCores[]={
 "fceumm_libretro.so",
 "snes9x_libretro.so",
 "parallel_n64_libretro.so",
 "genesis_plus_gx_libretro.so",
 "pcsx_rearmed_libretro.so"
};

int main(){
 if(!gg_platform_display_open(1920,1080))for(;;)usleep(1000000);
 sceSystemServiceHideSplashScreen();
 GGSurface s={1920,1080};
 int system=0,selected_game=0;
 GGGameList games[kSystemCount];
 for(int i=0;i<kSystemCount;i++)gg_scan_games(kSystems[i],&games[i]);
 int pad_ok=gg_pad_open();
 int rescan_requested=0;
 for(;;){
   uint32_t p=pad_ok?gg_pad_pressed():0;
   if(rescan_requested){for(int i=0;i<kSystemCount;i++)gg_scan_games(kSystems[i],&games[i]);selected_game=0;rescan_requested=0;}
   if(p&GG_PAD_L1){system=(system+kSystemCount-1)%kSystemCount;selected_game=0;}
   if(p&GG_PAD_R1){system=(system+1)%kSystemCount;selected_game=0;}
   if((p&GG_PAD_LEFT)&&games[system].count>0)
     selected_game=(selected_game+games[system].count-1)%games[system].count;
   if((p&GG_PAD_RIGHT)&&games[system].count>0)
     selected_game=(selected_game+1)%games[system].count;
   if((p&GG_PAD_CROSS)&&games[system].count>0){
     launch_status=gg_launch_retroarch_payload(kCores[system],games[system].games[selected_game].filename);
   }
   gg_draw_console_browser(s,system,selected_game,&games[system]);
   if(!gg_platform_display_present())for(;;)usleep(1000000);
   usleep(16000);
 }
}
