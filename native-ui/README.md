# Goldengames RetroHub Pro v0.2.0 — Native UI

Development branch for the PS5 SDL2 native frontend.

## Goals
- Keep v0.1.0/main untouched.
- Native controller-first 1920x1080 UI.
- Each console has a distinct visual identity.
- Scan user-owned content only from /data/homebrew/RetroArch/roms/<system>/.
- Launch the existing RetroArch installation with a mapped libretro core.
- Never bundle ROMs, BIOS, saves, history, or commercial box art.

## Runtime
The bootstrap ELF is loaded first. It launches the UI ELF and supervises launch requests, following NativeHBL/websrv's hbldr model.

Expected RetroArch root: /data/homebrew/RetroArch
Expected ROM root: /data/homebrew/RetroArch/roms
Expected core root: /data/homebrew/RetroArch/cores

Firmware status: 5.10 is the owner's hardware test target. Other firmware versions remain unverified until tested.
