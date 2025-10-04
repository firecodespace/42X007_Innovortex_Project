<#
Lightweight orchestration script: deploy contract, start bridge, and launch configured game exe.
This skips building the C++ project. Run this from your local machine (Developer permissions not required).

Usage:
  Open PowerShell and run:
    powershell -ExecutionPolicy Bypass -File run_no_build.ps1

Make sure:
- Hardhat node is running on localhost:8545 OR the script will use the running node if present.
- run_config.json contains the proper ExePath to your built game exe (can be on D:).
#>

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Write-Output "Workspace root: $root"

function Get-PidOnPort($port) {
    $conn = Get-NetTCPConnection -LocalPort $port -ErrorAction SilentlyContinue
    if (!$conn) { return $null }
    $ownPid = $conn.OwningProcess
    if ($ownPid -and $ownPid -gt 4) { return $ownPid }
    return $null
}

# 1) Ensure Hardhat node is available (use existing if present)
$hhPort = 8545
$hhProc = Get-PidOnPort -port $hhPort
if ($hhProc) {
    Write-Output "Found node on port $hhPort (PID $hhProc). Using existing node."
} else {
    Write-Output "No local node detected on $hhPort. Please start Hardhat node in another terminal: 'npx hardhat node'"
}

# 2) Deploy contract
Push-Location (Join-Path $root 'blockchain')
try {
    & npx hardhat run --network local scripts/deploy.js
} catch { Write-Output "Deploy failed: $_" }
Pop-Location

# 3) Start or restart bridge
Push-Location (Join-Path $root 'node_bridge')
$bridgePort = 3000
$bridgePid = Get-PidOnPort -port $bridgePort
if ($bridgePid) {
    Write-Output "Stopping existing bridge PID $bridgePid"
    try { Stop-Process -Id $bridgePid -Force } catch { Write-Output ('Could not stop bridge {0}: {1}' -f $bridgePid, $_) }
}
Start-Process -FilePath node -ArgumentList 'bridge.js' -WorkingDirectory (Get-Location) -NoNewWindow -PassThru | Out-Null
Start-Sleep -Seconds 1
Pop-Location

# 4) Launch configured exe
$cfgPath = Join-Path $root 'run_config.json'
if (-not (Test-Path $cfgPath)) {
    Write-Output "run_config.json not found. Copy run_config.json.sample to run_config.json and set ExePath to your built game's exe."
    exit 1
}
try {
    $cfg = Get-Content $cfgPath -Raw | ConvertFrom-Json
    $exe = $cfg.ExePath
    if ($exe -and (Test-Path $exe)) {
        Write-Output "Launching game exe: $exe"
        Start-Process -FilePath $exe -WorkingDirectory (Split-Path $exe)
        Write-Output "Launched $exe"
    } else {
        Write-Output "Configured ExePath not found or invalid: $exe"
    }
} catch {
    Write-Output "Failed to read run_config.json: $_"
}

Write-Output "Done. Bridge running on http://localhost:3000 (if it started). Play the game and the mint will be triggered on win."
