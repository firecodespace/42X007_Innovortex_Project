Param(
  [string]$BuildDir = "PvP_BlockChain\build\Release",
  [string]$OutDir = "releases",
  [string]$Version = "0.1.0"
)

New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$timestamp = Get-Date -Format yyyyMMdd-HHmmss
$zipName = "GladiatorArena-$Version-win-x64-$timestamp.zip"
$zipPath = Join-Path -Path $OutDir -ChildPath $zipName
$tmp = Join-Path $env:TEMP "gladiator_release_$timestamp"
$resources = Join-Path $PWD "PvP_BlockChain\Resources"

Write-Host "Packaging build from: $BuildDir"

Remove-Item -Recurse -Force -ErrorAction SilentlyContinue $tmp
New-Item -ItemType Directory -Path $tmp | Out-Null

Get-ChildItem -Path $BuildDir -Filter "PvP_BlockChain*" -File | ForEach-Object {
  Copy-Item $_.FullName -Destination $tmp
}

if (Test-Path $resources) {
  Copy-Item -Path $resources -Destination $tmp -Recurse
}

if (Test-Path $zipPath) {
  Remove-Item $zipPath -Force
}

Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory($tmp, $zipPath)

Remove-Item -Recurse -Force $tmp

Write-Host "Created release: $zipPath"
