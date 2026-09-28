param(
 [string]$Report='',
 [string]$Output='',
 [string[]]$Systems=@(),
 [int]$Limit=0
)
$ErrorActionPreference='Stop'
[Net.ServicePointManager]::SecurityProtocol=[Net.SecurityProtocolType]::Tls12
if(!$Report){$Report=Join-Path $PSScriptRoot 'RetroHub-carátulas-informe.csv'}
if(!$Output){$Output=Join-Path $PSScriptRoot 'RetroHub-boxarts-oficial'}
if(!(Test-Path $Report)){throw "No encuentro el informe: $Report"}
$catalog=@{
 nes='Nintendo - Nintendo Entertainment System'; snes='Nintendo - Super Nintendo Entertainment System';
 n64='Nintendo - Nintendo 64'; gb='Nintendo - Game Boy'; gbc='Nintendo - Game Boy Color';
 gba='Nintendo - Game Boy Advance'; genesis='Sega - Mega Drive - Genesis';
 segacd='Sega - Mega-CD - Sega CD'; x32='Sega - 32X'; saturn='Sega - Saturn';
 psx='Sony - PlayStation'; atari2600='Atari - 2600'; atari7800='Atari - 7800';
 lynx='Atari - Lynx'; jaguar='Atari - Jaguar'; pce='NEC - PC Engine - TurboGrafx 16';
 arcade='Arcade'; amiga='Commodore - Amiga'; c64='Commodore - 64'
}
function Key([string]$value){
 $v=$value -replace '(?i)\s*-0[1-9]$',''
 $v=$v -replace '(?i)\s*\((Track\s*\d+|Rev\s*\w+|(?:En|Fr|De|Es|It)(?:,[A-Za-z]+)*|(?:USA|Europe|Japan|World)(?:,\s*(?:USA|Europe|Japan|World))*|U|E|J|V\d+(?:\.\d+)?|Disc\s*\d+(?:\s*of\s*\d+)?)\)',''
 $v=$v -replace '(?i)\s*\[(?:!|[A-Z]{2,5}-\d+)\]',''
 return ($v.ToLowerInvariant() -replace '[^\p{L}\p{N}]','')
}
function Region([string]$value){
 if($value -match '(?i)\((USA|U)(?:,|\))'){return 'USA'}
 if($value -match '(?i)\((Europe|E)(?:,|\))'){return 'Europe'}
 if($value -match '(?i)\((Japan|J)(?:,|\))'){return 'Japan'}
 return 'USA'
}
function Choose([string]$title,[object[]]$files){
 $key=Key $title
 if(!$key){return $null}
 $candidates=@($files | Where-Object {$_.Key -eq $key})
 if($candidates.Count -eq 0){return $null}
 if($candidates.Count -eq 1){return $candidates[0]}
 $region=Region $title
 $regional=@($candidates | Where-Object {$_.Name -match "(?i)\($region\)"})
 if($regional.Count -eq 1){return $regional[0]}
 $exact=@($candidates | Where-Object {$_.Name -ieq "$title.png"})
 if($exact.Count -eq 1){return $exact[0]}
 return $null
}
$rows=@(Import-Csv -Path $Report -Encoding UTF8 | Where-Object {
 $_.Status -eq 'Missing' -and (!$Systems.Count -or $Systems -contains $_.System)
})
if(!$rows.Count){Write-Host 'No hay juegos pendientes en el informe para esos sistemas.';exit 0}
New-Item -ItemType Directory -Force -Path $Output | Out-Null
$results=New-Object 'System.Collections.Generic.List[object]'
$downloaded=0
foreach($system in @($rows.System | Select-Object -Unique)){
 if(!$catalog.ContainsKey($system)){continue}
 $base='https://thumbnails.libretro.com/'+[Uri]::EscapeDataString($catalog[$system])+'/Named_Boxarts/'
 try{
  $html=(Invoke-WebRequest -UseBasicParsing -Uri $base -TimeoutSec 45).Content
  $files=New-Object 'System.Collections.Generic.List[object]'
  foreach($match in [regex]::Matches($html,'href="([^"/]+\.png)"',[Text.RegularExpressions.RegexOptions]::IgnoreCase)){
   $href=[Net.WebUtility]::HtmlDecode($match.Groups[1].Value)
   $name=[Uri]::UnescapeDataString($href)
   if($name -match '[\\/]'){continue}
   $files.Add([pscustomobject]@{Name=$name;Href=$href;Key=(Key ([IO.Path]::GetFileNameWithoutExtension($name)))})
  }
  if($files.Count -eq 0){throw "El catálogo no devolvió nombres PNG: $base"}
  Write-Host "$system : $($files.Count) carátulas disponibles en Libretro"
 }catch{
  Write-Warning "No pude leer el catálogo oficial de $system : $_"
  foreach($row in @($rows | Where-Object {$_.System -eq $system})){
   $results.Add([pscustomobject]@{System=$system;Title=$row.Title;Status='CatalogError';Source='';ExpectedFile=$row.ExpectedFile})
  }
  continue
 }
 $dir=Join-Path $Output $system
 New-Item -ItemType Directory -Force -Path $dir | Out-Null
 foreach($row in @($rows | Where-Object {$_.System -eq $system})){
  $selected=Choose $row.Title ($files.ToArray())
  $status='NotFound';$source=''
  if($selected){
   $source=$selected.Name
   $target=Join-Path $dir $row.ExpectedFile
   if(Test-Path $target){$status='AlreadyDownloaded'}
   elseif($Limit -gt 0 -and $downloaded -ge $Limit){$status='LimitReached'}
   else{
    try{
     Invoke-WebRequest -UseBasicParsing -Uri ($base+$selected.Href) -OutFile $target -TimeoutSec 60
     if((Get-Item $target).Length -gt 8MB){Remove-Item $target;$status='TooLarge'}
     else{$status='Downloaded';$downloaded++}
    }catch{
     Remove-Item -ErrorAction SilentlyContinue $target
     $status='DownloadError';Write-Warning "No pude descargar $system/$source : $_"
    }
   }
  }
  $results.Add([pscustomobject]@{System=$system;Title=$row.Title;Status=$status;Source=$source;ExpectedFile=$row.ExpectedFile})
 }
}
$resultsPath=Join-Path $PSScriptRoot 'RetroHub-descarga-oficial-informe.csv'
$results | Export-Csv -Path $resultsPath -NoTypeInformation -Encoding UTF8
foreach($group in @($results | Group-Object System)){
 $counts=@($group.Group | Group-Object Status | ForEach-Object {"$($_.Name)=$($_.Count)"}) -join ', '
 Write-Host "$($group.Name) : $counts"
}
Write-Host "Descargadas: $downloaded. Archivos: $Output"
Write-Host "Informe: $resultsPath"
Write-Host 'Copia solo las carpetas dentro de RetroHub-boxarts-oficial a /data/homebrew/PPSA99202/boxarts/ usando FileZilla.'
