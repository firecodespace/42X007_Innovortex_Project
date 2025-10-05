Param(
  [string]$BuildDir = "PvP_BlockChain\build\Release",
  [string]$OutDir = "releases",
  [string]$Version = "0.1.0"
)

# Create output dir
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$timestamp = Get-Date -Format yyyyMMdd-HHmmss
$zipName = "GladiatorArena-$Version-win-x64-$timestamp.zip"
$zipPath = Join-Path -Path $OutDir -ChildPath $zipName

Write-Host "Packaging build from: $BuildDir"

# Files to include (exe, dlls, Resources)
$files = @(
  Join-Path $BuildDir 'PvP_BlockChain.exe'),
  Join-Path $BuildDir 'PvP_BlockChain.pdb'
)

# Include Resources folder if exists
$resources = Join-Path $PWD 'PvP_BlockChain\Resources'
if (Test-Path $resources) {
  Write-Host "Including resources: $resources"
  Add-Type -AssemblyName System.IO.Compression.FileSystem
  [System.IO.Compression.ZipFile]::CreateFromDirectory($resources, "$zipPath.tmp.resources.zip")
}

# Create a temporary folder with executable + resources
$tmp = Join-Path $env:TEMP "gladiator_release_$timestamp"
Remove-Item -Recurse -Force -ErrorAction SilentlyContinue $tmp
New-Item -ItemType Directory -Path $tmp | Out-Null

# Copy exe and DLLs (pattern)
Get-ChildItem -Path $BuildDir -Filter "PvP_BlockChain*" -File | ForEach-Object { Copy-Item $_.FullName -Destination $tmp }

# Copy resource folder if present
if (Test-Path $resources) { Copy-Item -Path $resources -Destination $tmp -Recurse }

# Zip
if (Test-Path $zipPath) { Remove-Item $zipPath -Force }
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory($tmp, $zipPath)

# Cleanup
Remove-Item -Recurse -Force $tmp

Write-Host "Created release: $zipPath"

*** End Patch