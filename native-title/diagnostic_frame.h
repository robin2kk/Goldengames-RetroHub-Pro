#ifndef GOLDENGAMES_NATIVE_TITLE_DIAGNOSTIC_H
#define GOLDENGAMES_NATIVE_TITLE_DIAGNOSTIC_H
#include <stdint.h>
typedef struct { unsigned width; unsigned height; } GGSurface;
void gg_platform_put_pixel(unsigned x,unsigned y,uint32_t color);
void gg_draw_acceptance_frame(GGSurface s);
#endif
