#ifndef GG_CONSOLE_UI_H
#define GG_CONSOLE_UI_H
#include "diagnostic_frame.h"
#include "rom_scanner.h"
#ifdef __cplusplus
extern "C" {
#endif
void gg_draw_console_browser(GGSurface s,int system,int selected_game,const GGGameList *list);
#ifdef __cplusplus
}
#endif
#endif
