param(
 [Parameter(Mandatory=$true)][string]$HostAddress,
 [int]$Port=1337,
 [string]$UserName="anonymous",
 [string]$Password="anonymous@",
 [string]$Output=(Join-Path $PSScriptRoot "RetroHub-library")
)
$ErrorActionPreference="Stop"
$base="ftp://${HostAddress}:$Port"
$romRoot="/data/homebrew/RetroArch/roms"
$targetRoot="/data/homebrew/PPSA99202/library"
$systems=[ordered]@{
 nes=@("nes","zip"); snes=@("sfc","smc","zip"); n64=@("z64","n64","v64");
 gb=@("gb","zip"); gbc=@("gbc","zip"); gba=@("gba","zip");
 genesis=@("md","gen","bin","zip"); segacd=@("cue","chd"); x32=@("32x","bin");
 saturn=@("cue","chd"); psx=@("cue","chd","pbp");
 atari2600=@("a26","bin","zip"); atari7800=@("a78","bin","zip");
 lynx=@("lnx","zip"); jaguar=@("j64","jag","zip"); pce=@("pce","zip");
 arcade=@("zip"); amiga=@("adf","hdf","lha"); c64=@("d64","t64","crt")
}
$aliases=@{
 psx=@("ps1","playstation","sonyplaystation"); nes=@("nintendoentertainmentsystem");
 snes=@("supernintendo"); n64=@("nintendo64"); gb=@("gameboy");
 gbc=@("gameboycolor"); gba=@("gameboyadvance"); genesis=@("megadrive","segagenesis");
 segacd=@("megacd"); x32=@("sega32x"); saturn=@("segasaturn");
 pce=@("pcengine","turbografx16"); arcade=@("fbneo"); c64=@("commodore64")
}
function FtpRequest([string]$path,[string]$method) {
 $segments=$path.TrimStart('/').Split('/') | ForEach-Object {[Uri]::EscapeDataString($_)}
 $request=[Net.FtpWebRequest][Net.WebRequest]::Create("$base/$($segments -join '/')")
 $request.Method=$method
 $request.Credentials=[Net.NetworkCredential]::new($UserName,$Password)
 $request.UsePassive=$true
 $request.UseBinary=$true
 $request.KeepAlive=$false
 return $request
}
function ListNames([string]$path) {
 $response=(FtpRequest $path ([Net.WebRequestMethods+Ftp]::ListDirectory)).GetResponse()
 try {
  $reader=New-Object IO.StreamReader($response.GetResponseStream(),[Text.Encoding]::UTF8)
  try { $raw=$reader.ReadToEnd() } finally { $reader.Dispose() }
  return @($raw -split "`r?`n" | Where-Object {$_ -and $_ -ne '.' -and $_ -ne '..'} | ForEach-Object {($_ -replace '\\','/').Split('/')[-1]})
 } finally { $response.Dispose() }
}
function UploadBytes([string]$path,[byte[]]$bytes) {
 $request=FtpRequest $path ([Net.WebRequestMethods+Ftp]::UploadFile)
 $request.ContentLength=$bytes.Length
 $stream=$request.GetRequestStream()
 try { $stream.Write($bytes,0,$bytes.Length) } finally { $stream.Dispose() }
 $response=$request.GetResponse()
 try { Write-Host "Uploaded $path" } finally { $response.Dispose() }
}
$folders=@(ListNames $romRoot)
if($folders.Count -eq 0){throw "FTP could not list any folders under $romRoot"}
New-Item -ItemType Directory -Force -Path $Output | Out-Null
foreach($system in $systems.Keys){
 $lines=New-Object 'System.Collections.Generic.List[string]'
 foreach($folder in $folders){
  $normalized=($folder -replace '[^a-zA-Z0-9]','').ToLowerInvariant()
  if($normalized -ne $system -and $aliases[$system] -notcontains $normalized){continue}
  try { $games=@(ListNames "$romRoot/$folder") }
  catch { Write-Warning "Cannot list $romRoot/$folder : $_"; continue }
  foreach($game in $games){
   if($game.Contains('/') -or $game.Contains("`n") -or $game.Contains("`r")){continue}
   $ext=[IO.Path]::GetExtension($game).TrimStart('.').ToLowerInvariant()
   if($systems[$system] -contains $ext){$lines.Add("$romRoot/$folder/$game")}
  }
 }
 $path=Join-Path $Output "$system.lst"
 $bytes=(New-Object Text.UTF8Encoding($false)).GetBytes(($lines | Sort-Object -Unique) -join "`n")
 [IO.File]::WriteAllBytes($path,$bytes)
 UploadBytes "$targetRoot/$system.lst" $bytes
 Write-Host "$system : $($lines.Count) entries"
}
Write-Host "Done. Close and reopen RetroHub, or press Circle on each system to rescan."
