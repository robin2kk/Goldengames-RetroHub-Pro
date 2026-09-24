#ifndef GG_WALLPAPER_H
#define GG_WALLPAPER_H
#include "diagnostic_frame.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Returns 1 when an image was drawn; 0 uses the existing console theme. */
int gg_draw_wallpaper(GGSurface surface,int system);
#ifdef __cplusplus
}
#endif
#endif
