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
