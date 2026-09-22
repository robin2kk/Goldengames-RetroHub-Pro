#include <SDL.h>
#include <string.h>
#include "retrohub_ui.h"
int main(void){
 SDL_Window*w=0;SDL_Renderer*r=0;SDL_Event e;RetroHubState s;int run=1;
 memset(&s,0,sizeof(s));s.screen=RH_SYSTEMS;
 if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER)<0)return 1;
 w=SDL_CreateWindow("Goldengames RetroHub Pro",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1920,1080,0);
 r=SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
 if(!r)r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE);
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
 SDL_DestroyRenderer(r);SDL_DestroyWindow(w);SDL_Quit();return 0;
}
