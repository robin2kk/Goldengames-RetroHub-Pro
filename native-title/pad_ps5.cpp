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
namespace { int handle=-1; uint32_t previous=0; bool owns_user=false;
struct PadSample { uint8_t bytes[120]; };
uint32_t load_buttons(const PadSample&s){uint32_t v=0;memcpy(&v,s.bytes,4);return v;}
}
extern "C" int gg_pad_open(void){
 owns_user=sceUserServiceInitialize(nullptr)==0;
 int user=-1;if(sceUserServiceGetInitialUser(&user)<0)return 0;
 if(scePadInit()<0)return 0;handle=scePadOpen(user,0,0,nullptr);return handle>=0;
}
extern "C" uint32_t gg_pad_pressed(void){
 if(handle<0)return 0;PadSample s{};if(scePadRead(handle,&s,1)<=0)return 0;
 uint32_t b=load_buttons(s),fresh=b&~previous,out=0;previous=b;
 if(fresh&0x000080)out|=GG_PAD_LEFT;if(fresh&0x000020)out|=GG_PAD_RIGHT;
 if(fresh&0x004000)out|=GG_PAD_CROSS;if(fresh&0x002000)out|=GG_PAD_CIRCLE;
 if(fresh&0x000400)out|=GG_PAD_L1;if(fresh&0x000800)out|=GG_PAD_R1;return out;
}
extern "C" void gg_pad_close(void){if(handle>=0)scePadClose(handle);handle=-1;if(owns_user)sceUserServiceTerminate();owns_user=false;}
