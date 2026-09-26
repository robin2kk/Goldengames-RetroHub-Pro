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
const char* system_ids[19]={
 "nes","snes","n64","gb","gbc","gba","genesis","segacd","x32",
 "saturn","psx","atari2600","atari7800","lynx","jaguar","pce","arcade","amiga","c64"};
int loaded_system=-1;
bool decoder_ready=false,decoder_checked=false;
unsigned char *pixels=nullptr;
struct Cover { unsigned char *pixels; unsigned w,h; char key[600]; bool checked; unsigned used; };
Cover covers[7]{};
unsigned cover_clock=0;
const char* playlist_names[19]={
 "Nintendo - Nintendo Entertainment System","Nintendo - Super Nintendo Entertainment System","Nintendo - Nintendo 64",
 "Nintendo - Game Boy","Nintendo - Game Boy Color","Nintendo - Game Boy Advance",
 "Sega - Mega Drive - Genesis","Sega - Mega-CD - Sega CD","Sega - 32X",
 "Sega - Saturn","Sony - PlayStation","Atari - 2600","Atari - 7800",
 "Atari - Lynx","Atari - Jaguar","NEC - PC Engine - TurboGrafx 16",
 "Arcade","Commodore - Amiga","Commodore - 64"};

bool decode_cover(const char*path,Cover&slot){
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
    info.image_width>1536||info.image_height>1536||
    (uint64_t)info.image_width*info.image_height*4>8*1024*1024){free(png);return false;}
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
 slot.pixels=decoded;slot.w=w;slot.h=h;return true;
}

