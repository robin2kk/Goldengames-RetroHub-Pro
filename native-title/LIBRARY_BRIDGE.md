# RetroHub native library bridge

RetroHub is the native title `PPSA99202`. RetroArch remains the separately
installed payload at `/data/homebrew/RetroArch/`; this project does not bundle
RetroArch, cores, ROMs, BIOS, saves, or artwork.

The browser reads `/app0/library/<system>.lst` first. The FTP helper builds
these manifests from the user's existing RetroArch `.lpl` playlists and ROM
folders (including nested disc folders). A line may include the exact playlist
label after a tab, for example:

```
/data/homebrew/RetroArch/roms/psx/Game/Game.cue<TAB>Game (USA)
```

Here `<TAB>` represents one tab character. The path launches the game; the
label matches RetroArch's Named_Boxarts filenames. An optional third tab-separated
field contains the installed core filename from the playlist, such as
`genesis_plus_gx_libretro.so`. RetroHub uses this core for that game. On Windows, run
`tools/Build-RetroHubLibraryFromFTP.ps1 -HostAddress <PS5 IP> -Port 1337`, then
copy its `.lst` files into `/data/homebrew/PPSA99202/library/`. The helper does
not copy games. If no playlist exists for a console, it recursively scans the
ROM folder for formats supported by its assigned core.

If there is no usable manifest, the title attempts a direct folder scan. This
may be blocked by the PS5 title sandbox, so the FTP generated list is preferred.

Up/Down or L1/R1 selects the system, Left/Right selects the game, Circle rescans the
current system, and Cross sends its
content path and matching core to the websrv payload launcher. A failed launch
shows a message in the browser. Launch and return must be checked on PS5 5.10.
