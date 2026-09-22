#include "retrohub_runtime.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

static const char *ext_of(const char *name){const char *p=strrchr(name,'.');return p&&p[1]?p+1:"";}
static int allowed(const char *ext,const char *list){
 char buf[256],*tok,*save=0;
 if(!ext||!list)return 0;
 snprintf(buf,sizeof(buf),"%s",list);
 for(tok=strtok_r(buf,"|",&save);tok;tok=strtok_r(0,"|",&save)) if(!strcasecmp(ext,tok)) return 1;
 return 0;
}
static void title_from_filename(char *out,size_t n,const char *name){
 snprintf(out,n,"%s",name); char *dot=strrchr(out,'.'); if(dot)*dot=0;
}
int retrohub_scan_games(const char *system_id,const char *extensions,RetroGame *out,int max_games){
 char dirpath[RETROHUB_PATH_MAX],full[RETROHUB_PATH_MAX]; DIR *d; struct dirent *e; struct stat st; int count=0;
 if(!system_id||!extensions||!out||max_games<=0)return 0;
 snprintf(dirpath,sizeof(dirpath),"%s/%s",RETROARCH_ROMS,system_id);
 d=opendir(dirpath); if(!d)return 0;
 while((e=readdir(d))&&count<max_games){
  if(e->d_name[0]=='.'||!allowed(ext_of(e->d_name),extensions))continue;
  snprintf(full,sizeof(full),"%s/%s",dirpath,e->d_name);
  if(stat(full,&st)!=0||!S_ISREG(st.st_mode))continue;
  snprintf(out[count].path,sizeof(out[count].path),"%s",full);
  title_from_filename(out[count].title,sizeof(out[count].title),e->d_name); count++;
 }
 closedir(d); return count;
}
int retrohub_write_launch_request(const char *core,const char *content){
 char corepath[RETROHUB_PATH_MAX]; FILE *f;
 if(!core||!content)return -1;
 snprintf(corepath,sizeof(corepath),"%s/%s",RETROARCH_CORES,core);
 /* Request format consumed by the Goldengames bootstrap:
    line 1 target ELF, line 2 cwd, following lines argv. */
 f=fopen(RETROHUB_REQUEST,"w"); if(!f)return -1;
 fprintf(f,"%s\n%s\n-f\n-c\n%s\n-L\n%s\n%s\n",RETROARCH_ELF,RETROARCH_ROOT,RETROARCH_CFG,corepath,content);
 fclose(f); return 0;
}
