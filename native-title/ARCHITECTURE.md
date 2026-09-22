# Goldengames RetroHub Pro v0.2 — Native Title Architecture

Status: development branch only. v0.1.0 remains frozen.

## Decision

v0.2 moves away from the payload/bootstrap + SDL launcher experiment toward a PS5 homebrew-title architecture.

This is an independent GoldenGames implementation. Public PS5 projects are used as technical references for platform behavior; their UI, branding, assets and source code are not copied into RetroHub.

## Runtime model

```
PS5 Homebrew Launcher
        |
        v
Goldengames RetroHub Pro native title
        |
        +-- native display/presentation
        +-- native DualSense input
        +-- RetroHub system/game database
        +-- GoldenGames per-console UI renderer
        |
        v
RetroArch/content launch
```

The title owns its lifecycle. The shell splash is dismissed only after the display path is ready. The UI remains resident until an explicit launch/exit transition.

## Display contract

- Fixed initial presentation target: 1920x1080.
- Renderer must present an unmistakable GoldenGames diagnostic frame before the full UI.
- A blank/dark background is never accepted as proof of a working UI.
- First acceptance frame contains: GOLDENGAMES RETROHUB PRO, build identifier, five visible carousel cards and a bottom controller-hint bar.
- Full UI keeps each console's visual identity; it is not a generic palette swap.

## Input contract

Input is independent from rendering. The native pad layer obtains the active user, opens the controller, polls samples and converts edge transitions into RetroHub actions.

RetroHub actions:
- left/right: carousel
- up/down: page/category navigation where applicable
- confirm: enter/launch
- back: return one level
- options: utility menu

No SDL controller/event dependency is required by the new title architecture.

## Files and user data

No ROMs, BIOS, commercial box art, saves, history or private runtime state are distributed.

The existing user-facing ROM organization remains a compatibility target:
`/data/homebrew/RetroArch/roms/<system>/`

The native title must not delete or migrate user content automatically.

## Clean-room/reference policy

We may study public documentation and source to learn PS5 platform contracts and known-good behavior. GoldenGames code is written for this project. If any third-party GPL source is ever incorporated rather than merely studied, that file must be clearly attributed and distributed with its required license/source notices.

Technical references currently studied:
- ps5-payload-dev SDK / websrv
- Rufidj NativeHBL
- Mihawk PS5_RetroArch / PS5_Vulkan
- BlackBearReloaded ProsperoLight / native-app foundations

## Acceptance gates

1. Native title launches and stays on screen.
2. Unique GoldenGames diagnostic frame is visible.
3. DualSense navigation works without destabilizing video.
4. System carousel works.
5. Per-console visual identities render.
6. Local box art loads with fallback cards.
7. Game scanning is correct for cartridge and disc formats.
8. RetroArch launch and return path works.
9. PS5 firmware 5.10 hardware validation passes.
10. Only after owner approval: package/release v0.2.
