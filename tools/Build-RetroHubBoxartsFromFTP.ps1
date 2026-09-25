param(
 [Parameter(Mandatory=$true)][string]$HostAddress,
 [int]$Port=1337,
 [string]$UserName="anonymous",
 [string]$Password="anonymous@",
 [string]$Library="",
 [string]$Output="",
 [string]$ThumbnailRoot=""
)
$ErrorActionPreference="Stop"
if(!$Library){$Library=Join-Path $PSScriptRoot 'RetroHub-library'}
if(!$Output){$Output=Join-Path $PSScriptRoot 'RetroHub-boxarts'}
$base="ftp://${HostAddress}:$Port"
$playlists=[ordered]@{
 nes='Nintendo - Nintendo Entertainment System'; snes='Nintendo - Super Nintendo Entertainment System';
 n64='Nintendo - Nintendo 64'; gb='Nintendo - Game Boy'; gbc='Nintendo - Game Boy Color';
 gba='Nintendo - Game Boy Advance'; genesis='Sega - Mega Drive - Genesis';
 segacd='Sega - Mega-CD - Sega CD'; x32='Sega - 32X'; saturn='Sega - Saturn';
 psx='Sony - PlayStation'; atari2600='Atari - 2600'; atari7800='Atari - 7800';
 lynx='Atari - Lynx'; jaguar='Atari - Jaguar'; pce='NEC - PC Engine - TurboGrafx 16';
 arcade='Arcade'; amiga='Commodore - Amiga'; c64='Commodore - 64'
}
function FtpRequest([string]$path,[string]$method) {
 $segments=$path.TrimStart('/').Split('/') | ForEach-Object {[Uri]::EscapeDataString($_)}
 $request=[Net.FtpWebRequest][Net.WebRequest]::Create("$base/%2f$($segments -join '/')")
 $request.Method=$method
 $request.Credentials=[Net.NetworkCredential]::new($UserName,$Password)
 $request.UsePassive=$true
 $request.UseBinary=$true
 $request.KeepAlive=$false
 $request.Timeout=12000
 return $request
}
function ListNames([string]$path) {
 $request=FtpRequest $path ([Net.WebRequestMethods+Ftp]::ListDirectory)
 $response=$request.GetResponse()
 try {
  $reader=New-Object IO.StreamReader($response.GetResponseStream(),[Text.Encoding]::UTF8)
  try {$raw=$reader.ReadToEnd()} finally {$reader.Dispose()}
 } finally {$response.Dispose()}
 return @(($raw -split "`r?`n") | Where-Object {$_} | ForEach-Object {($_ -replace '\\','/').Split('/')[-1]})
}
function Download([string]$remote,[string]$local) {
 $response=(FtpRequest $remote ([Net.WebRequestMethods+Ftp]::DownloadFile)).GetResponse()
 try {
  $inputStream=$response.GetResponseStream()
  $file=[IO.File]::Create($local)
  try {$inputStream.CopyTo($file)} finally {$file.Dispose();$inputStream.Dispose()}
 } finally {$response.Dispose()}
}
if(!$ThumbnailRoot){
 foreach($candidate in @('/data/homebrew/RetroArch/thumbnails','/data/homebrew/RetroArch/.config/retroarch/thumbnails')){
  try {$null=ListNames $candidate;$ThumbnailRoot=$candidate;break} catch {}
 }
}
if(!$ThumbnailRoot){throw 'Could not find RetroArch thumbnails by FTP. Supply -ThumbnailRoot with the folder from RetroArch Settings > Directory > Thumbnails.'}
if(!(Test-Path $Library)){throw "Library manifests missing: $Library. Run Build-RetroHubLibraryFromFTP.ps1 first."}
$folderNames=@(ListNames $ThumbnailRoot)
$total=0
foreach($system in $playlists.Keys){
 $manifest=Join-Path $Library "$system.lst"
 if(!(Test-Path $manifest)){continue}
 $playlist=$playlists[$system]
 $folder=@($folderNames | Where-Object {$_ -ieq $playlist}) | Select-Object -First 1
 if(!$folder){Write-Host "$system : no thumbnail folder '$playlist'";continue}
 $remote="$ThumbnailRoot/$folder/Named_Boxarts"
 try {$files=@(ListNames $remote)}catch{Write-Warning "Cannot list $remote : $_";continue}
 $dir=Join-Path $Output $system
 New-Item -ItemType Directory -Force -Path $dir | Out-Null
 $count=0
 foreach($rom in [IO.File]::ReadAllLines($manifest)){
  if(!$rom -or $rom.StartsWith('#')){continue}
  $title=[IO.Path]::GetFileNameWithoutExtension($rom)
  if(!$title){continue}
  $safe=($title -replace '[&*/:<>?\\|]','_')
  $short=($safe -replace '\s*\(.*$','')
  $cover=@($files | Where-Object {$_ -ieq "$safe.png"}) | Select-Object -First 1
  if(!$cover -and $short){$cover=@($files | Where-Object {$_ -ieq "$short.png"}) | Select-Object -First 1}
  if(!$cover){continue}
  $local=Join-Path $dir "$safe.png"
  if(Test-Path $local){$count++;continue}
  try {Download "$remote/$cover" $local;$count++} catch {Remove-Item -ErrorAction SilentlyContinue $local;Write-Warning "Cannot download $remote/$cover : $_"}
 }
 $total+=$count;Write-Host "$system : $count covers"
}
Write-Host "Found $total matching covers. Copy the contents of $Output by FileZilla to /data/homebrew/PPSA99202/boxarts/ and restart RetroHub."
