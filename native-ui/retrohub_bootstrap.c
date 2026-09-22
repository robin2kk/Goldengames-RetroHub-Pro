/* SPDX-License-Identifier: GPL-3.0-or-later */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ps5/kernel.h>
#include "gpl/hbldr.h"
#define UI_ELF "/data/goldengames-retrohub/goldengames_retrohub_ui.elf"
#define REQUEST "/data/goldengames_retrohub_request.txt"
static int elevate(void){static const unsigned char caps[16]={ [0 ... 15]=0xff };if(kernel_set_ucred_authid(-1,0x4801000000000013L))return -1;if(kernel_set_ucred_uid(-1,0))return -1;if(kernel_set_ucred_caps(-1,caps))return -1;return 0;}
static int alive(pid_t p){return p>0&&(kill(p,0)==0||errno!=ESRCH);}
static int read_line(FILE*f,char*out,size_t n){if(!fgets(out,n,f))return -1;out[strcspn(out,"\r\n")]=0;return 0;}
static int launch_request(void){
 FILE*f=fopen(REQUEST,"r");char target[1024],cwd[1024],a[6][1024];char *argv[8]={0};char *envp[]={0};int i,n=0,pid;
 if(!f)return -1;if(read_line(f,target,sizeof(target))||read_line(f,cwd,sizeof(cwd))){fclose(f);return -1;}
 while(n<6&&read_line(f,a[n],sizeof(a[n]))==0){argv[n+1]=a[n];n++;}fclose(f);remove(REQUEST);
 argv[0]=target;pid=hbldr_launch(cwd,target,-1,argv,envp);return pid<0?-1:0;
}
int main(void){char *argv[]={UI_ELF,0};char *envp[]={0};pid_t ui;
 if(elevate())return EXIT_FAILURE;remove(REQUEST);
 ui=hbldr_launch("/data/goldengames-retrohub",UI_ELF,-1,argv,envp);if(ui<0)return EXIT_FAILURE;
 while(alive(ui)){if(access(REQUEST,F_OK)==0){usleep(150000);launch_request();break;}usleep(100000);}
 return EXIT_SUCCESS;
}
