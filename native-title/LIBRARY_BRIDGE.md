# RetroHub native content bridge

RetroHub is a separate native PS5 title. A title cannot enumerate another title's `/app0`.
To avoid duplicating ROM files, RetroHub supports small per-system manifests under
`/app0/library/`. Each non-comment line is the path the RetroArch title will see.

Default RetroArch title target: `PPSA99169`.

Examples:

`library/nes.lst`
```
/app0/content/nes/Super Mario Bros.nes
```

`library/snes.lst`
```
/app0/content/snes/Super Mario World.sfc
```

Controls:
- L1 / R1: previous / next system
- D-pad Left / Right: previous / next game
- Cross: launch selected game in RetroArch

The ROM itself exists only in the RetroArch title. RetroHub stores only the manifest entry.
No ROM, BIOS, commercial artwork, save data, or RetroArch binary is distributed by RetroHub.
