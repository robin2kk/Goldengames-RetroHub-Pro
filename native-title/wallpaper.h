#ifndef GG_WALLPAPER_H
#define GG_WALLPAPER_H
#include "diagnostic_frame.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Returns 1 when an image was drawn; 0 uses the existing console theme. */
int gg_draw_wallpaper(GGSurface surface,int system);
/* Draws the selected game's PNG cover, returning 0 if no readable match exists. */
int gg_draw_boxart(GGSurface surface,int system,const char *title,int x,int y,int w,int h);
#ifdef __cplusplus
}
#endif
#endif
