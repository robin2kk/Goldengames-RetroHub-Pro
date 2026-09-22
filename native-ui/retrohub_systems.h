#pragma once
#include <stddef.h>

typedef struct RetroSystem {
    const char *id;
    const char *name;
    const char *core;
    const char *extensions;
    unsigned char bg_r,bg_g,bg_b;
    unsigned char accent_r,accent_g,accent_b;
} RetroSystem;

static const RetroSystem RETRO_SYSTEMS[] = {
 {"nes","Nintendo Entertainment System","fceumm_libretro.so","nes|zip",224,224,216,196,32,40},
 {"snes","Super Nintendo","snes9x_libretro.so","sfc|smc|zip",190,190,194,105,76,150},
 {"n64","Nintendo 64","parallel_n64_libretro.so","z64|n64|v64|bin|u1|ndd",35,35,38,225,55,45},
 {"gb","Game Boy","gambatte_libretro.so","gb|zip",137,146,92,43,47,37},
 {"gbc","Game Boy Color","gambatte_libretro.so","gbc|zip",48,38,92,224,66,164},
 {"gba","Game Boy Advance","mgba_libretro.so","gba|zip",46,38,92,125,93,210},
 {"nds","Nintendo DS","desmume2015_libretro.so","nds|zip",232,234,238,55,98,165},
 {"genesis","Sega Genesis","genesis_plus_gx_libretro.so","md|gen|bin|zip",18,18,20,200,34,40},
 {"mastersystem","Master System","genesis_plus_gx_libretro.so","sms|zip",22,35,80,220,38,42},
 {"gamegear","Game Gear","genesis_plus_gx_libretro.so","gg|zip",20,20,22,58,110,205},
 {"segacd","Sega CD","genesis_plus_gx_libretro.so","cue|chd|m3u",20,22,28,50,105,190},
 {"sega32x","Sega 32X","picodrive_libretro.so","32x|bin|zip",18,18,20,225,155,35},
 {"saturn","Sega Saturn","yabause_libretro.so","cue|chd|m3u|zip",22,24,30,55,105,190},
 {"psx","PlayStation","pcsx_rearmed_libretro.so","cue|chd|pbp|m3u|bin|img|iso",194,194,190,38,86,165},
 {"pcengine","PC Engine","mednafen_pce_fast_libretro.so","pce|cue|ccd|chd|toc|m3u",235,235,232,205,40,40},
 {"arcade","Arcade","fbneo_libretro.so","zip|7z",12,12,18,240,45,160},
 {"amiga","Amiga","puae_libretro.so","adf|hdf|lha|zip|7z",25,39,55,238,96,45},
 {"atari2600","Atari 2600","stella2023_libretro.so","a26|bin",32,24,18,154,90,45},
 {"atari7800","Atari 7800","prosystem_libretro.so","a78|bin|cdf",18,18,20,205,205,205},
 {"lynx","Atari Lynx","handy_libretro.so","lnx|lyx|o",25,25,27,230,120,35},
 {"jaguar","Atari Jaguar","virtualjaguar_libretro.so","j64|jag|rom|abs|cof|bin|prg",16,16,18,205,35,35},
 {"wonderswancolor","WonderSwan Color","mednafen_wswan_libretro.so","ws|wsc|zip",230,230,232,60,115,190}
};
static const size_t RETRO_SYSTEM_COUNT=sizeof(RETRO_SYSTEMS)/sizeof(RETRO_SYSTEMS[0]);
