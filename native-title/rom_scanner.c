#include "rom_scanner.h"
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
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
int gg_scan_games(const char*id,GGGameList*out){if(!out)return 0;out->count=0;char path[256];/* Native PS5 titles cannot browse the external /data/homebrew tree.
    Files staged over FTP under /data/homebrew/PPSA99202/ are visible in-title as /app0/. */
 const char *roots[]={"/app0/content","/app0/roms"};DIR*d=0;
 for(unsigned i=0;i<sizeof(roots)/sizeof(roots[0]);i++){snprintf(path,sizeof(path),"%s/%s",roots[i],id);d=opendir(path);if(d)break;}
 if(!d)return 0;struct dirent*e;while((e=readdir(d))&&out->count<GG_MAX_GAMES){if(e->d_name[0]=='.')continue;const char*p=strrchr(e->d_name,'.');if(!p||!allowed(id,p+1))continue;GGGame*g=&out->games[out->count++];snprintf(g->filename,GG_NAME_MAX,"%s",e->d_name);title_from(e->d_name,g->title);}closedir(d);return out->count;}
