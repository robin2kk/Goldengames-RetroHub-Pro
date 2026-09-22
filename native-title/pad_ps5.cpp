#include "pad_ps5.h"
#include <stdint.h>
#include <string.h>
extern "C" {
int sceUserServiceInitialize(const void*);
int sceUserServiceGetInitialUser(int*);
int sceUserServiceTerminate(void);
int scePadInit(void);
int scePadOpen(int,int,int,const void*);
int scePadRead(int,void*,int);
int scePadClose(int);
}
namespace { int handle=-1; uint32_t previous=0;
struct PadSample { uint8_t bytes[120]; };
uint32_t load_buttons(const PadSample&s){uint32_t v=0;memcpy(&v,s.bytes,4);return v;}
}
extern "C" int gg_pad_open(void){
 sceUserServiceInitialize(nullptr); int user=-1;if(sceUserServiceGetInitialUser(&user)<0)return 0;
 scePadInit();handle=scePadOpen(user,0,0,nullptr);return handle>=0;
}
extern "C" uint32_t gg_pad_pressed(void){
 if(handle<0)return 0;PadSample s{};if(scePadRead(handle,&s,1)<=0)return 0;
 uint32_t b=load_buttons(s), out=0;
 const uint32_t L=0x80,R=0x20,X=0x400000,C=0x200000;
 uint32_t fresh=b&~previous;previous=b;
 if(fresh&L)out|=GG_PAD_LEFT;if(fresh&R)out|=GG_PAD_RIGHT;if(fresh&X)out|=GG_PAD_CROSS;if(fresh&C)out|=GG_PAD_CIRCLE;return out;
}
extern "C" void gg_pad_close(void){if(handle>=0)scePadClose(handle);handle=-1;sceUserServiceTerminate();}
