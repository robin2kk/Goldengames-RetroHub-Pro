#include "console_ui.h"
#include "wallpaper.h"
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
static void rect(GGSurface s,int x,int y,int w,int h,uint32_t c){if(x<0){w+=x;x=0;}if(y<0){h+=y;y=0;}if(x+w>(int)s.width)w=s.width-x;if(y+h>(int)s.height)h=s.height-y;if(w<=0||h<=0)return;for(int yy=y;yy<y+h;yy++)for(int xx=x;xx<x+w;xx++)gg_platform_put_pixel(xx,yy,c);}
static void line(GGSurface s,int x,int y,int w,int h,uint32_t c){rect(s,x,y,w,h,c);}
static void nes(GGSurface s){rect(s,0,0,s.width,s.height,0xffd8d8d8);rect(s,0,0,s.width,170,0xffeeeeee);rect(s,0,170,s.width,95,0xffb21f2d);rect(s,0,265,s.width,8,0xff4b4b4b);rect(s,0,900,s.width,180,0xff292929);line(s,80,70,380,12,0xffb21f2d);line(s,80,96,250,7,0xff555555);}
static void snes(GGSurface s){rect(s,0,0,s.width,s.height,0xffc9c8cc);rect(s,0,0,s.width,170,0xffe5e4e8);rect(s,0,170,s.width,105,0xff6b4d91);rect(s,0,275,s.width,10,0xffa89bc2);rect(s,0,900,s.width,180,0xff3b3940);for(int i=0;i<4;i++)rect(s,1500+i*55,82,34,34,i<2?0xff6651a0:0xff9a74bb);}
static void n64(GGSurface s){rect(s,0,0,s.width,s.height,0xff17191d);rect(s,0,0,s.width,185,0xff24272c);rect(s,0,185,s.width,9,0xffd73535);rect(s,0,194,s.width,9,0xff38a857);rect(s,0,203,s.width,9,0xffe5b82e);rect(s,0,212,s.width,9,0xff347bd1);rect(s,0,900,s.width,180,0xff0b0c0e);}
static void genesis(GGSurface s){rect(s,0,0,s.width,s.height,0xff08090b);rect(s,0,0,s.width,190,0xff17191d);rect(s,0,190,s.width,16,0xffc62828);rect(s,0,206,s.width,5,0xffeeeeee);rect(s,0,900,s.width,180,0xff111214);for(int x=80;x<1840;x+=48)rect(s,x,75,28,4,0xff4b4e54);}
static void psx(GGSurface s){rect(s,0,0,s.width,s.height,0xffbcbcbc);rect(s,0,0,s.width,180,0xffd8d8d8);rect(s,0,180,s.width,8,0xff686868);rect(s,0,900,s.width,180,0xff707070);rect(s,1540,75,28,28,0xff4ca3d9);rect(s,1585,75,28,28,0xffd95757);rect(s,1630,75,28,28,0xff61ad69);rect(s,1675,75,28,28,0xffd79ac2);}
static void card(GGSurface s,int x,int y,int w,int h,int active,uint32_t accent){if(active){rect(s,x-10,y-10,w+20,h+20,accent);rect(s,x-4,y-4,w+8,h+8,0xffffffff);}else rect(s,x-4,y-4,w+8,h+8,0xff55585e);rect(s,x,y,w,h,0xff20242c);rect(s,x+18,y+18,w-36,h-36,0xff303640);}
static uint32_t game_color(const GGGame *g,int n){uint32_t h=2166136261u;const unsigned char*p=(const unsigned char*)g->filename;while(*p){h^=*p++;h*=16777619u;}h^=(uint32_t)n*0x9e3779b9u;return 0xff000000u|0x303030u|(h&0x00cfcfcfu);}
static void upper_title(const char*src,char*out,unsigned cap){unsigned i=0;while(src[i]&&i+1<cap){out[i]=(char)toupper((unsigned char)src[i]);i++;}out[i]=0;}
void gg_draw_console_browser(GGSurface s,int system,int selected,const GGGameList *list,int launch_status){
 static const uint32_t accents[19]={
 0xffb21f2d,0xff6b4d91,0xffe5b82e,0xff7b2f8e,0xff159a8c,0xff563b8f,
 0xffc62828,0xff2d64a8,0xffd83b32,0xff315f9e,0xff4ca3d9,0xff8b5a2b,
 0xffc7c7c7,0xff2d333b,0xffc91d32,0xffd9473f,0xffd12f87,0xffb9b3a7,0xff7b68a6};
 static const char *systems[19]={
 "NINTENDO ENTERTAINMENT SYSTEM","SUPER NINTENDO","NINTENDO 64",
 "GAME BOY","GAME BOY COLOR","GAME BOY ADVANCE","SEGA GENESIS","SEGA CD",
 "SEGA 32X","SEGA SATURN","PLAYSTATION","ATARI 2600","ATARI 7800",
 "ATARI LYNX","ATARI JAGUAR","PC ENGINE","ARCADE","AMIGA","COMMODORE 64"};
 if(gg_draw_wallpaper(s,system)){
  /* The supplied wallpaper reserves the left column for the selected game. */
  if(list&&list->count>0&&selected>=0&&selected<list->count){
   const GGGame *game=&list->games[selected];
   rect(s,72,380,328,382,0xffeeeeee);rect(s,78,386,316,370,game_color(game,selected));
   char title[39],counter[48];upper_title(game->title,title,sizeof(title));
   rect(s,22,804,465,96,0xff111111);
   gg_draw_text(s,38,820,title,2,0xffffffff);
   snprintf(counter,sizeof(counter),"GAME %d OF %d",selected+1,list->count);
   gg_draw_text(s,38,862,counter,2,0xffcccccc);
  }else{
   gg_draw_text(s,36,600,"NO GAMES FOUND",3,0xffffffff);
  }
  rect(s,0,940,s.width,140,0xff141414);
  if(list){char info[120];
   snprintf(info,sizeof(info),"GAMES %d  ROM FOLDER %s  ERR %d  MANIFEST %s",list->count,
    list->folder_found?"FOUND":"NOT FOUND",list->folder_error,list->manifest_found?"FOUND":"NOT FOUND");
   gg_draw_text(s,35,964,info,2,0xffffffff);
  }
  gg_draw_text(s,35,1035,"UP DOWN SYSTEM   LEFT RIGHT GAME   X PLAY   O RESCAN",2,0xffffffff);
  if(launch_status<0)gg_draw_text(s,1040,1035,"LAUNCH FAILED",2,0xffff7777);
  return;
 }
 switch(system){
  case 0:nes(s);break; case 1:snes(s);break; case 2:n64(s);break;
  case 6:case 7:case 8:case 9:genesis(s);break; case 10:psx(s);break;
  default:rect(s,0,0,s.width,s.height,0xffe8e8e8);break;
 }
 gg_draw_text(s,78,72,"GOLDENGAMES RETROHUB PRO",4,0xff20242c);
 gg_draw_text(s,78,128,systems[system],3,0xff20242c);
 int cy=350;
 for(int d=-2;d<=2;d++){
  int idx=selected+d;
  int w=d==0?330:220,h=d==0?430:300,x=960+d*310-w/2,y=cy+(d==0?0:65);
  if(list&&idx>=0&&idx<list->count){
   card(s,x,y,w,h,d==0,accents[system]);
   rect(s,x+28,y+28,w-56,h-56,game_color(&list->games[idx],idx));
  }else{
   card(s,x,y,w,h,0,accents[system]);
   rect(s,x,y,w,h,0xff111318);
  }
 }
 rect(s,0,860,s.width,6,accents[system]);
 if(list&&list->count>0&&selected>=0&&selected<list->count){
  char title[64];upper_title(list->games[selected].title,title,sizeof(title));gg_draw_text(s,80,890,title,3,0xffffffff);
 }else{
  gg_draw_text(s,80,890,"NO GAMES FOUND",2,0xffffffff);
 }
 if(list){
  char info[120];
  snprintf(info,sizeof(info),"GAMES %d  ROM FOLDER %s  ERR %d  MANIFEST %s",list->count,
    list->folder_found?"FOUND":"NOT FOUND",list->folder_error,list->manifest_found?"FOUND":"NOT FOUND");
  gg_draw_text(s,80,950,info,2,0xffffffff);
 }
 gg_draw_text(s,80,1000,"UP DOWN SYSTEM   LEFT RIGHT GAME   X PLAY   O RESCAN",2,0xffffffff);
 if(launch_status<0)gg_draw_text(s,900,1000,"LAUNCH FAILED - CHECK WEBSRV AND RETROARCH",2,0xffff7777);
 if(list){
  int n=list->count;if(n>40)n=40;
  for(int i=0;i<n;i++)rect(s,80+i*18,820,10,22,accents[system]);
 }
}
