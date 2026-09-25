# RetroHub box art

RetroHub draws PNG covers from `PPSA99202/boxarts/<system>/<ROM stem>.png`.
It also attempts RetroArch's default `thumbnails/<playlist>/Named_Boxarts/`
folders directly, when the native title can read them. It does not fetch artwork
from the internet. Missing images display a simple `NO COVER` label.

If the PS5 title cannot read the RetroArch folder (as with the ROM scan), run
`tools/Build-RetroHubBoxartsFromFTP.ps1` on Windows PowerShell after the library
manifest helper. The script matches existing RetroArch PNG thumbnails to the
ROM names in your manifests and downloads only matching files from your PS5 FTP
server. It accepts `-ThumbnailRoot` if RetroArch stores thumbnails elsewhere.

Example:

```powershell
.\tools\Build-RetroHubBoxartsFromFTP.ps1 -HostAddress 192.168.1.184 -Port 1337
```

Copy the resulting `RetroHub-boxarts` contents with FileZilla into
`/data/homebrew/PPSA99202/boxarts/`, preserving the system folders, then reopen
RetroHub. Example: `boxarts/snes/Super Mario World.png`.
