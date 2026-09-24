param(
  [Parameter(Mandatory=$true)][string]$ContentRoot,
  [string]$Output = ".\\RetroHub-library"
)
$ErrorActionPreference = "Stop"
$systems = @{
  nes=@(".nes",".zip"); snes=@(".sfc",".smc",".zip"); n64=@(".z64",".n64",".v64");
  genesis=@(".md",".gen",".bin",".zip"); psx=@(".cue",".chd",".pbp")
}
New-Item -ItemType Directory -Force -Path $Output | Out-Null
foreach($system in $systems.Keys){
  $dir=Join-Path $ContentRoot $system
  $lines=@()
  if(Test-Path $dir){
    Get-ChildItem -LiteralPath $dir -File | Sort-Object Name | ForEach-Object {
      if($systems[$system] -contains $_.Extension.ToLowerInvariant()){
        $lines += "/app0/content/$system/$($_.Name)"
      }
    }
  }
  $dest=Join-Path $Output "$system.lst"
  [System.IO.File]::WriteAllLines($dest,$lines,[System.Text.UTF8Encoding]::new($false))
  Write-Host "$system : $($lines.Count) games -> $dest"
}
Write-Host "Copy the generated .lst files into PPSA99202/library/."
