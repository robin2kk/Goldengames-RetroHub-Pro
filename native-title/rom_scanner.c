#include "rom_scanner.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <dirent.h>

static int eq(const char*a,const char*b){while(*a&&*b){if(tolower((unsigned char)*a++)!=tolower((unsigned char)*b++))return 0;}return!*a&&!*b;}
static int allowed(const char*id,const char*ext){
 if(eq(id,"nes"))return eq(ext,"nes")||eq(ext,"zip");
 if(eq(id,"snes"))return eq(ext,"sfc")||eq(ext,"smc")||eq(ext,"zip");
 if(eq(id,"n64"))return eq(ext,"z64")||eq(ext,"n64")||eq(ext,"v64")||eq(ext,"7z");
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


static int already_listed(const GGGameList *out, const char *path){
 for(int i=0;i<out->count;i++)if(strcmp(out->games[i].filename,path)==0)return 1;
 return 0;
}

static int scan_manifest(const char *id,GGGameList*out){
 char manifest[256];snprintf(manifest,sizeof(manifest),"/app0/library/%s.lst",id);
 FILE *fp=fopen(manifest,"r");if(!fp)return out->count;
 char line[GG_NAME_MAX*2];
 while(out->count<GG_MAX_GAMES&&fgets(line,sizeof(line),fp)){
  size_t n=strlen(line);
  if(n&&line[n-1]!='\n'&&!feof(fp)){int ch;while((ch=fgetc(fp))!='\n'&&ch!=EOF){}continue;}
  while(n&&(line[n-1]=='\n'||line[n-1]=='\r'))line[--n]=0;
  if(!n||line[0]=='#')continue;
  /* A manifest may contain a path alone or path, title and core separated by tabs. */
  char *title=strchr(line,'\t');
  if(title){*title++=0;char *core=strchr(title,'\t');if(core)*core=0;}
  const char *path=line;
  const char *base=strrchr(path,'/');base=base?base+1:path;
  const char *p=strrchr(base,'.');if(!p||!allowed(id,p+1))continue;
  if(strncmp(path,"/data/homebrew/RetroArch/",25)!=0&&
     strncmp(path,"/mnt/usb",8)!=0&&strncmp(path,"/mnt/ext",8)!=0)continue;
  if(strlen(path)>=GG_NAME_MAX||already_listed(out,path))continue;
  GGGame *g=&out->games[out->count];
  snprintf(g->filename,GG_NAME_MAX,"%s",path);
  if(title&&*title)snprintf(g->title,GG_NAME_MAX,"%s",title);
  else title_from(base,g->title);
  out->count++;
 }
 fclose(fp);return out->count;
}
int gg_scan_games(const char*id,GGGameList*out){
 if(!id||!out)return 0;
 out->count=0;
 char path[256];
 /* Read the installed payload RetroArch library directly where accessible. */
 if(snprintf(path,sizeof(path),"/data/homebrew/RetroArch/roms/%s",id)>=(int)sizeof(path))return 0;
 scan_dir(path,id,out);
 /* Include manifest entries even when direct scanning found some games. */
 scan_manifest(id,out);
 return out->count;
}
