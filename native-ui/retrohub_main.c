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
 SDL_Window*w=0;SDL_Renderer*r=0;
 remove(UI_LOG);ui_notify("RetroHub TEST 1 main");SDL_Delay(1500);
 putenv("SDL_VIDEODRIVER=ps5");SDL_SetHint("SDL_VIDEODRIVER","ps5");
 if(SDL_Init(SDL_INIT_VIDEO)<0){ui_notify("RetroHub TEST FAIL SDL");return 1;}
 ui_notify("RetroHub TEST 2 SDL");SDL_Delay(1500);
 w=SDL_CreateWindow("Goldengames RetroHub Pro",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1920,1080,SDL_WINDOW_FULLSCREEN);
 if(!w){ui_notify("RetroHub TEST FAIL WINDOW");SDL_Quit();return 2;}
 sceSystemServiceHideSplashScreen();ui_notify("RetroHub TEST 3 WINDOW");SDL_Delay(1500);
 r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE|SDL_RENDERER_TARGETTEXTURE);
 if(!r)r=SDL_CreateRenderer(w,-1,SDL_RENDERER_SOFTWARE);
 if(!r){ui_notify("RetroHub TEST FAIL RENDERER");SDL_DestroyWindow(w);SDL_Quit();return 3;}
 ui_notify("RetroHub TEST 4 RENDERER");SDL_Delay(1500);
 SDL_SetRenderDrawColor(r,255,0,255,255);SDL_RenderClear(r);SDL_RenderPresent(r);
 ui_notify("RetroHub TEST 5 MAGENTA PRESENTED");SDL_Delay(10000);
 SDL_SetRenderDrawColor(r,0,255,0,255);SDL_RenderClear(r);SDL_RenderPresent(r);
 ui_notify("RetroHub TEST 6 GREEN PRESENTED");SDL_Delay(10000);
 ui_notify("RetroHub TEST 7 END");SDL_Delay(2000);
 SDL_DestroyRenderer(r);SDL_DestroyWindow(w);SDL_Quit();return 0;
}
