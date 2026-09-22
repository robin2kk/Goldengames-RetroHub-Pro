#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "retrohub_ui.h"
int sceNotificationSend(int userId, _Bool isLogged, const char *payload);
int sceSystemServiceHideSplashScreen(void);
static void ui_notify(const char *s){char p[1024];snprintf(p,sizeof(p),"{\"rawData\":{\"viewTemplateType\":\"InteractiveToastTemplateB\",\"channelType\":\"Downloads\",\"useCaseId\":\"IDC\",\"toastOverwriteType\":\"No\",\"isImmediate\":true,\"priority\":100,\"viewData\":{\"message\":{\"body\":\"%s\"}},\"platformViews\":{\"previewDisabled\":{\"viewData\":{\"message\":{\"body\":\"%s\"}}}}},\"localNotificationId\":\"786420029\"}",s,s);sceNotificationSend(0xFE,1,p);}

#define UI_LOG "/data/goldengames_retrohub_ui.log"

static void ui_log(const char *msg){
 FILE *fp=fopen(UI_LOG,"a");
 if(!fp)return;
 fprintf(fp,"[ui] %s\n",msg?msg:"(null)");
 fclose(fp);
}
static void ui_log_sdl(const char *stage){
 char b[1024];
 snprintf(b,sizeof(b),"%s: %s",stage,SDL_GetError());
 ui_log(b);
}

int main(void){
 SDL_Window*w=0;SDL_Renderer*r=0;SDL_Event e;RetroHubState s;int run=1;
 remove(UI_LOG);
 putenv("SDL_VIDEODRIVER=ps5");SDL_SetHint("SDL_VIDEODRIVER","ps5");
 if(SDL_Init(SDL_INIT_VIDEO)<0){ui_notify("RetroHub FAIL SDL");return 1;}
 w=SDL_CreateWindow("Goldengames RetroHub Pro",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1920,1080,SDL_WINDOW_FULLSCREEN);
 if(!w){ui_notify("RetroHub FAIL WINDOW");SDL_Quit();return 2;}
 sceSystemServiceHideSplashScreen();
 r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE|SDL_RENDERER_TARGETTEXTURE);
 if(!r)r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE|SDL_RENDERER_PRESENTVSYNC);
 if(!r)r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE);
 if(!r){ui_notify("RetroHub FAIL RENDERER");SDL_DestroyWindow(w);SDL_Quit();return 3;}
 SDL_SetRenderDrawBlendMode(r,SDL_BLENDMODE_BLEND);
 memset(&s,0,sizeof(s));s.screen=RH_SYSTEMS;
 ui_notify("RetroHub STATIC UI TEST");
 while(run){
  retrohub_render(r,&s);
  SDL_RenderPresent(r);
  SDL_Delay(16);
 }
 return 0;
}
