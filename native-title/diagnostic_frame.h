#ifndef GOLDENGAMES_NATIVE_TITLE_DIAGNOSTIC_H
#define GOLDENGAMES_NATIVE_TITLE_DIAGNOSTIC_H
#include <stdint.h>
typedef struct { uint32_t *pixels; unsigned width; unsigned height; unsigned pitch_pixels; } GGSurface;
void gg_draw_acceptance_frame(GGSurface s);
#endif
