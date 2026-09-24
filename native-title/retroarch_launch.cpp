#include "retroarch_launch.h"
#include <stdint.h>
#include <stdio.h>

struct GGAppLaunchCtx {
 int structsize;
 int user_id;
 int app_opt;
 uint64_t crash_report;
 int check_flag;
};

extern "C" int sceUserServiceInitialize(void *params);
extern "C" int sceUserServiceGetForegroundUser(int *user_id);
extern "C" int sceSystemServiceLaunchApp(const char *title_id,char **argv,GGAppLaunchCtx *ctx);

extern "C" int gg_launch_retroarch(const char *title_id,const char *core_path,const char *content_path){
 if(!title_id||!title_id[0]||!core_path||!core_path[0]||!content_path||!content_path[0])return -1;

 static char arg0[]="retroarch";
 static char argL[]="-L";
 static char core[512];
 static char content[512];
 snprintf(core,sizeof(core),"%s",core_path);
 snprintf(content,sizeof(content),"%s",content_path);
 char *argv[]={arg0,argL,core,content,nullptr};

 GGAppLaunchCtx ctx={0};
 ctx.structsize=sizeof(ctx);
 sceUserServiceInitialize(nullptr);
 sceUserServiceGetForegroundUser(&ctx.user_id);
 return sceSystemServiceLaunchApp(title_id,argv,&ctx);
}
