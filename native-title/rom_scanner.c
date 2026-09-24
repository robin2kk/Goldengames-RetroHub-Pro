#include "rom_scanner.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#ifndef GG_RETROARCH_ROMS
#define GG_RETROARCH_ROMS "/data/homebrew/RetroArch/roms"
#endif

static int eq(const char*a,const char*b){while(*a&&*b){if(tolower((unsigned char)*a++)!=tolower((unsigned char)*b++))return 0;}return!*a&&!*b;}
static int folder_match(const char *id,const char *folder){
 char normalized[96];unsigned n=0;
 for(const unsigned char*p=(const unsigned char*)folder;*p;p++){
  if(isalnum(*p)){if(n+1>=sizeof(normalized))return 0;normalized[n++]=(char)tolower(*p);}
 }
 normalized[n]=0;
 if(eq(id,normalized))return 1;
 static const struct {const char*id,*folder;} aliases[]={
  {"psx","ps1"},{"psx","playstation"},{"psx","sonyplaystation"},
  {"nes","nintendoentertainmentsystem"},{"snes","supernintendo"},
  {"n64","nintendo64"},{"gb","gameboy"},{"gbc","gameboycolor"},
  {"gba","gameboyadvance"},{"genesis","megadrive"},{"genesis","segagenesis"},
  {"segacd","megacd"},{"x32","sega32x"},{"saturn","segasaturn"},
  {"pce","pcengine"},{"pce","turbografx16"},{"arcade","fbneo"},
  {"c64","commodore64"},{"atari2600","atari2600"},{"atari7800","atari7800"}
 };
 for(unsigned i=0;i<sizeof(aliases)/sizeof(aliases[0]);i++)
  if(eq(id,aliases[i].id)&&eq(normalized,aliases[i].folder))return 1;
 return 0;
}
static int allowed(const char*id,const char*ext){
 if(eq(id,"nes"))return eq(ext,"nes")||eq(ext,"zip");
 if(eq(id,"snes"))return eq(ext,"sfc")||eq(ext,"smc")||eq(ext,"zip");
 if(eq(id,"n64"))return eq(ext,"z64")||eq(ext,"n64")||eq(ext,"v64");
 if(eq(id,"genesis"))return eq(ext,"md")||eq(ext,"gen")||eq(ext,"bin")||eq(ext,"zip");
 if(eq(id,"psx"))return eq(ext,"cue")||eq(ext,"chd")||eq(ext,"pbp");
 if(eq(id,"gb"))return eq(ext,"gb")||eq(ext,"zip");
 if(eq(id,"gbc"))return eq(ext,"gbc")||eq(ext,"zip");
 if(eq(id,"gba"))return eq(ext,"gba")||eq(ext,"zip");
 if(eq(id,"segacd")||eq(id,"saturn"))return eq(ext,"cue")||eq(ext,"chd");
 if(eq(id,"x32"))return eq(ext,"32x")||eq(ext,"bin");
 if(eq(id,"atari2600"))return eq(ext,"a26")||eq(ext,"bin")||eq(ext,"zip");
 if(eq(id,"atari7800"))return eq(ext,"a78")||eq(ext,"bin")||eq(ext,"zip");
 if(eq(id,"lynx"))return eq(ext,"lnx")||eq(ext,"zip");
 if(eq(id,"jaguar"))return eq(ext,"j64")||eq(ext,"jag")||eq(ext,"zip");
 if(eq(id,"pce"))return eq(ext,"pce")||eq(ext,"zip");
 if(eq(id,"arcade"))return eq(ext,"zip");
 if(eq(id,"amiga"))return eq(ext,"adf")||eq(ext,"hdf")||eq(ext,"lha");
 if(eq(id,"c64"))return eq(ext,"d64")||eq(ext,"t64")||eq(ext,"crt");
 return 0;
}
static void title_from(const char*name,char*out){snprintf(out,GG_NAME_MAX,"%s",name);char*p=strrchr(out,'.');if(p)*p=0;}

static int scan_dir(const char *path,const char *id,GGGameList*out){
 DIR *dir=opendir(path);if(!dir)return 0;
 out->folder_found=1;
 struct dirent *entry;
 while(out->count<GG_MAX_GAMES&&(entry=readdir(dir))){
  const char *name=entry->d_name;
  if(name[0]=='.')continue;
  const char *p=strrchr(name,'.');if(!p||!allowed(id,p+1))continue;
  GGGame*g=&out->games[out->count];
  if(snprintf(g->filename,GG_NAME_MAX,"%s/%s",path,name)>=GG_NAME_MAX)continue;
  title_from(name,g->title);out->count++;
 }
 closedir(dir);return out->count;
}


static int scan_manifest(const char *id,GGGameList*out){
 char manifest[256];snprintf(manifest,sizeof(manifest),"/app0/library/%s.lst",id);
 FILE *fp=fopen(manifest,"r");if(!fp)return 0;
 out->manifest_found=1;
 char line[GG_NAME_MAX];
 while(out->count<GG_MAX_GAMES&&fgets(line,sizeof(line),fp)){
  size_t n=strlen(line);
  if(n&&line[n-1]!='\n'&&!feof(fp)){int ch;while((ch=fgetc(fp))!='\n'&&ch!=EOF){}continue;}
  while(n&&(line[n-1]=='\n'||line[n-1]=='\r'))line[--n]=0;
  if(!n||line[0]=='#')continue;
  const char *base=strrchr(line,'/');base=base?base+1:line;
  const char *p=strrchr(base,'.');if(!p||!allowed(id,p+1))continue;
  /* Manifests store the real path used by the existing payload RetroArch. */
  if(strncmp(line,"/data/homebrew/RetroArch/",25)!=0&&
     strncmp(line,"/mnt/usb",8)!=0&&strncmp(line,"/mnt/ext",8)!=0)continue;
  if(strlen(line)>=GG_NAME_MAX-1)continue;
  GGGame *g=&out->games[out->count];snprintf(g->filename,GG_NAME_MAX,"%s",line);title_from(base,g->title);out->count++;
 }
 fclose(fp);return out->count;
}
int gg_scan_games(const char*id,GGGameList*out){
 if(!id||!out)return 0;
 out->count=0;
 out->folder_found=0;out->manifest_found=0;
 char path[512];const char *root=GG_RETROARCH_ROMS;
 DIR *dirs=opendir(root);
 if(dirs){
  struct dirent *entry;
  while(out->count<GG_MAX_GAMES&&(entry=readdir(dirs))){
   if(!folder_match(id,entry->d_name))continue;
   if(snprintf(path,sizeof(path),"%s/%s",root,entry->d_name)>=(int)sizeof(path))continue;
   scan_dir(path,id,out);
  }
  closedir(dirs);
 }
 /* A manifest supplies paths when the native title cannot enumerate that folder. */
 scan_manifest(id,out);
 return out->count;
}
