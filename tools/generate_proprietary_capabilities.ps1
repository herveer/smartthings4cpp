<#
.SYNOPSIS
  Generate typed C++ capability classes from SmartThings capability JSON schemas.

.DESCRIPTION
  Reads every <id>.v<ver>.json schema in -SchemaDir (as produced by
  fetch_capability_schemas.ps1) whose id begins with one of the proprietary prefixes,
  and emits, under include/smartthings4cpp/capabilities/<prefix>/ and
  src/capabilities/, a typed Capability subclass per id (reactive attribute getters +
  command methods), a per-prefix umbrella header, a per-prefix implementation TU, and
  proprietary_factory.cpp (the createProprietaryCapability() factory).

  Re-run this whenever you fetch new/updated schemas. It overwrites generated files
  but does not touch capability.h/capabilities.h/CMakeLists.txt — if you add a brand
  new prefix you must register its umbrella include + source there (the six current
  prefixes are already wired).

.PARAMETER SchemaDir
  Directory containing the *.json capability schemas.
#>
param(
    [Parameter(Mandatory = $true)][string]$SchemaDir
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$incRoot = Join-Path $repo 'include/smartthings4cpp/capabilities'
$srcRoot = Join-Path $repo 'src/capabilities'

$keywords = @('alignas','alignof','and','asm','auto','bool','break','case','catch','char','class','const','consteval','constexpr','continue','default','delete','do','double','else','enum','explicit','export','extern','false','float','for','friend','goto','if','inline','int','long','mutable','namespace','new','noexcept','not','nullptr','operator','or','private','protected','public','register','return','short','signed','sizeof','static','struct','switch','template','this','throw','true','try','typedef','typename','union','unsigned','using','virtual','void','volatile','while','xor')
$reservedMethods = @('refreshStatus','updateStatus','statusJson','capabilityId','componentId','executeCommand','attributeValue','attributeString','attributeNumber','attributeUnit','attributeStringArray','attributeBool','setLocalAttribute','parseStatus')

function Pascal([string]$s) {
    $parts = $s -split '[^A-Za-z0-9]+' | Where-Object { $_ -ne '' }
    ($parts | ForEach-Object { $_.Substring(0,1).ToUpper() + $_.Substring(1) }) -join ''
}
function Snake([string]$s) {
    $t = [regex]::Replace($s, '([a-z0-9])([A-Z])', '$1_$2')
    $t = [regex]::Replace($t, '[^A-Za-z0-9]+', '_')
    $t.ToLower()
}
function AttrType($schema) {
    $v = $schema.properties.value
    if ($null -eq $v) { return @{ kind = 'json' } }
    switch ($v.type) {
        'string'  { return @{ kind = 'string' } }
        'integer' { return @{ kind = 'integer' } }
        'number'  { return @{ kind = 'number' } }
        'boolean' { return @{ kind = 'boolean' } }
        'array'   { if ($v.items.type -eq 'string') { return @{ kind = 'strarray' } } else { return @{ kind = 'json' } } }
        default   { return @{ kind = 'json' } }
    }
}
function ArgCppType($schema) {
    switch ($schema.type) {
        'string'  { return 'const std::string& ' }
        'integer' { return 'int ' }
        'number'  { return 'double ' }
        'boolean' { return 'bool ' }
        default   { return 'const nlohmann::json& ' }
    }
}

$prefixes = 'samsungce','samsungvd','custom','sec','hca','samsungim'
$byPrefix = @{}
foreach ($p in $prefixes) { $byPrefix[$p] = @{ headers = @(); impls = @(); classes = @() } }

foreach ($file in (Get-ChildItem $SchemaDir -Filter *.json | Sort-Object Name)) {
    $j = Get-Content $file.FullName -Raw | ConvertFrom-Json
    $id = $j.id
    $dot = $id.IndexOf('.')
    if ($dot -lt 0) { continue }
    $prefix = $id.Substring(0, $dot)
    if (-not $byPrefix.ContainsKey($prefix)) { continue }
    $rest = $id.Substring($dot + 1)
    $class = Pascal $rest
    if ($class -match '^[0-9]') { $class = '_' + $class }
    $snake = Snake $rest

    $usedProps = @{}; $propDecls = @(); $fieldDecls = @(); $parseLines = @()
    $attrs = if ($j.attributes) { @($j.attributes.PSObject.Properties) } else { @() }
    foreach ($a in $attrs) {
        $an = $a.Name
        $info = AttrType $a.Value.schema
        $prop = Pascal $an
        if ($prop -in 'Id','Version','ComponentId','Result','Capability','Client','Component','Category','Location','Room','ObservableObject','Property') { $prop = $prop + 'Attr' }
        if ($prop -eq $class) { $prop = $prop + 'Value' }
        while ($usedProps.ContainsKey($prop)) { $prop = $prop + '_' }
        $usedProps[$prop] = $true
        $fld = '_' + $an.Substring(0,1).ToLower() + $an.Substring(1)
        switch ($info.kind) {
            'string'   { $cpp = 'std::string'; $fieldDecls += "$cpp $fld;";
                         $parseLines += "`t`tSetPropertyValueAndNotify<&$class`::$prop>($fld, attributeString(`"$an`", $fld));" }
            'integer'  { $cpp = 'int'; $fieldDecls += "$cpp $fld = 0;";
                         $parseLines += "`t`tSetPropertyValueAndNotify<&$class`::$prop>($fld, static_cast<int>(attributeNumber(`"$an`", $fld)));" }
            'number'   { $cpp = 'double'; $fieldDecls += "$cpp $fld = 0.0;";
                         $parseLines += "`t`tSetPropertyValueAndNotify<&$class`::$prop>($fld, attributeNumber(`"$an`", $fld));" }
            'boolean'  { $cpp = 'bool'; $fieldDecls += "$cpp $fld = false;";
                         $parseLines += "`t`tSetPropertyValueAndNotify<&$class`::$prop>($fld, attributeBool(`"$an`", $fld));" }
            'strarray' { $cpp = 'std::vector<std::string>'; $fieldDecls += "$cpp $fld;";
                         $parseLines += "`t`t{ auto v = attributeStringArray(`"$an`"); if (!v.empty()) SetPropertyValueAndNotify<&$class`::$prop>($fld, v); }" }
            default    { $cpp = 'nlohmann::json'; $fieldDecls += "$cpp $fld;";
                         $parseLines += "`t`t{ auto v = attributeValue(`"$an`"); if (!v.is_null()) SetPropertyValueAndNotify<&$class`::$prop>($fld, v); }" }
        }
        $propDecls += "`t`tReactiveLitepp::ReadonlyProperty<$cpp> $prop{ [this]() { return $fld; } };"
    }

    $cmdDecls = @(); $cmdDefs = @(); $cmdNames = @()
    $cmds = if ($j.commands) { @($j.commands.PSObject.Properties) } else { @() }
    foreach ($c in $cmds) {
        $cn = $c.Name; $method = $cn
        $cmdNames += '"' + $cn + '"'
        if ($method -in $reservedMethods) { $method = $method + 'Command' }
        if ($method -in $keywords) { $method = $method + '_' }
        if ($method -eq $class) { $method = $method + 'Command' }
        $params = @(); $pushes = @()
        foreach ($arg in $c.Value.arguments) {
            $pn = $arg.name
            if ($pn -in $keywords) { $pn = $pn + '_' }
            $params += (ArgCppType $arg.schema) + $pn
            $pushes += "`t`tcommandArgs.push_back($pn);"
        }
        $sig = ($params -join ', ')
        $cmdDecls += "`t`tResult<void> $method($sig);"
        if ($pushes.Count -gt 0) {
            $body = "`t`tnlohmann::json commandArgs = nlohmann::json::array();`r`n" + (($pushes) -join "`r`n") + "`r`n`t`treturn executeCommand(`"$cn`", commandArgs);"
        } else {
            $body = "`t`treturn executeCommand(`"$cn`");"
        }
        $cmdDefs += "`tResult<void> $class`::$method($sig) {`r`n$body`r`n`t}"
    }

    $h = @('#pragma once','','#include "../../capability.h"','','namespace smartthings4cpp {',"namespace $prefix {",'',
           "`t/** @brief The @c $id capability (Samsung-proprietary; generated). */","`tclass $class : public Capability {","`tpublic:",
           "`t`tstatic constexpr const char* CAPABILITY_ID = `"$id`";",'',
           "`t`t$class(int version, std::string componentId, std::string deviceId, Client* client);")
    if ($propDecls.Count -gt 0) { $h += ''; $h += $propDecls }
    if ($cmdDecls.Count -gt 0) { $h += ''; $h += $cmdDecls }
    if ($cmdNames.Count -gt 0) { $h += "`t`tstd::vector<std::string> commandNames() const override { return { $($cmdNames -join ', ') }; }" }
    if ($parseLines.Count -gt 0) { $h += ''; $h += "`tprotected:"; $h += "`t`tvoid parseStatus(const nlohmann::json& status) override;" }
    if ($fieldDecls.Count -gt 0) { $h += ''; $h += "`tprivate:"; foreach ($fd in $fieldDecls) { $h += "`t`t$fd" } }
    $h += "`t};"; $h += ''; $h += "} // namespace $prefix"; $h += '} // namespace smartthings4cpp'
    $headerRel = "$prefix/$snake.h"
    $headerPath = Join-Path $incRoot $headerRel
    New-Item -ItemType Directory -Force -Path (Split-Path $headerPath) | Out-Null
    [IO.File]::WriteAllText($headerPath, (($h -join "`r`n") + "`r`n"))

    $impl = @("`t$class`::$class(int version, std::string componentId, std::string deviceId, Client* client)",
              "`t`t: Capability(`"$id`", version, std::move(componentId), std::move(deviceId), client) {","`t}")
    if ($parseLines.Count -gt 0) { $impl += ''; $impl += "`tvoid $class`::parseStatus(const nlohmann::json&) {"; $impl += $parseLines; $impl += "`t}" }
    foreach ($d in $cmdDefs) { $impl += ''; $impl += $d }

    $byPrefix[$prefix].headers += $headerRel
    $byPrefix[$prefix].impls += ($impl -join "`r`n")
    $byPrefix[$prefix].classes += @{ Class = $class; Id = $id }
}

$factoryLines = @()
foreach ($p in $prefixes) {
    $info = $byPrefix[$p]
    if ($info.classes.Count -eq 0) { continue }
    $u = @('#pragma once','',"/** @file $p.h @brief All $p.* proprietary capabilities (generated). */",'')
    foreach ($hr in ($info.headers | Sort-Object)) { $u += "#include `"$hr`"" }
    [IO.File]::WriteAllText((Join-Path $incRoot "$p.h"), (($u -join "`r`n") + "`r`n"))

    $c = @("#include `"smartthings4cpp/capabilities/$p.h`"",'','#include <utility>','','namespace smartthings4cpp {',"namespace $p {",'')
    $c += ($info.impls -join "`r`n`r`n"); $c += ''; $c += "} // namespace $p"; $c += '} // namespace smartthings4cpp'
    [IO.File]::WriteAllText((Join-Path $srcRoot "$p.cpp"), (($c -join "`r`n") + "`r`n"))

    foreach ($cl in $info.classes) {
        $factoryLines += "`tif (id == $p`::$($cl.Class)::CAPABILITY_ID) return std::make_unique<$p`::$($cl.Class)>(version, componentId, deviceId, client);"
    }
}

$f = @('#include "smartthings4cpp/capability.h"','#include "smartthings4cpp/capabilities.h"','','namespace smartthings4cpp {','',
       "`tstd::unique_ptr<Capability> createProprietaryCapability(const std::string& id, int version,",
       "`t`tconst std::string& componentId, const std::string& deviceId, Client* client) {")
$f += $factoryLines; $f += "`treturn nullptr;"; $f += "`t}"; $f += ''; $f += '} // namespace smartthings4cpp'
[IO.File]::WriteAllText((Join-Path $srcRoot 'proprietary_factory.cpp'), (($f -join "`r`n") + "`r`n"))

Write-Host "Generated $($factoryLines.Count) proprietary capability class(es)."
