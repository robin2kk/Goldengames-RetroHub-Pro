#ifndef GOLDENGAMES_NATIVE_TITLE_DIAGNOSTIC_H
#define GOLDENGAMES_NATIVE_TITLE_DIAGNOSTIC_H
#include <stdint.h>
typedef struct { unsigned width; unsigned height; } GGSurface;
#ifdef __cplusplus
extern "C" {
#endif
void gg_platform_put_pixel(unsigned,unsigned,uint32_t);
void gg_draw_retrohub_frame(GGSurface s,int selected);
#ifdef __cplusplus
}
#endif
#endif
