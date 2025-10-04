# Orchestrator script: start local blockchain, deploy, start bridge, build and run game
# Run this from PowerShell (prefer Developer Command Prompt for msbuild availability)

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Write-Output "Workspace root: $root"

# Helper: get numeric PID from Get-NetTCPConnection result (filter out 0 / Idle)
function Get-PidOnPort($port) {
    $conn = Get-NetTCPConnection -LocalPort $port -ErrorAction SilentlyContinue
    if (!$conn) { return $null }
    $ownPid = $conn.OwningProcess
    if ($ownPid -and $ownPid -gt 4) { return $ownPid }
    return $null
}

# 1) Start Hardhat node if not running on 8545
$hhPort = 8545
$hhProc = Get-PidOnPort -port $hhPort
if ($hhProc) {
    Write-Output "Found process on port $hhPort (PID $hhProc). Will use existing node."
} else {
    Write-Output "Starting Hardhat node..."
    Start-Process -FilePath npx -ArgumentList 'hardhat node' -WorkingDirectory (Join-Path $root 'blockchain') -NoNewWindow -PassThru | Out-Null
    Start-Sleep -Seconds 3
}

# 2) Deploy contract
Write-Output "Deploying GameNFT to local node..."
Push-Location (Join-Path $root 'blockchain')
try {
    & npx hardhat run --network local scripts/deploy.js
} catch {
    Write-Output "Deploy script failed: $_"
}
Pop-Location

# 3) Start Node bridge
Write-Output "Starting node bridge..."
Push-Location (Join-Path $root 'node_bridge')
$bridgePort = 3000
$bridgePid = Get-PidOnPort -port $bridgePort
if ($bridgePid) {
    Write-Output "Stopping existing bridge PID $bridgePid"
    try { Stop-Process -Id $bridgePid -Force -ErrorAction Stop } catch { Write-Output ('Could not stop process {0}: {1}' -f $bridgePid, $_) }
}
Start-Process -FilePath node -ArgumentList 'bridge.js' -WorkingDirectory (Get-Location) -PassThru | Out-Null
Start-Sleep -Seconds 1
Pop-Location

# 4) Build game using msbuild (try to locate msbuild)
Write-Output "Building C++ game (PvP_BlockChain) - looking for msbuild..."
function Find-MSBuild {
    # Try where.exe
    try {
        $p = & where msbuild 2>$null | Select-Object -First 1
        if ($p) { return $p }
    } catch {}

    # Try common Visual Studio 2022 location (Community/Professional/Enterprise)
    $candidates = @(
        "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe",
        "C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\MSBuild\\Current\\Bin\\MSBuild.exe",
        "C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\MSBuild\\Current\\Bin\\MSBuild.exe",
        "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\MSBuild\\Current\\Bin\\MSBuild.exe"
    )
    foreach ($c in $candidates) { if (Test-Path $c) { return $c } }
    return $null
}

$msb = Find-MSBuild
if ($msb) {
    Write-Output "Using msbuild: $msb"
    Push-Location (Join-Path $root 'PvP_BlockChain')
    & $msb PvP_BlockChain.vcxproj /p:Configuration=Debug /m
    Pop-Location
} else {
    Write-Output "msbuild not found in PATH or known locations. To build the game, run this script in a 'Developer Command Prompt for VS' or install Visual Studio Build Tools. Skipping build step."
}

# 5) Run game executable (Debug path - adjust if needed)
# 5) Run game executable
# Prefer external exe path from run_config.json (so you can point to D: build)
$cfgPath = Join-Path $root 'run_config.json'
$launched = $false
if (Test-Path $cfgPath) {
    try {
        $cfg = Get-Content $cfgPath -Raw | ConvertFrom-Json
        $exe = $cfg.ExePath
        if ($exe -and (Test-Path $exe)) {
            Write-Output "Launching configured exe: $exe"
            Start-Process -FilePath $exe -WorkingDirectory (Split-Path $exe)
            $launched = $true
        } else {
            Write-Output "Configured ExePath not found or invalid: $exe"
        }
    } catch { Write-Output "Failed to read run_config.json: $_" }
}

if (-not $launched) {
    $exe = Join-Path $root 'PvP_BlockChain\Debug\PvP_BlockChain.exe'
    if (Test-Path $exe) {
        Write-Output "Launching local debug exe: $exe"
        Start-Process -FilePath $exe -WorkingDirectory (Split-Path $exe)
        $launched = $true
    } else {
        Write-Output "Executable not found at $exe. Check build output or run_config.json." 
    }
}

Write-Output "Orchestration finished. If the game is running, play a match; the bridge will mint when a player wins."
