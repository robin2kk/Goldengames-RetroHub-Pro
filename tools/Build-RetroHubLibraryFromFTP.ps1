param(
 [Parameter(Mandatory=$true)][string]$HostAddress,
 [int]$Port=1337,
 [string]$UserName="anonymous",
 [string]$Password="anonymous@",
 [string]$Output=""
)
$ErrorActionPreference="Stop"
if(!$Output){$Output=Join-Path $PSScriptRoot "RetroHub-library"}
$base="ftp://${HostAddress}:$Port"
$romRoot="/data/homebrew/RetroArch/roms"
$systems=[ordered]@{
 nes=@("nes","zip"); snes=@("sfc","smc","zip"); n64=@("z64","n64","v64","bin","u1","ndd","zip");
 gb=@("gb","zip"); gbc=@("gbc","zip"); gba=@("gba","zip");
 genesis=@("md","gen","bin","zip"); segacd=@("cue","chd","iso","m3u","bin"); x32=@("32x","bin");
 saturn=@("cue","chd","m3u","iso","ccd","mds","bin","zip");
 psx=@("cue","chd","pbp","m3u","iso","img","ccd","mdf","bin","toc","cbn");
 atari2600=@("a26","bin","zip"); atari7800=@("a78","bin","zip");
 lynx=@("lnx","zip"); jaguar=@("j64","jag","zip"); pce=@("pce","zip");
 arcade=@("zip"); amiga=@("adf","hdf","lha"); c64=@("d64","t64","crt")
}
$aliases=@{
 psx=@("ps1","playstation","playstation1","sonyplaystation","sonyplaystation1"); nes=@("nintendoentertainmentsystem");
 snes=@("supernintendo"); n64=@("nintendo64","nintendonintendo64","nintendo64roms"); gb=@("gameboy");
 gbc=@("gameboycolor"); gba=@("gameboyadvance"); genesis=@("megadrive","segagenesis");
 segacd=@("megacd"); x32=@("sega32x"); saturn=@("segasaturn","segasaturnroms");
 pce=@("pcengine","turbografx16"); arcade=@("fbneo"); c64=@("commodore64")
}
$playlists=@{
 nes="Nintendo - Nintendo Entertainment System"; snes="Nintendo - Super Nintendo Entertainment System";
 n64="Nintendo - Nintendo 64"; gb="Nintendo - Game Boy"; gbc="Nintendo - Game Boy Color";
 gba="Nintendo - Game Boy Advance"; genesis="Sega - Mega Drive - Genesis";
 segacd="Sega - Mega-CD - Sega CD"; x32="Sega - 32X"; saturn="Sega - Saturn";
 psx="Sony - PlayStation"; atari2600="Atari - 2600"; atari7800="Atari - 7800";
 lynx="Atari - Lynx"; jaguar="Atari - Jaguar";
 pce="NEC - PC Engine - TurboGrafx 16"; arcade="Arcade";
 amiga="Commodore - Amiga"; c64="Commodore - 64"
}
function Normalize([string]$name){return ($name -replace '[^a-zA-Z0-9]','').ToLowerInvariant()}
function FtpRequest([string]$path,[string]$method) {
 $segments=$path.TrimStart('/').Split('/') | ForEach-Object {[Uri]::EscapeDataString($_)}
 # .NET treats ftp://host/path as relative to the login directory.
 # %2f makes these PS5 paths absolute, like FileZilla's /data/homebrew/.
 $request=[Net.FtpWebRequest][Net.WebRequest]::Create("$base/%2f$($segments -join '/')")
 $request.Method=$method
 $request.Credentials=[Net.NetworkCredential]::new($UserName,$Password)
 $request.UsePassive=$true
 $request.UseBinary=$true
 $request.KeepAlive=$false
 $request.Timeout=12000
 return $request
}
function ReadListing([string]$path,[string]$method) {
 $response=(FtpRequest $path $method).GetResponse()
 try {
  $reader=New-Object IO.StreamReader($response.GetResponseStream(),[Text.Encoding]::UTF8)
  try { return $reader.ReadToEnd() } finally { $reader.Dispose() }
 } finally { $response.Dispose() }
}
function ListNames([string]$path) {
 $details=$false
 try { $raw=ReadListing $path ([Net.WebRequestMethods+Ftp]::ListDirectory) }
 catch {
  $raw=ReadListing $path ([Net.WebRequestMethods+Ftp]::ListDirectoryDetails)
  $details=$true
 }
 $names=New-Object 'System.Collections.Generic.List[string]'
 foreach($row in ($raw -split "`r?`n")){
  if(!$row){continue}
  if($details){
   if($row -notmatch '^[d\-l]\S*\s+\d+\s+\S+\s+\S+\s+\d+\s+\S+\s+\d+\s+\S+\s+(.+)$'){continue}
   $name=$Matches[1]
  }else{$name=($row -replace '\\','/').Split('/')[-1]}
  if($name -and $name -ne '.' -and $name -ne '..'){$names.Add($name)}
 }
 return $names.ToArray()
}
function ReadFile([string]$path) {return ReadListing $path ([Net.WebRequestMethods+Ftp]::DownloadFile)}
function ValidPath([string]$path) {
 return $path.StartsWith('/data/homebrew/RetroArch/') -or
        $path.StartsWith('/mnt/usb') -or $path.StartsWith('/mnt/ext')
}
function AddEntry([string]$system,[string]$path,[string]$title,[string]$corePath,[hashtable]$target) {
 if(!$path -or !(ValidPath $path) -or $path.Contains("`n") -or $path.Contains("`r") -or $path.Contains("`t")){return}
 $ext=[IO.Path]::GetExtension($path).TrimStart('.').ToLowerInvariant()
 if($systems[$system] -notcontains $ext){return}
 if(!$target.ContainsKey($path)){
  if(!$title){$title=[IO.Path]::GetFileNameWithoutExtension($path)}
  $target[$path]=($title -replace "[`r`n`t]",' ').Trim()
 }
 $coreName=($corePath -replace '\\','/').Split('/')[-1]
 if($coreName -cmatch '^[a-z0-9_-]+_libretro\.so$'){$script:entryCores[$path]=$coreName}
}
function WalkFolder([string]$system,[string]$folder,[int]$depth,[hashtable]$target) {
 try {$names=@(ListNames $folder)}catch{Write-Warning "Cannot list $folder : $_";return}
 $hasCue=($system -eq 'psx' -or $system -eq 'saturn' -or $system -eq 'segacd') -and @($names | Where-Object {$_ -match '(?i)\.cue$'}).Count -gt 0
 foreach($name in $names){
  if(!$name -or $name -eq '.' -or $name -eq '..' -or $name.Contains('/') -or
     $name.Contains("`n") -or $name.Contains("`r")){continue}
  $full="$folder/$name"
  $ext=[IO.Path]::GetExtension($name).TrimStart('.').ToLowerInvariant()
  if($systems[$system] -contains $ext){
   if(($system -eq 'psx' -or $system -eq 'saturn' -or $system -eq 'segacd') -and $ext -eq 'bin' -and $hasCue){continue}
   AddEntry $system $full '' '' $target
  }elseif($depth -gt 0 -and !$ext){
   WalkFolder $system $full ($depth-1) $target
  }
 }
}
$lookup=@{}
$entries=@{}
$entryCores=@{}
foreach($system in $systems.Keys){
 $entries[$system]=@{}
 $lookup[(Normalize $system)]=$system
 $lookup[(Normalize $playlists[$system])]=$system
 foreach($alias in $aliases[$system]){$lookup[(Normalize $alias)]=$system}
}
$playlistCount=0
foreach($root in @('/data/homebrew/RetroArch/.config/retroarch/playlists','/data/homebrew/RetroArch/playlists')){
 try {$files=@(ListNames $root)}catch{continue}
 foreach($file in $files){
  if(!$file.EndsWith('.lpl',[StringComparison]::OrdinalIgnoreCase)){continue}
  try {$parsed=(ReadFile "$root/$file" | ConvertFrom-Json)}
  catch {Write-Warning "Cannot read playlist $file : $_";continue}
  $playlistSystem=$lookup[(Normalize ([IO.Path]::GetFileNameWithoutExtension($file)))]
  foreach($item in @($parsed.items)){
   if(!$item){continue}
   $system=$playlistSystem
   if($item.db_name){
    $candidate=$lookup[(Normalize ([IO.Path]::GetFileNameWithoutExtension([string]$item.db_name)))]
    if($candidate){$system=$candidate}
   }
   if(!$system){continue}
   $before=$entries[$system].Count
   AddEntry $system ([string]$item.path) ([string]$item.label) ([string]$item.core_path) $entries[$system]
   if($entries[$system].Count -gt $before){$playlistCount++}
  }
 }
}
Write-Host "RetroArch playlists: $playlistCount matching games"
try {$folders=@(ListNames $romRoot)}catch {$folders=@();Write-Warning "Cannot list $romRoot : $_"}
foreach($system in $systems.Keys){
 foreach($folder in $folders){
  $normalized=Normalize $folder
  if($normalized -ne $system -and $aliases[$system] -notcontains $normalized){continue}
  WalkFolder $system "$romRoot/$folder" 3 $entries[$system]
 }
}
New-Item -ItemType Directory -Force -Path $Output | Out-Null
foreach($system in $systems.Keys){
 $lines=New-Object 'System.Collections.Generic.List[string]'
 foreach($path in @($entries[$system].Keys | Sort-Object)){
  $line="$path`t$($entries[$system][$path])"
  if($entryCores.ContainsKey($path)){$line+="`t$($entryCores[$path])"}
  $lines.Add($line)
 }
 $file=Join-Path $Output "$system.lst"
 $bytes=(New-Object Text.UTF8Encoding($false)).GetBytes(($lines.ToArray()) -join "`n")
 [IO.File]::WriteAllBytes($file,$bytes)
 Write-Host "$system : $($lines.Count) games"
}
Write-Host "Copy the .lst files from $Output to /data/homebrew/PPSA99202/library/."
Write-Host 'Restart RetroHub or press Circle on each system to rescan.'
