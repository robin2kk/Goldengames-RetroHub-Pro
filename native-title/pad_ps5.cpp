#include "pad_ps5.h"
#include <stdint.h>
#include <string.h>

extern "C" {
int sceKernelLoadStartModule(const char*, unsigned long, const void*, unsigned int, const void*, int*);
int sceKernelDlsym(int, const char*, void**);
}

namespace {
typedef int (*FnUserInit)(const void*);
typedef int (*FnGetUser)(int*);
typedef int (*FnPadInit)(void);
typedef int (*FnPadOpen)(int,int,int,const void*);
typedef int (*FnPadRead)(int,void*,int);
typedef int (*FnPadClose)(int);

FnUserInit user_init=nullptr; FnGetUser get_user=nullptr;
FnPadInit pad_init=nullptr; FnPadOpen pad_open=nullptr; FnPadRead pad_read=nullptr; FnPadClose pad_close=nullptr;
int handle=-1; uint32_t previous=0;
struct PadSample { uint8_t bytes[120]; };

int load_module(const char *name){
 const char *roots[]={"/system/common/lib/","/system/priv/lib/"};
 char path[256];
 for(unsigned i=0;i<2;i++){
  unsigned n=0; while(roots[i][n]&&n+1<sizeof(path)){path[n]=roots[i][n];n++;}
  unsigned j=0; while(name[j]&&n+1<sizeof(path)){path[n++]=name[j++];}
  path[n]=0; int res=0; int h=sceKernelLoadStartModule(path,0,nullptr,0,nullptr,&res); if(h>=0)return h;
 }
 return -1;
}
template<class T> bool sym(int m,const char*n,T&out){void*p=nullptr;if(m<0||sceKernelDlsym(m,n,&p)<0||!p)return false;out=(T)p;return true;}
uint32_t buttons(const PadSample&s){uint32_t v=0;memcpy(&v,s.bytes,4);return v;}
}

extern "C" int gg_pad_open(void){
 int u=load_module("libSceUserService.sprx");
 int p=load_module("libScePad.sprx");
 if(!sym(u,"sceUserServiceInitialize",user_init)||!sym(u,"sceUserServiceGetInitialUser",get_user))return 0;
 if(!sym(p,"scePadInit",pad_init)||!sym(p,"scePadOpen",pad_open)||!sym(p,"scePadRead",pad_read)||!sym(p,"scePadClose",pad_close))return 0;
 user_init(nullptr);int user=-1;if(get_user(&user)<0)return 0;
 if(pad_init()<0)return 0;handle=pad_open(user,0,0,nullptr);return handle>=0;
}
extern "C" uint32_t gg_pad_pressed(void){
 if(handle<0||!pad_read)return 0;PadSample s{};if(pad_read(handle,&s,1)<=0)return 0;
 uint32_t b=buttons(s),fresh=b&~previous;previous=b,out=0;
 if(fresh&0x000080)out|=GG_PAD_LEFT;if(fresh&0x000020)out|=GG_PAD_RIGHT;
 if(fresh&0x400000)out|=GG_PAD_CROSS;if(fresh&0x200000)out|=GG_PAD_CIRCLE;return out;
}
extern "C" void gg_pad_close(void){if(handle>=0&&pad_close)pad_close(handle);handle=-1;}
