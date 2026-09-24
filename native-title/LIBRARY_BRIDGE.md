# RetroHub native library bridge

RetroHub is the native title `PPSA99202`. RetroArch remains the separately
installed payload at `/data/homebrew/RetroArch/`; this project does not bundle
RetroArch, cores, ROMs, BIOS, saves, or artwork.

The browser first scans `/data/homebrew/RetroArch/roms/` directly, recognizing
common folder names such as `ps1`, `psx`, `PlayStation`, `Mega Drive`, and
`Sony - PlayStation` for the matching system.
If the folder cannot be read or contains no recognized games, it reads
`/app0/library/<system>.lst`. Each manifest entry is the full path to a game
that the payload can open, for example:

```
/data/homebrew/RetroArch/roms/nes/Super Mario Bros.nes
```

Generate all 19 manifest files on a PC with
`tools/Build-RetroHubLibrary.ps1 -RetroArchRoot <local RetroArch folder>`.
Copy the resulting `.lst` files into `PPSA99202/library/` if direct scanning
does not find the installed games. The list contains paths only; no game files
are copied into RetroHub.

The screen reports whether it found a ROM folder or manifest for each system.
If both say `NOT FOUND`, verify the PS5 path above or add the matching `.lst`.

L1/R1 selects the system, Left/Right selects the game, Circle rescans the
current system, and Cross sends its
content path and matching core to the websrv payload launcher. A failed launch
shows a message in the browser. Launch and return must be checked on PS5 5.10.
