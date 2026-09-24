#include "diagnostic_frame.h"
#include "console_ui.h"
#include "rom_scanner.h"
#include "pad_ps5.h"
#include "retroarch_launch.h"
#include <unistd.h>
#include <stdio.h>

extern "C" int sceSystemServiceHideSplashScreen(void);
extern "C" int gg_platform_display_open(unsigned,unsigned);
extern "C" int gg_platform_display_present(void);

static const char* kSystems[]={"nes","snes","n64","genesis","psx"};
static const int kSystemCount=5;
static const char* kRetroArchTitleId="PPSA99169";\nstatic int launch_status=0;
static const char* kCores[]={
 "fceumm_libretro.so",
 "snes9x_libretro.so",
 "parallel_n64_libretro.so",
 "genesis_plus_gx_libretro.so",
 "pcsx_rearmed_libretro.so"
};

static void gg_build_launch_paths(int system,const GGGameList*list,int selected,
                                  char*core,size_t core_n,char*content,size_t content_n){
 if(!list||list->count<=0||selected<0||selected>=list->count){
   if(core_n)core[0]=0;if(content_n)content[0]=0;return;
 }
 snprintf(core,core_n,"/app0/cores/%s",kCores[system]);
 snprintf(content,content_n,"%s",list->games[selected].filename);
}

int main(){
 if(!gg_platform_display_open(1920,1080))for(;;)usleep(1000000);
 sceSystemServiceHideSplashScreen();
 GGSurface s={1920,1080};
 int system=0,selected_game=0;
 GGGameList games[kSystemCount];
 for(int i=0;i<kSystemCount;i++)gg_scan_games(kSystems[i],&games[i]);
 int pad_ok=gg_pad_open();
 for(;;){
   uint32_t p=pad_ok?gg_pad_pressed():0;
   if(p&GG_PAD_L1){system=(system+kSystemCount-1)%kSystemCount;selected_game=0;}
   if(p&GG_PAD_R1){system=(system+1)%kSystemCount;selected_game=0;}
   if((p&GG_PAD_LEFT)&&games[system].count>0)
     selected_game=(selected_game+games[system].count-1)%games[system].count;
   if((p&GG_PAD_RIGHT)&&games[system].count>0)
     selected_game=(selected_game+1)%games[system].count;
   if((p&GG_PAD_CROSS)&&games[system].count>0){
     char core[512],content[512];
     gg_build_launch_paths(system,&games[system],selected_game,core,sizeof(core),content,sizeof(content));
     launch_status=gg_launch_retroarch(kRetroArchTitleId,core,content);
   }
   gg_draw_console_browser(s,system,selected_game,&games[system]);
   if(!gg_platform_display_present())for(;;)usleep(1000000);
   usleep(16000);
 }
}