bool find_cover(int system,const char*title,Cover&slot){
 if(system<0||system>=19||!title||!title[0])return false;
 if(!decoder_checked){decoder_ready=sceSysmoduleLoadModule(0x008c)>=0;decoder_checked=true;}
 if(!decoder_ready)return false;
 char safe[512];size_t n=strlen(title);if(n>=sizeof(safe))return false;
 for(size_t i=0;i<=n;i++){
  char c=title[i];safe[i]=(c=='&'||c=='*'||c=='/'||c==':'||c=='<'||c=='>'||c=='?'||c=='\\'||c=='|')?'_':c;
 }
 char path[1024];
 if(snprintf(path,sizeof(path),"/app0/boxarts/%s/%s.png",system_ids[system],safe)<(int)sizeof(path)&&decode_cover(path,slot))return true;
 /* The payload's thumbnail directory may be readable on some installations. */
 const char*roots[]={"/data/homebrew/RetroArch/thumbnails","/data/homebrew/RetroArch/.config/retroarch/thumbnails"};
 for(const char*root:roots){
  if(snprintf(path,sizeof(path),"%s/%s/Named_Boxarts/%s.png",root,playlist_names[system],safe)<(int)sizeof(path)&&decode_cover(path,slot))return true;
 }
 char*tag=strchr(safe,'(');if(tag){while(tag>safe&&tag[-1]==' ')tag--;*tag=0;
  if(snprintf(path,sizeof(path),"/app0/boxarts/%s/%s.png",system_ids[system],safe)<(int)sizeof(path)&&decode_cover(path,slot))return true;
  for(const char*root:roots){
   if(snprintf(path,sizeof(path),"%s/%s/Named_Boxarts/%s.png",root,playlist_names[system],safe)<(int)sizeof(path)&&decode_cover(path,slot))return true;
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
 /* Crop away the old embedded logo and black frames. The console illustration
    becomes an atmospheric backdrop, darkened so artwork stays legible. */
 for(unsigned y=0;y<H;y++)for(unsigned x=0;x<W;x++){
  unsigned shade=18+(x>W/2?6:0)+(y<H/2?3:0);
  unsigned r=8,g=13,b=25;
  if(pixels){
   unsigned sx=450+(x*750/W),sy=y*IMAGE_H/H;
   const unsigned char *p=pixels+((size_t)sy*IMAGE_W+sx)*4;
   r+=(unsigned)p[2]*shade/100;g+=(unsigned)p[1]*shade/100;b+=(unsigned)p[0]*shade/100;
  }
  gg_platform_put_pixel(x,y,0xff000000u|(r<<16)|(g<<8)|b);
 }
 return 1;
}
extern "C" int gg_draw_boxart_tinted(GGSurface surface,int system,const char*title,int x,int y,int w,int h,unsigned brightness){
 if(!title||w<=0||h<=0)return 0;
 char key[600];if(snprintf(key,sizeof(key),"%d:%s",system,title)>=(int)sizeof(key))return 0;
 Cover*slot=nullptr;
 for(Cover&c:covers)if(c.checked&&strcmp(c.key,key)==0){slot=&c;break;}
 if(!slot){
  slot=&covers[0];
  for(Cover&c:covers)if(!c.checked||c.used<slot->used){slot=&c;if(!c.checked)break;}
  free(slot->pixels);slot->pixels=nullptr;slot->w=slot->h=0;
  snprintf(slot->key,sizeof(slot->key),"%s",key);slot->checked=true;
  find_cover(system,title,*slot);
 }
 slot->used=++cover_clock;
 if(!slot->pixels)return 0;
 if(brightness>100)brightness=100;
 double scale=(double)w/slot->w;if((double)h/slot->h<scale)scale=(double)h/slot->h;
 int dw=(int)(slot->w*scale),dh=(int)(slot->h*scale);
 if(dw<=0||dh<=0)return 0;
 int ox=x+(w-dw)/2,oy=y+(h-dh)/2;
 for(int yy=0;yy<dh;yy++)for(int xx=0;xx<dw;xx++){
  int dx=ox+xx,dy=oy+yy;if(dx<0||dy<0||dx>=(int)surface.width||dy>=(int)surface.height)continue;
  const unsigned char*p=slot->pixels+((size_t)(yy*slot->h/dh)*slot->w+(xx*slot->w/dw))*4;
  if(p[3]<128)continue;
  gg_platform_put_pixel(dx,dy,0xff000000u|((uint32_t)(p[2]*brightness/100)<<16)|
                       ((uint32_t)(p[1]*brightness/100)<<8)|(uint32_t)(p[0]*brightness/100));
 }
 return 1;
}
extern "C" int gg_draw_boxart(GGSurface surface,int system,const char*title,int x,int y,int w,int h){
 return gg_draw_boxart_tinted(surface,system,title,x,y,w,h,100);
}
extern "C" int gg_draw_boxart_reflection(GGSurface surface,int system,const char*title,int x,int y,int w,int h){
 if(!title||w<=0||h<=0)return 0;
 char key[600];if(snprintf(key,sizeof(key),"%d:%s",system,title)>=(int)sizeof(key))return 0;
 const Cover*slot=nullptr;
 for(const Cover&c:covers)if(c.checked&&c.pixels&&strcmp(c.key,key)==0){slot=&c;break;}
 if(!slot)return 0;
 for(int yy=0;yy<h;yy++)for(int xx=0;xx<w;xx++){
  int dx=x+xx,dy=y+yy;if(dx<0||dy<0||dx>=(int)surface.width||dy>=(int)surface.height)continue;
  unsigned sx=(unsigned)((uint64_t)xx*slot->w/w),sy=slot->h-1-(unsigned)((uint64_t)yy*slot->h/h);
  const unsigned char*p=slot->pixels+((size_t)sy*slot->w+sx)*4;
  unsigned a=(unsigned)(h-yy)*15/(unsigned)h;
  unsigned r=8+p[2]*a/100,g=13+p[1]*a/100,b=23+p[0]*a/100;
  gg_platform_put_pixel(dx,dy,0xff000000u|(r<<16)|(g<<8)|b);
 }
 return 1;
}
