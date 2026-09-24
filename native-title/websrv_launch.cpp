#include "websrv_launch.h"
#include <stdio.h>

/*
 * RetroArch payload launch adapter.
 *
 * The final transport is intentionally isolated here so the native UI,
 * scanner, controls and system/core mapping do not depend on a second PS5
 * title.  websrv/hbldr is the supported runtime for the RetroArch package
 * used by this project.
 */
extern "C" int gg_launch_retroarch_payload(const char *core_name,const char *content_path){
 if(!core_name||!core_name[0]||!content_path||!content_path[0])return -1;
 /*
  * Transport implementation is the remaining integration item.
  * Keep this fail-closed until the websrv handoff is linked, rather than
  * silently falling back to the discarded PPSA99169 cross-title path.
  */
 return -2;
}
