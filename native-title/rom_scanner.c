#include "rom_scanner.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#ifndef GG_RETROARCH_ROMS
#define GG_RETROARCH_ROMS "/data/homebrew/RetroArch/roms"
#endif
#ifndef GG_LIBRARY_ROOT
#define GG_LIBRARY_ROOT "/app0/library"
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
  {"psx","ps1"},{"psx","playstation"},{"psx","playstation1"},
  {"psx","sonyplaystation"},{"psx","sonyplaystation1"},
  {"nes","nintendoentertainmentsystem"},{"snes","supernintendo"},
  {"n64","nintendo64"},{"n64","nintendonintendo64"},
  {"n64","nintendo64roms"},{"gb","gameboy"},{"gbc","gameboycolor"},
  {"gba","gameboyadvance"},{"genesis","megadrive"},{"genesis","segagenesis"},
  {"segacd","megacd"},{"x32","sega32x"},{"saturn","segasaturn"},
  {"saturn","segasaturnroms"},
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
 if(eq(id,"n64"))return eq(ext,"z64")||eq(ext,"n64")||eq(ext,"v64")||
  eq(ext,"bin")||eq(ext,"u1")||eq(ext,"ndd")||eq(ext,"zip");
 if(eq(id,"genesis"))return eq(ext,"md")||eq(ext,"gen")||eq(ext,"bin")||eq(ext,"zip");
 if(eq(id,"psx"))return eq(ext,"cue")||eq(ext,"chd")||eq(ext,"pbp")||eq(ext,"m3u")||
  eq(ext,"iso")||eq(ext,"img")||eq(ext,"ccd")||eq(ext,"mdf")||eq(ext,"bin")||eq(ext,"toc")||eq(ext,"cbn");
 if(eq(id,"gb"))return eq(ext,"gb")||eq(ext,"zip");
 if(eq(id,"gbc"))return eq(ext,"gbc")||eq(ext,"zip");
 if(eq(id,"gba"))return eq(ext,"gba")||eq(ext,"zip");
 if(eq(id,"saturn"))return eq(ext,"cue")||eq(ext,"chd")||eq(ext,"m3u")||
  eq(ext,"iso")||eq(ext,"ccd")||eq(ext,"mds")||eq(ext,"bin")||eq(ext,"zip");
 if(eq(id,"segacd"))return eq(ext,"cue")||eq(ext,"chd");
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

static int scan_dir(const char *path,const char *id,GGGameList*out,int depth){
 DIR *dir=opendir(path);if(!dir){out->folder_error=errno;return 0;}
 out->folder_found=1;
 out->folder_error=0;
 struct dirent *entry;
 int has_cue=0;
 if(eq(id,"psx")||eq(id,"saturn")){
  while((entry=readdir(dir))){const char*p=strrchr(entry->d_name,'.');if(p&&eq(p+1,"cue")){has_cue=1;break;}}
  rewinddir(dir);
 }
 while(out->count<GG_MAX_GAMES&&(entry=readdir(dir))){
  const char *name=entry->d_name;
  if(name[0]=='.')continue;
  char full[GG_NAME_MAX];
  if(snprintf(full,sizeof(full),"%s/%s",path,name)>=(int)sizeof(full))continue;
  struct stat st;if(stat(full,&st)<0)continue;
  if(S_ISDIR(st.st_mode)){
   if(depth>0)scan_dir(full,id,out,depth-1);
   continue;
  }
  if(!S_ISREG(st.st_mode))continue;
  const char *p=strrchr(name,'.');if(!p||!allowed(id,p+1))continue;
  if(has_cue&&eq(p+1,"bin"))continue; /* track data belongs to a cue sheet */
  GGGame*g=&out->games[out->count];
  snprintf(g->filename,GG_NAME_MAX,"%s",full);
  title_from(name,g->title);out->count++;
 }
 closedir(dir);return out->count;
}


static int scan_manifest(const char *id,GGGameList*out){
 char manifest[256];snprintf(manifest,sizeof(manifest),"%s/%s.lst",GG_LIBRARY_ROOT,id);
 FILE *fp=fopen(manifest,"r");if(!fp)return 0;
 out->manifest_found=1;
 char line[GG_NAME_MAX];
 while(out->count<GG_MAX_GAMES&&fgets(line,sizeof(line),fp)){
  size_t n=strlen(line);
  if(n&&line[n-1]!='\n'&&!feof(fp)){int ch;while((ch=fgetc(fp))!='\n'&&ch!=EOF){}continue;}
  while(n&&(line[n-1]=='\n'||line[n-1]=='\r'))line[--n]=0;
  if(!n||line[0]=='#')continue;
  /* An optional RetroArch playlist label follows the path. It supplies the
     precise name used by Named_Boxarts without changing the launch path. */
  char *label=strchr(line,'\t');if(label)*label++=0;
  const char *base=strrchr(line,'/');base=base?base+1:line;
  const char *p=strrchr(base,'.');if(!p||!allowed(id,p+1))continue;
  /* Manifests store the real path used by the existing payload RetroArch. */
  if(strncmp(line,"/data/homebrew/RetroArch/",25)!=0&&
     strncmp(line,"/mnt/usb",8)!=0&&strncmp(line,"/mnt/ext",8)!=0)continue;
  if(strlen(line)>=GG_NAME_MAX-1)continue;
  int duplicate=0;for(int i=0;i<out->count;i++)if(eq(out->games[i].filename,line)){duplicate=1;break;}
  if(duplicate)continue;
  GGGame *g=&out->games[out->count];snprintf(g->filename,GG_NAME_MAX,"%s",line);
  if(label&&*label)snprintf(g->title,GG_NAME_MAX,"%s",label);
  else title_from(base,g->title);
  out->count++;
 }
 fclose(fp);return out->count;
}
int gg_scan_games(const char*id,GGGameList*out){
 if(!id||!out)return 0;
 out->count=0;
 out->folder_found=0;out->manifest_found=0;out->folder_error=0;
 /* Curated FTP manifests are authoritative when the title cannot read the
    RetroArch folder. Do not duplicate entries by scanning again. */
 if(scan_manifest(id,out)>0)return out->count;
 char path[512];const char *root=GG_RETROARCH_ROMS;
 DIR *dirs=opendir(root);
 if(dirs){
  struct dirent *entry;
  while(out->count<GG_MAX_GAMES&&(entry=readdir(dirs))){
   if(!folder_match(id,entry->d_name))continue;
   if(snprintf(path,sizeof(path),"%s/%s",root,entry->d_name)>=(int)sizeof(path))continue;
   scan_dir(path,id,out,3);
  }
  closedir(dirs);
  /* Some FTP layouts expose the exact system directory even when it was not
     returned by the parent listing. Avoid scanning it twice. */
  if(!out->folder_found &&
     snprintf(path,sizeof(path),"%s/%s",root,id)<(int)sizeof(path))
   scan_dir(path,id,out,3);
 }else{
  out->folder_error=errno;
  /* Try the known direct path independently of parent directory enumeration. */
  if(snprintf(path,sizeof(path),"%s/%s",root,id)<(int)sizeof(path))
   scan_dir(path,id,out,3);
 }
 /* A manifest supplies paths when the native title cannot enumerate that folder. */
 return out->count;
}
