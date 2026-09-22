# Build

This frontend targets ps5-payload-sdk and SDL2, using the same bootstrap/hbldr architecture as NativeHBL.

Required build environment:
- ps5-payload-sdk with prospero-clang
- SDL2, SDL2_image, SDL2_ttf available in the SDK target
- hbldr/elfldr/pt backend sources derived from ps5-payload-dev/websrv under GPL-3.0-or-later

Planned outputs:
- build/goldengames_retrohub_bootstrap.elf
- build/goldengames_retrohub_ui.elf

The project intentionally does not ship ROMs, BIOS files or commercial artwork.
