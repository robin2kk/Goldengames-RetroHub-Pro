#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "retrohub_ui.h"
int sceNotificationSend(int userId, _Bool isLogged, const char *payload);
int sceSystemServiceHideSplashScreen(void);
static void ui_notify(const char *s){char p[1024];snprintf(p,sizeof(p),"{\"rawData\":{\"viewTemplateType\":\"InteractiveToastTemplateB\",\"channelType\":\"Downloads\",\"useCaseId\":\"IDC\",\"toastOverwriteType\":\"No\",\"isImmediate\":true,\"priority\":100,\"viewData\":{\"message\":{\"body\":\"%s\"}},\"platformViews\":{\"previewDisabled\":{\"viewData\":{\"message\":{\"body\":\"%s\"}}}}},\"localNotificationId\":\"786420029\"}",s,s);sceNotificationSend(0xFE,1,p);}

#define UI_LOG "/data/goldengames_retrohub_ui.log"

static void ui_log(const char *msg){
 FILE *fp=fopen(UI_LOG,"a");
 if(!fp)return;
 fprintf(fp,"[ui] %s\n",msg?msg:"(null)");
 fclose(fp);
}
static void ui_log_sdl(const char *stage){
 char b[1024];
 snprintf(b,sizeof(b),"%s: %s",stage,SDL_GetError());
 ui_log(b);
}

int main(void){
 SDL_Window*w=0;SDL_Renderer*r=0;SDL_Event e;RetroHubState s;int run=1;
 remove(UI_LOG);ui_notify("RetroHub UI 1 main reached");
 ui_log("Goldengames RetroHub UI start");
 memset(&s,0,sizeof(s));s.screen=RH_SYSTEMS;
 putenv("SDL_VIDEODRIVER=ps5");SDL_SetHint("SDL_VIDEODRIVER","ps5");ui_log("SDL_VIDEODRIVER=ps5");
 ui_log("calling SDL_Init");
 if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER)<0){ui_notify("RetroHub UI FAILED SDL Init");ui_log_sdl("SDL_Init FAILED");return 1;}
 ui_notify("RetroHub UI 2 SDL OK");ui_log("SDL_Init OK");
 ui_log("creating window 1920x1080");
 w=SDL_CreateWindow("Goldengames RetroHub Pro",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1920,1080,SDL_WINDOW_FULLSCREEN);
 if(!w){ui_notify("RetroHub UI FAILED window");ui_log_sdl("SDL_CreateWindow FAILED");SDL_Quit();return 2;}
 ui_notify("RetroHub UI 3 window OK");ui_log("SDL_CreateWindow OK");sceSystemServiceHideSplashScreen();ui_log("sceSystemServiceHideSplashScreen called");
 ui_log("creating accelerated renderer");
 r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE|SDL_RENDERER_TARGETTEXTURE);
 if(!r){ui_log_sdl("software target renderer FAILED; trying vsync");r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE|SDL_RENDERER_PRESENTVSYNC);}
 if(!r){ui_log_sdl("software vsync renderer FAILED; trying software");r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE);}
 if(!r){ui_notify("RetroHub UI FAILED renderer");ui_log_sdl("software renderer FAILED");SDL_DestroyWindow(w);SDL_Quit();return 3;}
 ui_notify("RetroHub UI 4 renderer OK");ui_log("renderer OK");
 SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);ui_notify("RetroHub UI 5 entering loop");ui_log("entering main loop");
 while(run){
  while(SDL_PollEvent(&e)){
   if(e.type==SDL_QUIT)run=0;
   if(e.type==SDL_CONTROLLERBUTTONDOWN){
    if(e.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_LEFT)retrohub_move(&s,-1);
    else if(e.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_RIGHT)retrohub_move(&s,1);
    else if(e.cbutton.button==SDL_CONTROLLER_BUTTON_A)retrohub_activate(&s);
    else if(e.cbutton.button==SDL_CONTROLLER_BUTTON_B){if(s.screen==RH_GAMES)retrohub_back(&s);else run=0;}
   }
  }
  retrohub_render(r,&s);SDL_RenderPresent(r);SDL_Delay(8);
 }
 ui_log("leaving main loop");
 SDL_DestroyRenderer(r);SDL_DestroyWindow(w);SDL_Quit();ui_log("UI exit");return 0;
}
