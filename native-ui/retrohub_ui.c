#include "retrohub_ui.h"
#include <stdio.h>
#include <string.h>
static void color(SDL_Renderer*r,unsigned char R,unsigned char G,unsigned char B,unsigned char A){SDL_SetRenderDrawColor(r,R,G,B,A);}
static void rect(SDL_Renderer*r,int x,int y,int w,int h){SDL_Rect q={x,y,w,h};SDL_RenderFillRect(r,&q);}
void retrohub_enter_system(RetroHubState*s){
 const RetroSystem *sys=&RETRO_SYSTEMS[s->system_index];
 s->game_count=retrohub_scan_games(sys->id,sys->extensions,s->games,RETROHUB_MAX_GAMES);
 s->game_index=0;s->carousel=0;s->screen=RH_GAMES;
}
void retrohub_back(RetroHubState*s){if(s->screen==RH_GAMES){s->screen=RH_SYSTEMS;s->game_count=0;s->game_index=0;}}
void retrohub_move(RetroHubState*s,int d){
 if(s->screen==RH_SYSTEMS){int n=(int)RETRO_SYSTEM_COUNT;s->system_index=(s->system_index+d+n)%n;}
 else if(s->game_count){s->game_index=(s->game_index+d+s->game_count)%s->game_count;}
}
void retrohub_activate(RetroHubState*s){
 if(s->screen==RH_SYSTEMS){retrohub_enter_system(s);return;}
 if(s->game_count){const RetroSystem*sys=&RETRO_SYSTEMS[s->system_index];retrohub_write_launch_request(sys->core,s->games[s->game_index].path);}
}
void retrohub_render(SDL_Renderer*r,RetroHubState*s){
 const RetroSystem *sys=&RETRO_SYSTEMS[s->system_index]; int i;
 color(r,sys->bg_r,sys->bg_g,sys->bg_b,255);SDL_RenderClear(r);
 /* Header + console identity stripe. Text/box-art layers plug into NativeHBL TTF/IMG renderer. */
 color(r,sys->accent_r,sys->accent_g,sys->accent_b,255);rect(r,0,155,1920,12);
 if(s->screen==RH_SYSTEMS){
  for(i=-2;i<=2;i++){int idx=(s->system_index+i+(int)RETRO_SYSTEM_COUNT)%(int)RETRO_SYSTEM_COUNT;int w=i?230:330,h=i?300:430,x=960+i*300-w/2,y=360+(i?55:0);(void)idx;color(r,i?80:sys->accent_r,i?80:sys->accent_g,i?85:sys->accent_b,255);rect(r,x,y,w,h);}
 } else {
  for(i=-2;i<=2;i++){int w=i?220:330,h=i?315:465,x=960+i*285-w/2,y=330+(i?70:0);color(r,i?72:sys->accent_r,i?72:sys->accent_g,i?78:sys->accent_b,255);rect(r,x,y,w,h);}
 }
 color(r,12,12,16,220);rect(r,0,965,1920,115);
}
