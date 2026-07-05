<#
.SYNOPSIS
  Fetch SmartThings capability schemas for every capability used by your devices.

.DESCRIPTION
  Enumerates all devices on the account, collects the distinct (capabilityId, version)
  pairs (optionally filtered to the Samsung-proprietary prefixes), and downloads each
  capability definition from GET /v1/capabilities/{id}/{version} into -OutDir.

  These JSON files are the input to generate_capabilities.ps1.

.PARAMETER Token
  A SmartThings Personal Access Token. If omitted, set $env:SMARTTHINGS_TOKEN.

.PARAMETER OutDir
  Directory to write <id>.v<ver>.json files into (created if needed).

.PARAMETER Proprietary
  Only fetch the proprietary prefixes (samsungce./samsungvd./custom./sec./hca./samsungim.).
  Omit to fetch every capability the devices use.

.EXAMPLE
  ./fetch_capability_schemas.ps1 -Token <pat> -OutDir ./schemas -Proprietary
#>
param(
    [string]$Token = $env:SMARTTHINGS_TOKEN,
    [Parameter(Mandatory = $true)][string]$OutDir,
    [switch]$Proprietary
)
$ErrorActionPreference = 'Stop'
if (-not $Token) { throw "Provide -Token or set SMARTTHINGS_TOKEN." }
$headers = @{ Authorization = "Bearer $Token"; Accept = 'application/json' }
$base = 'https://api.smartthings.com/v1'
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

# Enumerate all devices (paginated).
$devices = @(); $url = "$base/devices"
while ($url) {
    $resp = Invoke-RestMethod -Uri $url -Headers $headers -Method Get
    if ($resp.items) { $devices += $resp.items }
    $url = if ($resp._links.next.href) { $resp._links.next.href } else { $null }
}
Write-Host "Devices: $($devices.Count)"

$proprietaryRegex = '^(samsungce|samsungvd|custom|sec|hca|samsungim)\.'
$map = @{}
foreach ($d in $devices) {
    foreach ($c in $d.components) {
        foreach ($cap in $c.capabilities) {
            if ($Proprietary -and ($cap.id -notmatch $proprietaryRegex)) { continue }
            $ver = if ($cap.version) { $cap.version } else { 1 }
            $map["$($cap.id)|$ver"] = $true
        }
    }
}
Write-Host "Distinct capabilities: $($map.Count)"

$ok = 0; $fail = 0
foreach ($pair in ($map.Keys | Sort-Object)) {
    $id, $ver = $pair.Split('|')
    try {
        $raw = Invoke-WebRequest -Uri "$base/capabilities/$id/$ver" -Headers $headers -UseBasicParsing
        $raw.Content | Set-Content -Path (Join-Path $OutDir "$id.v$ver.json") -Encoding utf8
        $ok++
    } catch { Write-Warning "FAIL $id v$ver : $($_.Exception.Message)"; $fail++ }
}
Write-Host "Fetched $ok schema(s), $fail failure(s) into $OutDir"
