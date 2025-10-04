<#
Simple launcher for existing game executable. Edit run_config.json to set ExePath.
Usage: PowerShell -File launch_game.ps1
#>
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$cfg = Join-Path $root 'run_config.json'
if (-not (Test-Path $cfg)) {
    Write-Output "run_config.json not found. Create it with the path to your game exe."
    Write-Output "Copy run_config.json.sample to run_config.json and edit ExePath."
    exit 1
}

$json = Get-Content $cfg -Raw | ConvertFrom-Json
$exe = $json.ExePath
if (-not $exe) { Write-Output "ExePath missing in config."; exit 1 }
if (-not (Test-Path $exe)) { Write-Output "Exe not found: $exe"; exit 1 }

Write-Output "Launching game: $exe"
Start-Process -FilePath $exe -WorkingDirectory (Split-Path $exe)
