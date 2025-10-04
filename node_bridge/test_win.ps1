<#
Simple test script to POST a win event to the bridge.
Usage:
  powershell -ExecutionPolicy Bypass -File .\test_win.ps1
Or pass addresses:
  powershell -ExecutionPolicy Bypass -File .\test_win.ps1 -winner 0x... -loser 0x...
#>
param(
    [string]$winner = '0x627306090abaB3A6e1400e9345bC60c78a8BEf57',
    [string]$loser  = '0xf17f52151EbEF6C7334FAD080c5704D77216b732'
)

$body = @{ winnerAddress = $winner; loserAddress = $loser } | ConvertTo-Json -Depth 5
Write-Output "Posting to http://localhost:3000/win with body:`n$body"
try {
    $resp = Invoke-RestMethod -Uri 'http://localhost:3000/win' -Method POST -Body $body -ContentType 'application/json' -ErrorAction Stop
    Write-Output "Response:`n$($resp | ConvertTo-Json -Depth 5)"
} catch {
    Write-Output "Request failed:`n$($_.Exception.Response | ForEach-Object { try { (Get-Content -Raw -Path $_.ResponseStream) } catch { $_ } })"
    Write-Output "Error object:`n$($_ | Out-String)"
}
