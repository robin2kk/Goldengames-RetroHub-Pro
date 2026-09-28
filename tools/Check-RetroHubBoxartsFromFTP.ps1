param(
 [Parameter(Mandatory=$true)][string]$HostAddress,
 [int]$Port=1337,
 [string]$UserName='anonymous',
 [string]$Password='anonymous@',
 [string]$Library='',
 [switch]$Apply
)
$ErrorActionPreference='Stop'
if(!$Library){$Library=Join-Path $PSScriptRoot 'RetroHub-library'}
if(!(Test-Path $Library)){throw "No encuentro $Library. Usa -Library con la ruta de RetroHub-library."}
$base="ftp://${HostAddress}:$Port"
$root='/data/homebrew/PPSA99202/boxarts'
function Request([string]$path,[string]$method){
 $segments=$path.TrimStart('/').Split('/') | ForEach-Object {[Uri]::EscapeDataString($_)}
 $request=[Net.FtpWebRequest][Net.WebRequest]::Create("$base/%2f$($segments -join '/')")
 $request.Method=$method
 $request.Credentials=[Net.NetworkCredential]::new($UserName,$Password)
 $request.UsePassive=$true;$request.UseBinary=$true;$request.KeepAlive=$false
 $request.Timeout=12000;$request.ReadWriteTimeout=12000
 return $request
}
function ListNames([string]$path){
 $response=(Request $path ([Net.WebRequestMethods+Ftp]::ListDirectory)).GetResponse()
 try{
  $reader=New-Object IO.StreamReader($response.GetResponseStream(),[Text.Encoding]::UTF8)
  try{$raw=$reader.ReadToEnd()}finally{$reader.Dispose()}
 }finally{$response.Dispose()}
 return @(($raw -split "`r?`n") | Where-Object {$_} | ForEach-Object {($_ -replace '\\','/').Split('/')[-1]})
}
function Download([string]$path){
 $response=(Request $path ([Net.WebRequestMethods+Ftp]::DownloadFile)).GetResponse()
 try{
  $stream=$response.GetResponseStream()
  $memory=New-Object IO.MemoryStream
  try{$stream.CopyTo($memory);return $memory.ToArray()}finally{$memory.Dispose();$stream.Dispose()}
 }finally{$response.Dispose()}
}
function Upload([string]$path,[byte[]]$bytes){
 $request=Request $path ([Net.WebRequestMethods+Ftp]::UploadFile)
 $request.ContentLength=$bytes.Length
 $stream=$request.GetRequestStream()
 try{$stream.Write($bytes,0,$bytes.Length)}finally{$stream.Dispose()}
 $response=$request.GetResponse();$response.Dispose()
}
function Safe([string]$name){return ($name -replace '[&*/:<>?\\|]','_').Trim()}
function Key([string]$name){return (($name.ToLowerInvariant()) -replace '[^\p{L}\p{N}]','')}
function RegionFree([string]$name){
 return ($name -replace '(?i)\s*[\(\[]\s*(USA|Europe|Japan|World|En|Fr|De|Es|Rev\s*\w*)\s*[\)\]]','').Trim()
}
$report=New-Object 'System.Collections.Generic.List[object]'
$totals=[ordered]@{Exact=0;Repairable=0;Copied=0;Missing=0;Ambiguous=0;Errors=0}
foreach($system in @('nes','snes','n64','gb','gbc','gba','genesis','segacd','x32','saturn','psx','atari2600','atari7800','lynx','jaguar','pce','arcade','amiga','c64')){
 $manifest=Join-Path $Library "$system.lst"
 if(!(Test-Path $manifest)){continue}
 $remote="$root/$system"
 try{$files=@(ListNames $remote | Where-Object {$_ -match '(?i)\.png$'})}
 catch{$files=@();Write-Warning "No pude leer $remote : $_"}
 $count=0
 foreach($line in [IO.File]::ReadAllLines($manifest)){
  if(!$line -or $line.StartsWith('#')){continue}
  $parts=$line.Split(@("`t"),3,[StringSplitOptions]::None)
  $title=if($parts.Count -gt 1 -and $parts[1]){$parts[1]}else{[IO.Path]::GetFileNameWithoutExtension($parts[0])}
  if(!$title){continue}
  $targetName="$(Safe $title).png"
  $source='';$state='Missing'
  if($files -ccontains $targetName){$state='Exact'}
  else{
   $stem=[IO.Path]::GetFileNameWithoutExtension($parts[0])
   $keys=@((Key $title),(Key $stem),(Key (RegionFree $title)),(Key (RegionFree $stem))) | Where-Object {$_} | Select-Object -Unique
   foreach($key in $keys){
    $candidates=@($files | Where-Object {
     $fileTitle=[IO.Path]::GetFileNameWithoutExtension($_)
     (Key $fileTitle) -eq $key -or (Key (RegionFree $fileTitle)) -eq $key
    })
    if($candidates.Count -eq 1){$source=$candidates[0];$state='Repairable';break}
    if($candidates.Count -gt 1){$state='Ambiguous'}
   }
  }
  if($state -eq 'Repairable' -and $Apply){
   try{
    $bytes=Download "$remote/$source"
    Upload "$remote/$targetName" $bytes
    $files+=@($targetName)
    $state='Copied'
   }catch{$state='Errors';Write-Warning "No pude copiar $system/$source a $targetName : $_"}
  }
  $totals[$state]++;$count++
  $report.Add([pscustomobject]@{System=$system;Title=$title;Status=$state;ExistingFile=$source;ExpectedFile=$targetName})
 }
 Write-Host "$system : $count juegos, $($files.Count) PNG"
}
$reportPath=Join-Path $PSScriptRoot 'RetroHub-carátulas-informe.csv'
$report | Export-Csv -Path $reportPath -NoTypeInformation -Encoding UTF8
Write-Host "Resultado: $($totals.Exact) exactas; $($totals.Repairable) renombrables; $($totals.Copied) copiadas; $($totals.Missing) sin imagen; $($totals.Ambiguous) ambiguas; $($totals.Errors) errores."
Write-Host "Informe: $reportPath"
if(!$Apply){Write-Host 'Para crear copias con los nombres correctos en la PS5, ejecuta otra vez con -Apply.'}
