# RetroArch integration plan for v0.2

## Supported engine

GoldenGames RetroHub Pro v0.2 targets the PS5 payload RetroArch build from
`tsuramatsu1/retroarch`, which is based on John Törnblom's RetroArch port in
`ps5-payload-dev/websrv`.

This is intentionally **not** coupled to Mihawk's native RetroArch title and
does not copy Mihawk source, UI, or assets.

## Confirmed payload launch contract

The existing RetroArch package is launched by websrv/hbldr with:

- executable: `/data/homebrew/RetroArch/retroarch.elf`
- working directory / HOME: `/data/homebrew/RetroArch`
- arguments normally include:
  `-f -c /data/homebrew/RetroArch/retroarch.cfg`

websrv's launcher supports arbitrary payload arguments and environment values.
Therefore a selected game can be passed to RetroArch without converting
RetroArch into a second PS5 native title.

Target argument shape:

```
-f
-c
/data/homebrew/RetroArch/retroarch.cfg
-L
/data/homebrew/RetroArch/.config/retroarch/cores/<core>_libretro.so
/data/homebrew/RetroArch/roms/<system>/<game>
```

## Architecture

RetroHub remains the native PS5 title `PPSA99202`.

The ROM library remains in the existing RetroArch folder. RetroHub does not
bundle or duplicate ROMs, BIOS files, saves, commercial artwork, or RetroArch
binaries.

A small generated library manifest provides the visible game list to RetroHub.
Each manifest line stores the real RetroArch payload path for that game.

On Cross, RetroHub will hand the selected payload path + arguments to a small
launch bridge compatible with websrv/hbldr. The previous cross-title
`PPSA99169` SystemService launcher is experimental only and is not the final
v0.2 engine.

## Controls

- L1 / R1: previous / next console
- D-pad Left / Right: previous / next game
- Cross: launch selected game
