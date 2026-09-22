#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include "retrohub_ui.h"

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
 remove(UI_LOG);
 ui_log("Goldengames RetroHub UI start");
 memset(&s,0,sizeof(s));s.screen=RH_SYSTEMS;
 ui_log("calling SDL_Init");
 if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER)<0){ui_log_sdl("SDL_Init FAILED");return 1;}
 ui_log("SDL_Init OK");
 ui_log("creating window 1920x1080");
 w=SDL_CreateWindow("Goldengames RetroHub Pro",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1920,1080,0);
 if(!w){ui_log_sdl("SDL_CreateWindow FAILED");SDL_Quit();return 2;}
 ui_log("SDL_CreateWindow OK");
 ui_log("creating accelerated renderer");
 r=SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
 if(!r){ui_log_sdl("accelerated renderer FAILED; trying software");r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE);}
 if(!r){ui_log_sdl("software renderer FAILED");SDL_DestroyWindow(w);SDL_Quit();return 3;}
 ui_log("renderer OK");
 ui_log("entering main loop");
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
