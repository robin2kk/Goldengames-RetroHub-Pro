#include "websrv_launch.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

extern "C" int ps5_socket_close(int);

static int enc(char*out,unsigned cap,const char*in){
 static const char h[]="0123456789ABCDEF";unsigned o=0;
 while(*in){unsigned char c=(unsigned char)*in++;
  unsigned needed=((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.'||c=='~')?1:3;
  if(o+needed>=cap)return -1;
  if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-'||c=='_'||c=='.'||c=='~')out[o++]=(char)c;
  else{out[o++]='%';out[o++]=h[c>>4];out[o++]=h[c&15];}}
 out[o]=0;return 0;
}
static int valid_core(const char*s){
 if(!s||!*s)return 0;
 for(const unsigned char*p=(const unsigned char*)s;*p;p++)
  if(!((*p>='a'&&*p<='z')||(*p>='0'&&*p<='9')||*p=='_'||*p=='.'||*p=='-'))return 0;
 return 1;
}
static int escaped_arg(char*out,unsigned cap,const char*in){
 unsigned n=0;
 for(;*in;in++){
  if(*in=='\n'||*in=='\r'||(unsigned char)*in<32)return -1;
  if(*in==' '||*in=='\\'){if(n+1>=cap)return -1;out[n++]='\\';}
  if(n+1>=cap)return -1;out[n++]=*in;
 }
 out[n]=0;return 0;
}
static int allsend(int fd,const char*p,unsigned long n){
 while(n){int w=(int)send(fd,p,n,0);if(w<=0)return -1;p+=w;n-=(unsigned long)w;}return 0;
}
extern "C" int gg_launch_retroarch_payload(const char*core_name,const char*content_path){
 if(!valid_core(core_name)||!content_path||!*content_path||
    (strncmp(content_path,"/data/homebrew/RetroArch/",25)!=0&&
     strncmp(content_path,"/mnt/usb",8)!=0&&strncmp(content_path,"/mnt/ext",8)!=0))return -1;
 const char*root="/data/homebrew/RetroArch";
 char exe[256],core[512],game[1024],args[2048],env[768],pe[768],pa[6144],pv[1536],pc[768],req[9216];
 snprintf(exe,sizeof(exe),"%s/retroarch.elf",root);
 if(snprintf(core,sizeof(core),"%s/.config/retroarch/cores/%s",root,core_name)>=(int)sizeof(core)||
    escaped_arg(game,sizeof(game),content_path)<0)return -1;
 if(snprintf(args,sizeof(args),"-f -c %s/retroarch.cfg -L %s %s",root,core,game)>=(int)sizeof(args))return -1;
 snprintf(env,sizeof(env),"HOME=%s LD_LIBRARY_PATH=%s",root,root);
 if(enc(pe,sizeof(pe),exe)<0||enc(pa,sizeof(pa),args)<0||enc(pv,sizeof(pv),env)<0||enc(pc,sizeof(pc),root)<0)return -1;
 if(snprintf(req,sizeof(req),"GET /hbldr?pipe=0&daemon=0&path=%s&args=%s&env=%s&cwd=%s HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: close\r\n\r\n",pe,pa,pv,pc)>=(int)sizeof(req))return -1;
 int fd=socket(AF_INET,SOCK_STREAM,0);if(fd<0)return -2;
 timeval timeout={3,0};setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
 sockaddr_in a={};a.sin_family=AF_INET;a.sin_port=htons(8080);a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);
 if(connect(fd,(const sockaddr*)&a,sizeof(a))<0){ps5_socket_close(fd);return -3;}
 if(allsend(fd,req,strlen(req))<0){ps5_socket_close(fd);return -4;}
 char r[96]={0};int n=recv(fd,r,sizeof(r)-1,0);ps5_socket_close(fd);
 if(n<=0)return -5;return strstr(r," 200 ")?0:-6;
}
