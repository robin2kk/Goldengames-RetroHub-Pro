/* RetroHub HD coverflow. All artwork is loaded from the owner's PNG files. */
#include "console_ui.h"
#include "wallpaper.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void rect(GGSurface s,int x,int y,int w,int h,uint32_t c){
 if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}
 if(x+w>(int)s.width)w=s.width-x;
 if(y+h>(int)s.height)h=s.height-y;
 if(w<=0||h<=0)return;
 for(int yy=y;yy<y+h;yy++)for(int xx=x;xx<x+w;xx++)gg_platform_put_pixel(xx,yy,c);
}
static void label(GGSurface s,int x,int y,const char*source,int max,int scale,uint32_t color){
 char text[96];int n=0;
 while(source&&source[n]&&n<max&&n<(int)sizeof(text)-1){
  unsigned char c=(unsigned char)source[n];
  text[n]=(char)((c>=32&&c<=126)?toupper(c):' ');n++;
 }
 text[n]=0;gg_draw_text(s,x,y,text,scale,color);
}
static void centered(GGSurface s,int center,int y,const char*source,int max,int scale,uint32_t color){
 int n=0;while(source&&source[n]&&n<max)n++;
 label(s,center-n*3*scale,y,source,max,scale,color);
}
static void bevel(GGSurface s,int x,int y,int w,int h,uint32_t edge){
 rect(s,x-18,y-13,w+36,h+38,0xff070b13);
 rect(s,x-10,y-8,w+20,h+22,0xff141c2a);
 rect(s,x-3,y-3,w+6,h+6,edge);
 rect(s,x,y,w,h,0xff111b29);
 rect(s,x+6,y+6,w-12,h-12,0xff1d2838);
}
static void sleeve(GGSurface s,int x,int y,int w,int h,const char*system,const char*title,uint32_t accent){
 rect(s,x,y,w,h,0xff18253a);
 rect(s,x,y,w,10,accent);
 for(int i=0;i<5;i++)rect(s,x+i*13,y+28+i*8,w-i*28,2,0xff263c57);
 rect(s,x+25,y+h/3,w-50,2,accent);
 centered(s,x+w/2,y+h/3+30,"RETROHUB",8,w>300?4:2,0xffe8f1ff);
 centered(s,x+w/2,y+h/3+92,system,w>300?25:16,w>300?2:1,0xffa6bdd5);
 if(w>300)centered(s,x+w/2,y+h-72,title,31,2,0xfff0f4ff);
 rect(s,x+25,y+h-24,w-50,3,accent);
}
static void card(GGSurface s,int system,int index,const GGGameList*list,int x,int y,int w,int h,int focus,const char*system_name,uint32_t accent){
 if(index<0||!list||index>=list->count)return;
 const GGGame*game=&list->games[index];
 uint32_t edge=focus?0xfff7f8fa:0xff43516a;
 bevel(s,x,y,w,h,edge);
 if(focus){rect(s,x-3,y-3,w+6,5,accent);rect(s,x-3,y+h-2,w+6,5,accent);}
 if(!gg_draw_boxart_tinted(s,system,game->title,x+8,y+8,w-16,h-16,focus?100:67))
  sleeve(s,x+8,y+8,w-16,h-16,system_name,game->title,accent);
 /* A narrow glass highlight reads like a physical case without hiding art. */
 rect(s,x+4,y+4,4,h-8,0xff627184);
 rect(s,x+4,y+4,w-8,2,0xff8795a7);
 if(focus){
  rect(s,x-15,y+h+17,w+30,1,0xff54667b);
  gg_draw_boxart_reflection(s,system,game->title,x+9,y+h+27,w-18,76);
 }
}
void gg_draw_console_browser(GGSurface s,int system,int selected,const GGGameList*list,int launch_status){
 static const uint32_t accent[19]={
  0xffe65d53,0xff9b79ec,0xffe3bd51,0xffa783d6,0xff66d1c7,0xffae8be6,
  0xffe66a59,0xff73a3e0,0xffe67654,0xff719dec,0xff66b4eb,0xffb58b65,
  0xffd8dbe2,0xff8394a9,0xffe86966,0xffed8464,0xffe877a3,0xffc9c1b4,0xffaa91d3};
 static const char*systems[19]={
  "NINTENDO ENTERTAINMENT SYSTEM","SUPER NINTENDO","NINTENDO 64",
  "GAME BOY","GAME BOY COLOR","GAME BOY ADVANCE","SEGA GENESIS","SEGA CD",
  "SEGA 32X","SEGA SATURN","PLAYSTATION","ATARI 2600","ATARI 7800",
  "ATARI LYNX","ATARI JAGUAR","PC ENGINE","ARCADE","AMIGA","COMMODORE 64"};
 if(system<0||system>=19)return;
 gg_draw_wallpaper(s,system);
 uint32_t theme=accent[system];
 rect(s,0,0,s.width,6,theme);
 rect(s,0,0,8,s.height,theme);
 /* Header and navigation live on a clean, 1080p canvas above the artwork. */
 rect(s,0,6,s.width,153,0xff0c1423);
 rect(s,82,54,5,65,theme);
 gg_draw_text(s,108,48,"GOLDENGAMES",3,0xff92a5c0);
 gg_draw_text(s,108,88,"RETROHUB",6,0xfff2f6fc);
 gg_draw_text(s,1240,67,"SYSTEM",2,0xff8ea0b7);
 label(s,1240,100,systems[system],27,2,0xffeef3fa);
 rect(s,82,153,1750,2,0xff3b4b62);
 rect(s,82,153,240,4,theme);
 /* Low-profile floor under the physical box art. */
 rect(s,0,854,s.width,226,0xff0b121e);
 rect(s,82,856,1750,2,0xff35475d);
 rect(s,82,989,1750,2,0xff344158);
 if(list&&list->count>0&&selected>=0&&selected<list->count){
  /* Draw from the outside in so the focused cover is always on top. */
  card(s,system,selected-2,list,193,370,186,275,0,systems[system],theme);
  card(s,system,selected+2,list,1541,370,186,275,0,systems[system],theme);
  card(s,system,selected-1,list,432,299,270,390,0,systems[system],theme);
  card(s,system,selected+1,list,1218,299,270,390,0,systems[system],theme);
  card(s,system,selected,list,755,213,410,537,1,systems[system],theme);
  centered(s,960,885,list->games[selected].title,43,3,0xfff5f7fa);
  char count[48];snprintf(count,sizeof(count),"GAME %d OF %d",selected+1,list->count);
  centered(s,960,945,count,32,2,0xff9badc5);
 }else{
  centered(s,960,473,"NO GAMES FOUND",20,5,0xffe7edf6);
  centered(s,960,540,"PRESS CIRCLE TO RESCAN",25,2,0xffa1b4d0);
 }
 label(s,90,1023,systems[system],29,2,0xff9cafc5);
 gg_draw_text(s,930,1023,"L1 R1 SYSTEM   LEFT RIGHT GAME   X PLAY   O RESCAN",2,0xffb9c7da);
 if(launch_status<0){
  rect(s,1420,161,438,44,0xff65202b);
  gg_draw_text(s,1440,177,"LAUNCH FAILED",2,0xffffffff);
 }
}
