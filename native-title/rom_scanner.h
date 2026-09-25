#ifndef GG_ROM_SCANNER_H
#define GG_ROM_SCANNER_H
#define GG_MAX_GAMES 256
#define GG_NAME_MAX 512
typedef struct { char filename[GG_NAME_MAX]; char title[GG_NAME_MAX]; } GGGame;
typedef struct { GGGame games[GG_MAX_GAMES]; int count; int folder_found; int manifest_found; int folder_error; } GGGameList;
#ifdef __cplusplus
extern "C" {
#endif
int gg_scan_games(const char *system_id, GGGameList *out);
#ifdef __cplusplus
}
#endif
#endif
