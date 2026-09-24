#include "websrv_launch.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

extern "C" int ps5_socket_close(int);

static void enc(char*out,unsigned cap,const char*in){
 static const char h[]="0123456789ABCDEF";unsigned o=0;
 while(*in&&o+4<cap){unsigned char c=(unsigned char)*in++;
  if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.'||c=='~')out[o++]=(char)c;
  else{out[o++]='%';out[o++]=h[c>>4];out[o++]=h[c&15];}}
 out[o]=0;
}
static int allsend(int fd,const char*p,unsigned long n){
 while(n){int w=(int)send(fd,p,n,0);if(w<=0)return -1;p+=w;n-=(unsigned long)w;}return 0;
}
extern "C" int gg_launch_retroarch_payload(const char*core_name,const char*content_path){
 if(!core_name||!*core_name||!content_path||!*content_path)return -1;
 const char*root="/data/homebrew/RetroArch";
 char exe[256],core[512],args[1536],env[768],pe[768],pa[3072],pv[1536],pc[768],req[7168];
 snprintf(exe,sizeof(exe),"%s/retroarch.elf",root);
 snprintf(core,sizeof(core),"%s/.config/retroarch/cores/%s",root,core_name);
 snprintf(args,sizeof(args),"-f -c %s/retroarch.cfg -L %s %s",root,core,content_path);
 snprintf(env,sizeof(env),"HOME=%s LD_LIBRARY_PATH=%s",root,root);
 enc(pe,sizeof(pe),exe);enc(pa,sizeof(pa),args);enc(pv,sizeof(pv),env);enc(pc,sizeof(pc),root);
 snprintf(req,sizeof(req),"GET /hbldr?pipe=0&daemon=0&path=%s&args=%s&env=%s&cwd=%s HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: close\r\n\r\n",pe,pa,pv,pc);
 int fd=socket(AF_INET,SOCK_STREAM,0);if(fd<0)return -2;
 sockaddr_in a={};a.sin_family=AF_INET;a.sin_port=htons(8080);a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
 if(connect(fd,(const sockaddr*)&a,sizeof(a))<0){ps5_socket_close(fd);return -3;}
 if(allsend(fd,req,strlen(req))<0){ps5_socket_close(fd);return -4;}
 char r[96]={0};int n=recv(fd,r,sizeof(r)-1,0);ps5_socket_close(fd);
 if(n<=0)return -5;return strstr(r," 200 ")?0:-6;
}
