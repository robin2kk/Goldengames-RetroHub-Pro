/* GoldenGames wallpaper adapter for the GPL-3.0-or-later ProsperoLight PNG ABI. */
#include "wallpaper.h"
#include "ps5_pngdec.hpp"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" int sceSysmoduleLoadModule(uint16_t module_id);

namespace {
constexpr unsigned W=1920,H=1080,IMAGE_W=1200,IMAGE_H=675;
const char* names[19]={
 "nes","snes","n64","gb","gbc","gba","genesis","segacd","x32",
 "saturn","psx","atari2600",nullptr,nullptr,nullptr,"pce","arcade",nullptr,nullptr};
int loaded_system=-1;
bool decoder_ready=false,decoder_checked=false;
unsigned char *pixels=nullptr;
unsigned char *cover=nullptr;
unsigned cover_w=0,cover_h=0;
char cover_key[600]="";
bool cover_checked=false;
const char* playlist_names[19]={
 "Nintendo - Nintendo Entertainment System","Nintendo - Super Nintendo Entertainment System","Nintendo - Nintendo 64",
 "Nintendo - Game Boy","Nintendo - Game Boy Color","Nintendo - Game Boy Advance",
 "Sega - Mega Drive - Genesis","Sega - Mega-CD - Sega CD","Sega - 32X",
 "Sega - Saturn","Sony - PlayStation","Atari - 2600","Atari - 7800",
 "Atari - Lynx","Atari - Jaguar","NEC - PC Engine - TurboGrafx 16",
 "Arcade","Commodore - Amiga","Commodore - 64"};

bool decode_cover(const char*path){
 FILE*f=fopen(path,"rb");if(!f)return false;
 if(fseek(f,0,SEEK_END)!=0){fclose(f);return false;}
 long len=ftell(f);
 if(len<=0||len>8*1024*1024||fseek(f,0,SEEK_SET)!=0){fclose(f);return false;}
 unsigned char*png=(unsigned char*)malloc((size_t)len);
 if(!png){fclose(f);return false;}
 bool ok=fread(png,1,(size_t)len,f)==(size_t)len;fclose(f);
 if(!ok){free(png);return false;}
 ScePngDecParseParam parse{png,(uint32_t)len,0};ScePngDecImageInfo info{};
 if(scePngDecParseHeader(&parse,&info)<0||!info.image_width||!info.image_height||
    info.image_width>2048||info.image_height>2048||
    (uint64_t)info.image_width*info.image_height*4>16*1024*1024){free(png);return false;}
 unsigned w=info.image_width,h=info.image_height;
 ScePngDecCreateParam create{sizeof(ScePngDecCreateParam),info.bit_depth>8?1u:0u,w};
 int size=scePngDecQueryMemorySize(&create);
 if(size<=0||size>16*1024*1024){free(png);return false;}
 void*work=malloc((size_t)size),*handle=nullptr;
 if(!work||scePngDecCreate(&create,work,(uint32_t)size,&handle)<0){free(work);free(png);return false;}
 unsigned char*decoded=(unsigned char*)malloc((size_t)w*h*4);
 if(!decoded){scePngDecDelete(handle);free(work);free(png);return false;}
 ScePngDecDecodeParam decode{png,decoded,(uint32_t)len,w*h*4,1,255,w*4};
 ScePngDecImageInfo output{};ok=scePngDecDecode(handle,&decode,&output)>=0;
 scePngDecDelete(handle);free(work);free(png);
 if(!ok){free(decoded);return false;}
 cover=decoded;cover_w=w;cover_h=h;return true;
}

bool find_cover(int system,const char*title){
 if(system<0||system>=19||!title||!title[0])return false;
 if(!decoder_checked){decoder_ready=sceSysmoduleLoadModule(0x008c)>=0;decoder_checked=true;}
 if(!decoder_ready)return false;
 char safe[512];size_t n=strlen(title);if(n>=sizeof(safe))return false;
 for(size_t i=0;i<=n;i++){
  char c=title[i];safe[i]=(c=='&'||c=='*'||c=='/'||c==':'||c=='<'||c=='>'||c=='?'||c=='\\'||c=='|')?'_':c;
 }
 char path[1024];
 if(names[system]&&snprintf(path,sizeof(path),"/app0/boxarts/%s/%s.png",names[system],safe)<(int)sizeof(path)&&decode_cover(path))return true;
 /* The payload's thumbnail directory may be readable on some installations. */
 const char*roots[]={"/data/homebrew/RetroArch/thumbnails","/data/homebrew/RetroArch/.config/retroarch/thumbnails"};
 for(const char*root:roots){
  if(snprintf(path,sizeof(path),"%s/%s/Named_Boxarts/%s.png",root,playlist_names[system],safe)<(int)sizeof(path)&&decode_cover(path))return true;
 }
 char*tag=strchr(safe,'(');if(tag){while(tag>safe&&tag[-1]==' ')tag--;*tag=0;
  if(names[system]&&snprintf(path,sizeof(path),"/app0/boxarts/%s/%s.png",names[system],safe)<(int)sizeof(path)&&decode_cover(path))return true;
  for(const char*root:roots){
   if(snprintf(path,sizeof(path),"%s/%s/Named_Boxarts/%s.png",root,playlist_names[system],safe)<(int)sizeof(path)&&decode_cover(path))return true;
  }
 }
 return false;
}

bool load(int system){
 free(pixels);pixels=nullptr;loaded_system=system;
 if(system<0||system>=19||!names[system])return false;
 if(!decoder_checked){decoder_ready=sceSysmoduleLoadModule(0x008c)>=0;decoder_checked=true;}
 if(!decoder_ready)return false;
 char path[128];
 if(snprintf(path,sizeof(path),"/app0/backgrounds/%s.png",names[system])>=(int)sizeof(path))return false;
 FILE *f=fopen(path,"rb");if(!f)return false;
 if(fseek(f,0,SEEK_END)!=0){fclose(f);return false;}
 long len=ftell(f);
 if(len<=0||len>8*1024*1024||fseek(f,0,SEEK_SET)!=0){fclose(f);return false;}
 unsigned char *png=(unsigned char*)malloc((size_t)len);
 if(!png){fclose(f);return false;}
 bool ok=fread(png,1,(size_t)len,f)==(size_t)len;
 fclose(f);
 if(!ok){free(png);return false;}
 ScePngDecParseParam parse{png,(uint32_t)len,0};ScePngDecImageInfo info{};
 if(scePngDecParseHeader(&parse,&info)<0||info.image_width!=IMAGE_W||info.image_height!=IMAGE_H){free(png);return false;}
 ScePngDecCreateParam create{sizeof(ScePngDecCreateParam),info.bit_depth>8?1u:0u,IMAGE_W};
 int work_size=scePngDecQueryMemorySize(&create);
 if(work_size<=0||work_size>16*1024*1024){free(png);return false;}
 void *work=malloc((size_t)work_size),*handle=nullptr;
 if(!work||scePngDecCreate(&create,work,(uint32_t)work_size,&handle)<0){free(work);free(png);return false;}
 pixels=(unsigned char*)malloc((size_t)IMAGE_W*IMAGE_H*4);
 if(!pixels){scePngDecDelete(handle);free(work);free(png);return false;}
 ScePngDecDecodeParam decode{png,pixels,(uint32_t)len,IMAGE_W*IMAGE_H*4,1,255,IMAGE_W*4};
 ScePngDecImageInfo output{};
 ok=scePngDecDecode(handle,&decode,&output)>=0;
 scePngDecDelete(handle);free(work);free(png);
 if(!ok){free(pixels);pixels=nullptr;}
 return ok;
}
}
extern "C" int gg_draw_wallpaper(GGSurface surface,int system){
 if(surface.width!=W||surface.height!=H||system<0||system>=19)return 0;
 if(loaded_system!=system)load(system);
 if(!pixels)return 0;
 for(unsigned y=0;y<H;y++)for(unsigned x=0;x<W;x++){
  const unsigned char *p=pixels+((size_t)(y*IMAGE_H/H)*IMAGE_W+(x*IMAGE_W/W))*4;
  gg_platform_put_pixel(x,y,0xff000000u|((uint32_t)p[2]<<16)|((uint32_t)p[1]<<8)|p[0]);
 }
 return 1;
}
extern "C" int gg_draw_boxart(GGSurface surface,int system,const char*title,int x,int y,int w,int h){
 if(!title||w<=0||h<=0)return 0;
 char key[600];if(snprintf(key,sizeof(key),"%d:%s",system,title)>=(int)sizeof(key))return 0;
 if(!cover_checked||strcmp(key,cover_key)!=0){
  free(cover);cover=nullptr;cover_w=cover_h=0;
  snprintf(cover_key,sizeof(cover_key),"%s",key);cover_checked=true;
  find_cover(system,title);
 }
 if(!cover)return 0;
 double scale=(double)w/cover_w;if((double)h/cover_h<scale)scale=(double)h/cover_h;
 int dw=(int)(cover_w*scale),dh=(int)(cover_h*scale);
 if(dw<=0||dh<=0)return 0;
 int ox=x+(w-dw)/2,oy=y+(h-dh)/2;
 for(int yy=0;yy<dh;yy++)for(int xx=0;xx<dw;xx++){
  int dx=ox+xx,dy=oy+yy;if(dx<0||dy<0||dx>=(int)surface.width||dy>=(int)surface.height)continue;
  const unsigned char*p=cover+((size_t)(yy*cover_h/dh)*cover_w+(xx*cover_w/dw))*4;
  if(p[3]<128)continue;
  gg_platform_put_pixel(dx,dy,0xff000000u|((uint32_t)p[2]<<16)|((uint32_t)p[1]<<8)|p[0]);
 }
 return 1;
}
