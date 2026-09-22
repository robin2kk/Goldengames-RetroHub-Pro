/* SPDX-License-Identifier: GPL-3.0-or-later */
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <ps5/kernel.h>
#include "gpl/hbldr.h"

#define UI_ELF "/data/goldengames-retrohub/goldengames_retrohub_ui.elf"
#define REQUEST "/data/goldengames_retrohub_request.txt"
#define LOG "/data/goldengames_retrohub_bootstrap.log"

int sceUserServiceInitialize(void *);
void sceUserServiceTerminate(void);
int sceNotificationSend(int userId, _Bool isLogged, const char *payload);
static void stage(const char *s){char p[1024];snprintf(p,sizeof(p),"{\"rawData\":{\"viewTemplateType\":\"InteractiveToastTemplateB\",\"channelType\":\"Downloads\",\"useCaseId\":\"IDC\",\"toastOverwriteType\":\"No\",\"isImmediate\":true,\"priority\":100,\"viewData\":{\"message\":{\"body\":\"%s\"}},\"platformViews\":{\"previewDisabled\":{\"viewData\":{\"message\":{\"body\":\"%s\"}}}}},\"localNotificationId\":\"786420028\"}",s,s);sceNotificationSend(0xFE,1,p);}

static void logline(const char *s){FILE *f=fopen(LOG,"a");if(f){fprintf(f,"%s\n",s?s:"(null)");fclose(f);}}
static void logpath(const char *p,const char *v){char b[1400];snprintf(b,sizeof(b),"%s%s",p,v?v:"(null)");logline(b);}
static int elevate(void){static const uint8_t caps[16]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};if(kernel_set_ucred_authid(-1,0x4801000000000013L))return -1;if(kernel_set_ucred_uid(-1,0))return -1;if(kernel_set_ucred_caps(-1,caps))return -1;return 0;}
static int alive(pid_t p){return p>0&&(kill(p,0)==0||errno!=ESRCH);}
static int read_line(FILE*f,char*out,size_t n){if(!fgets(out,n,f))return -1;out[strcspn(out,"\r\n")]=0;return 0;}
static int launch_request(void){FILE*f=fopen(REQUEST,"r");char target[1024],cwd[1024],a[6][1024];char *argv[8]={0};char *envp[]={"HOME=/data/homebrew/RetroArch","LD_LIBRARY_PATH=/data/homebrew/RetroArch",0};int n=0,pid;if(!f){logline("request open failed");return -1;}if(read_line(f,target,sizeof(target))||read_line(f,cwd,sizeof(cwd))){fclose(f);logline("request parse failed");return -1;}while(n<6&&read_line(f,a[n],sizeof(a[n]))==0){argv[n+1]=a[n];n++;}fclose(f);remove(REQUEST);argv[0]=target;logpath("launch target: ",target);pid=hbldr_launch(cwd,target,-1,argv,envp);if(pid<0)logline("requested hbldr_launch failed");return pid<0?-1:0;}

int main(void){char *argv[]={UI_ELF,0};char *envp[]={0};pid_t ui;remove(LOG);stage("RetroHub 1 main reached");logline("Goldengames RetroHub bootstrap start");if(sceUserServiceInitialize(0)){logline("sceUserServiceInitialize failed");return EXIT_FAILURE;}stage("RetroHub 2 UserService OK");logline("UserService initialized");if(elevate()){logline("privilege elevation failed");sceUserServiceTerminate();return EXIT_FAILURE;}stage("RetroHub 3 privileges OK");logline("privilege elevation OK");if(access(UI_ELF,F_OK)!=0){logpath("UI ELF missing: ",UI_ELF);sceUserServiceTerminate();return EXIT_FAILURE;}stage("RetroHub 4 UI found");logpath("UI ELF found: ",UI_ELF);remove(REQUEST);stage("RetroHub 5 launching UI");logline("calling hbldr_launch for UI");ui=hbldr_launch("/data/goldengames-retrohub",UI_ELF,-1,argv,envp);if(ui<0){stage("RetroHub FAILED hbldr");logline("UI hbldr_launch failed");sceUserServiceTerminate();return EXIT_FAILURE;}stage("RetroHub 6 hbldr OK");logline("UI hbldr_launch OK");while(alive(ui)){if(access(REQUEST,F_OK)==0){logline("launch request detected");usleep(150000);launch_request();break;}usleep(100000);}logline("bootstrap exit");sceUserServiceTerminate();return EXIT_SUCCESS;}
