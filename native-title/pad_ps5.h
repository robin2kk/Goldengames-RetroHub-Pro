#ifndef GOLDENGAMES_NATIVE_PAD_H
#define GOLDENGAMES_NATIVE_PAD_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { GG_PAD_LEFT=1u<<0,GG_PAD_RIGHT=1u<<1,GG_PAD_CROSS=1u<<2,GG_PAD_CIRCLE=1u<<3,GG_PAD_L1=1u<<4,GG_PAD_R1=1u<<5 };
int gg_pad_open(void);
uint32_t gg_pad_pressed(void);
void gg_pad_close(void);
#ifdef __cplusplus
}
#endif
#endif
