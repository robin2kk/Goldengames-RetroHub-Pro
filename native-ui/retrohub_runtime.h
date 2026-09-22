#pragma once
#define RETROARCH_ROOT "/data/homebrew/RetroArch"
#define RETROARCH_ELF RETROARCH_ROOT "/retroarch.elf"
#define RETROARCH_CFG RETROARCH_ROOT "/retroarch.cfg"
#define RETROARCH_CORES RETROARCH_ROOT "/cores"
#define RETROARCH_ROMS RETROARCH_ROOT "/roms"
#define RETROHUB_REQUEST "/data/goldengames_retrohub_request.txt"
#define RETROHUB_MAX_GAMES 2048
#define RETROHUB_PATH_MAX 1024
typedef struct RetroGame { char title[256]; char path[RETROHUB_PATH_MAX]; } RetroGame;
int retrohub_scan_games(const char *system_id,const char *extensions,RetroGame *out,int max_games);
int retrohub_write_launch_request(const char *core,const char *content);
