# RetroHub HD coverflow

The native browser renders at 1920 x 1080. It crops the supplied console
wallpapers to exclude embedded Libretro branding and darkens their console
illustrations as a backdrop. Up to five game covers are displayed in a
coverflow arrangement. The focused cover uses the original colors and a faint
reflection; adjacent covers are dimmed. A missing PNG is represented by a
branded sleeve and the ROM title, without a fabricated game cover.

Cover PNGs come from `/app0/boxarts/<system>/<ROM stem>.png` or, when readable,
the user's existing RetroArch `thumbnails/<playlist>/Named_Boxarts/` folder.
All 19 systems can load art even when a supplied wallpaper is unavailable.
PNG dimensions are limited to 1536 x 1536 and 8 MiB of decoded pixels; seven
recent covers are cached to avoid disk access every frame. User ROMs, BIOS,
commercial art and existing RetroArch configuration are not packaged.

Controls are unchanged: L1/R1 or Up/Down switch systems, Left/Right select a
game, Cross launches it, and Circle rescans the library.
