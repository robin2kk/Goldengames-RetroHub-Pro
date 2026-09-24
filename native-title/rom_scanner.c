#include "rom_scanner.h"
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

extern int getdents(int fd,char *buffer,int bytes);

static int eq(const char*a,const char*b){while(*a&&*b){if(tolower((unsigned char)*a++)!=tolower((unsigned char)*b++))return 0;}return!*a&&!*b;}
static int allowed(const char*id,const char*ext){
 if(eq(id,"nes"))return eq(ext,"nes")||eq(ext,"zip");
 if(eq(id,"snes"))return eq(ext,"sfc")||eq(ext,"smc")||eq(ext,"zip");
 if(eq(id,"n64"))return eq(ext,"z64")||eq(ext,"n64")||eq(ext,"v64");
 if(eq(id,"genesis"))return eq(ext,"md")||eq(ext,"gen")||eq(ext,"bin")||eq(ext,"zip");
 if(eq(id,"psx"))return eq(ext,"cue")||eq(ext,"chd")||eq(ext,"pbp");
 return 0;
}
static void title_from(const char*name,char*out){snprintf(out,GG_NAME_MAX,"%s",name);char*p=strrchr(out,'.');if(p)*p=0;}

static int scan_dir(const char *path,const char *id,GGGameList*out){
 int fd=open(path,O_RDONLY|O_DIRECTORY);if(fd<0)return 0;
 char *buf=(char*)malloc(64*1024);if(!buf){close(fd);return 0;}
 int done=0;
 while(!done&&out->count<GG_MAX_GAMES){
  int n=getdents(fd,buf,64*1024);if(n<=0)break;
  size_t off=0;
  while(off<(size_t)n&&out->count<GG_MAX_GAMES){
   size_t rem=(size_t)n-off;char *rec=buf+off;uint32_t ino=0;uint16_t reclen=0;
   if(rem<8){done=1;break;}memcpy(&ino,rec,4);memcpy(&reclen,rec+4,2);
   size_t namelen=(unsigned char)rec[7];
   if(reclen<8+namelen+1||reclen>rem||namelen>=GG_NAME_MAX||rec[8+namelen]!=0){done=1;break;}
   off+=reclen;if(!ino)continue;const char *name=rec+8;if(name[0]=='.')continue;
   const char *p=strrchr(name,'.');if(!p||!allowed(id,p+1))continue;
   GGGame*g=&out->games[out->count++];snprintf(g->filename,GG_NAME_MAX,"%s",name);title_from(name,g->title);
  }
 }
 free(buf);close(fd);return out->count;
}

int gg_scan_games(const char*id,GGGameList*out){
 if(!out)return 0;out->count=0;char path[256];
 /* A staged native title sees its own FTP folder as /app0. */
 const char *roots[]={"/app0/content","/app0/roms"};
 for(unsigned i=0;i<sizeof(roots)/sizeof(roots[0]);i++){
  snprintf(path,sizeof(path),"%s/%s",roots[i],id);
  if(scan_dir(path,id,out)>0)break;
 }
 return out->count;
}
