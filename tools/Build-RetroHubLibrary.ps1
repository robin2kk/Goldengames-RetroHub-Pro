param(
  [Parameter(Mandatory=$true)][string]$RetroArchRoot,
  [string]$Output = ".\\RetroHub-library",
  [string]$PS5RetroArchRoot = "/data/homebrew/RetroArch"
)
$ErrorActionPreference = "Stop"
$systems = @{
  nes=@(".nes",".zip"); snes=@(".sfc",".smc",".zip"); n64=@(".z64",".n64",".v64");
  genesis=@(".md",".gen",".bin",".zip"); psx=@(".cue",".chd",".pbp")
}
New-Item -ItemType Directory -Force -Path $Output | Out-Null
foreach($system in $systems.Keys){
  $dir=Join-Path $RetroArchRoot ("roms\"+$system)
  $lines=@()
  if(Test-Path $dir){
    Get-ChildItem -LiteralPath $dir -File | Sort-Object Name | ForEach-Object {
      if($systems[$system] -contains $_.Extension.ToLowerInvariant()){
        $lines += "$PS5RetroArchRoot/roms/$system/$($_.Name)"
      }
    }
  }
  $dest=Join-Path $Output "$system.lst"
  [System.IO.File]::WriteAllLines($dest,$lines,[System.Text.UTF8Encoding]::new($false))
  Write-Host "$system : $($lines.Count) games -> $dest"
}
Write-Host "Copy the generated .lst files into PPSA99202/library/."
