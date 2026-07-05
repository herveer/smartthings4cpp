<#
.SYNOPSIS
  Generate typed C++ capability classes from SmartThings capability JSON schemas.

.DESCRIPTION
  Reads every <id>.v<ver>.json schema in -SchemaDir (as produced by
  fetch_capability_schemas.ps1) and emits a typed Capability subclass per id
  (reactive attribute getters + command methods), grouped by "namespace group":

    * standard capabilities (ids with no vendor prefix, e.g. "battery",
      "colorControl") go under include/smartthings4cpp/capabilities/standard/,
      the smartthings4cpp::standard namespace, src/capabilities/standard.cpp,
      and are registered in createStandardCapability() (standard_factory.cpp);

    * Samsung-proprietary capabilities (samsungce.*, samsungvd.*, custom.*,
      sec.*, hca.*, samsungim.*) go under capabilities/<prefix>/, the
      smartthings4cpp::<prefix> namespace, src/capabilities/<prefix>.cpp, and are
      registered in createProprietaryCapability() (proprietary_factory.cpp).

  In addition to the per-capability files, the script auto-maintains the wiring:
    * the generated-source list in src/CMakeLists.txt, and
    * the generated-umbrella #includes in include/smartthings4cpp/capabilities.h,
  both delimited by BEGIN/END GENERATED marker comments (which must already be
  present - the script rewrites the region between them but will not invent it).

  Every typed capability is generated from a schema - there are no hand-written
  capability classes. Re-run this whenever you fetch new/updated schemas. It
  overwrites the generated files and rewrites the two marker regions, but never
  edits capability.h or capability.cpp (which just declare/call the two factories).

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
$capabilitiesHeader = Join-Path $repo 'include/smartthings4cpp/capabilities.h'
$srcCMake = Join-Path $repo 'src/CMakeLists.txt'

$keywords = @('alignas','alignof','and','asm','auto','bool','break','case','catch','char','class','const','consteval','constexpr','continue','default','delete','do','double','else','enum','explicit','export','extern','false','float','for','friend','goto','if','inline','int','long','mutable','namespace','new','noexcept','not','nullptr','operator','or','private','protected','public','register','return','short','signed','sizeof','static','struct','switch','template','this','throw','true','try','typedef','typename','union','unsigned','using','virtual','void','volatile','while','xor')
$reservedMethods = @('refreshStatus','updateStatus','statusJson','capabilityId','componentId','executeCommand','attributeValue','attributeString','attributeNumber','attributeUnit','attributeStringArray','attributeBool','setLocalAttribute','parseStatus')

# Proprietary vendor prefixes (each becomes its own namespace/folder/TU).
$proprietaryPrefixes = 'samsungce','samsungvd','custom','sec','hca','samsungim'
# Synthetic group name for the no-prefix (standard) capabilities.
$standardGroup = 'standard'

# Markers delimiting the auto-maintained regions in the two wiring files.
$cmakeBegin = '    # === BEGIN GENERATED CAPABILITY SOURCES (managed by tools/generate_capabilities.ps1) ==='
$cmakeEnd   = '    # === END GENERATED CAPABILITY SOURCES ==='
$incBegin   = '// === BEGIN GENERATED CAPABILITY INCLUDES (managed by tools/generate_capabilities.ps1) ==='
$incEnd     = '// === END GENERATED CAPABILITY INCLUDES ==='

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

# Rewrite the text between $begin and $end markers in $path with $lines,
# preserving the file's existing newline style. The markers must already exist.
function Update-Region([string]$path, [string]$begin, [string]$end, [string[]]$lines) {
    $txt = [IO.File]::ReadAllText($path)
    $nl = if ($txt.Contains("`r`n")) { "`r`n" } else { "`n" }
    $rx = [regex]::new([regex]::Escape($begin) + '.*?' + [regex]::Escape($end),
        [System.Text.RegularExpressions.RegexOptions]::Singleline)
    if (-not $rx.IsMatch($txt)) {
        throw "Marker region not found in $path.`nExpected these lines to be present:`n$begin`n$end"
    }
    $mid = if ($lines.Count -gt 0) { $nl + ($lines -join $nl) } else { '' }
    $replacement = $begin + $mid + $nl + $end
    $new = $rx.Replace($txt, { param($m) $replacement }.GetNewClosure(), 1)
    [IO.File]::WriteAllText($path, $new)
}

# All groups, standard first, then the proprietary prefixes in a stable order.
$allGroups = @($standardGroup) + $proprietaryPrefixes
$byGroup = @{}
foreach ($g in $allGroups) { $byGroup[$g] = @{ headers = @(); impls = @(); classes = @() } }

foreach ($file in (Get-ChildItem $SchemaDir -Filter *.json | Sort-Object Name)) {
    $j = Get-Content $file.FullName -Raw | ConvertFrom-Json
    $id = $j.id
    $dot = $id.IndexOf('.')
    if ($dot -lt 0) {
        # Standard capability (no vendor prefix).
        $group = $standardGroup
        $rest = $id
    } else {
        $prefix = $id.Substring(0, $dot)
        if ($proprietaryPrefixes -notcontains $prefix) { continue }  # -> UnknownCapability
        $group = $prefix
        $rest = $id.Substring($dot + 1)
    }
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
        # Backing field: derive from the sanitized, de-duplicated property name so
        # attribute ids containing non-identifier characters (e.g. "DeviceWatch-Enroll")
        # still yield a valid, unique C++ member name.
        $fld = '_' + $prop.Substring(0,1).ToLower() + $prop.Substring(1)
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
        $propDecls += "`t`tReactiveLitepp::ReadonlyProperty<$cpp> $prop = MakeReadonlyProperty<&$class`::$prop>([this]() { return $fld; });"
    }

    $cmdDecls = @(); $cmdDefs = @(); $cmdNames = @()
    $cmds = if ($j.commands) { @($j.commands.PSObject.Properties) } else { @() }
    foreach ($c in $cmds) {
        $cn = $c.Name; $method = $cn
        $cmdNames += '"' + $cn + '"'
        if ($method -in $reservedMethods) { $method = $method + 'Command' }
        if ($method -in $keywords) { $method = $method + '_' }
        # Case-sensitive: a member function named exactly like its class is ill-formed
        # (injected-class-name), but a differently-cased name like execute()/Execute is
        # fine. Using -eq (case-insensitive) here renamed execute -> executeCommand, which
        # then hid the base Capability::executeCommand and made the body recurse.
        if ($method -ceq $class) { $method = $method + 'Command' }
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

    $note = if ($group -eq $standardGroup) { 'generated' } else { 'Samsung-proprietary; generated' }
    $h = @('#pragma once','','#include "../../capability.h"','','namespace smartthings4cpp {',"namespace $group {",'',
           "`t/** @brief The @c $id capability ($note). */","`tclass $class : public Capability {","`tpublic:",
           "`t`tstatic constexpr const char* CAPABILITY_ID = `"$id`";",'',
           "`t`t$class(int version, std::string componentId, std::string deviceId, Client* client);")
    if ($propDecls.Count -gt 0) { $h += ''; $h += $propDecls }
    if ($cmdDecls.Count -gt 0) { $h += ''; $h += $cmdDecls }
    if ($cmdNames.Count -gt 0) { $h += "`t`tstd::vector<std::string> commandNames() const override { return { $($cmdNames -join ', ') }; }" }
    if ($parseLines.Count -gt 0) { $h += ''; $h += "`tprotected:"; $h += "`t`tvoid parseStatus(const nlohmann::json& status) override;" }
    if ($fieldDecls.Count -gt 0) { $h += ''; $h += "`tprivate:"; foreach ($fd in $fieldDecls) { $h += "`t`t$fd" } }
    $h += "`t};"; $h += ''; $h += "} // namespace $group"; $h += '} // namespace smartthings4cpp'
    $headerRel = "$group/$snake.h"
    $headerPath = Join-Path $incRoot $headerRel
    New-Item -ItemType Directory -Force -Path (Split-Path $headerPath) | Out-Null
    [IO.File]::WriteAllText($headerPath, (($h -join "`r`n") + "`r`n"))

    $impl = @("`t$class`::$class(int version, std::string componentId, std::string deviceId, Client* client)",
              "`t`t: Capability(`"$id`", version, std::move(componentId), std::move(deviceId), client) {","`t}")
    if ($parseLines.Count -gt 0) { $impl += ''; $impl += "`tvoid $class`::parseStatus(const nlohmann::json&) {"; $impl += $parseLines; $impl += "`t}" }
    foreach ($d in $cmdDefs) { $impl += ''; $impl += $d }

    $byGroup[$group].headers += $headerRel
    $byGroup[$group].impls += ($impl -join "`r`n")
    $byGroup[$group].classes += @{ Class = $class; Id = $id }
}

# Emit per-group umbrella header + implementation TU, and collect factory entries
# and wiring lines. The two factory TUs are always emitted (even when empty) so
# createStandardCapability()/createProprietaryCapability() are always defined.
$standardFactoryLines = @()
$proprietaryFactoryLines = @()
$cmakeSourceLines = @()
$includeLines = @()

foreach ($g in $allGroups) {
    $info = $byGroup[$g]
    if ($info.classes.Count -eq 0) { continue }

    $ucomment = if ($g -eq $standardGroup) {
        "/** @file $standardGroup.h @brief All standard (non-proprietary) capabilities (generated). */"
    } else {
        "/** @file $g.h @brief All $g.* proprietary capabilities (generated). */"
    }
    $u = @('#pragma once','',$ucomment,'')
    foreach ($hr in ($info.headers | Sort-Object)) { $u += "#include `"$hr`"" }
    [IO.File]::WriteAllText((Join-Path $incRoot "$g.h"), (($u -join "`r`n") + "`r`n"))

    $c = @("#include `"smartthings4cpp/capabilities/$g.h`"",'','#include <utility>','','namespace smartthings4cpp {',"namespace $g {",'')
    $c += ($info.impls -join "`r`n`r`n"); $c += ''; $c += "} // namespace $g"; $c += '} // namespace smartthings4cpp'
    [IO.File]::WriteAllText((Join-Path $srcRoot "$g.cpp"), (($c -join "`r`n") + "`r`n"))

    $includeLines += "#include `"capabilities/$g.h`""
    $cmakeSourceLines += "    capabilities/$g.cpp"

    foreach ($cl in $info.classes) {
        $line = "`tif (id == $g`::$($cl.Class)::CAPABILITY_ID) return std::make_unique<$g`::$($cl.Class)>(version, componentId, deviceId, client);"
        if ($g -eq $standardGroup) { $standardFactoryLines += $line } else { $proprietaryFactoryLines += $line }
    }
}

# Both factory TUs are always compiled; list them after the group TUs.
$cmakeSourceLines += '    capabilities/standard_factory.cpp'
$cmakeSourceLines += '    capabilities/proprietary_factory.cpp'

function Write-Factory([string]$fnName, [string]$fileName, [string[]]$lines) {
    # When the factory has no entries its parameters are unused; emit an unnamed
    # signature so it stays warning-clean under /W4 without a churny (void) dance.
    if ($lines.Count -gt 0) {
        $sig = @("`tstd::unique_ptr<Capability> $fnName(const std::string& id, int version,",
                 "`t`tconst std::string& componentId, const std::string& deviceId, Client* client) {")
    } else {
        $sig = @("`tstd::unique_ptr<Capability> $fnName(const std::string&, int,",
                 "`t`tconst std::string&, const std::string&, Client*) {")
    }
    $f = @('#include "smartthings4cpp/capability.h"','#include "smartthings4cpp/capabilities.h"','','namespace smartthings4cpp {','')
    $f += $sig
    $f += $lines; $f += "`treturn nullptr;"; $f += "`t}"; $f += ''; $f += '} // namespace smartthings4cpp'
    [IO.File]::WriteAllText((Join-Path $srcRoot $fileName), (($f -join "`r`n") + "`r`n"))
}
Write-Factory 'createStandardCapability' 'standard_factory.cpp' $standardFactoryLines
Write-Factory 'createProprietaryCapability' 'proprietary_factory.cpp' $proprietaryFactoryLines

# Auto-maintain the wiring regions.
Update-Region $srcCMake $cmakeBegin $cmakeEnd $cmakeSourceLines
Update-Region $capabilitiesHeader $incBegin $incEnd $includeLines

$stdCount = $byGroup[$standardGroup].classes.Count
$propCount = ($allGroups | Where-Object { $_ -ne $standardGroup } | ForEach-Object { $byGroup[$_].classes.Count } | Measure-Object -Sum).Sum
Write-Host "Generated $stdCount standard and $propCount proprietary capability class(es)."
Write-Host "Updated wiring in src/CMakeLists.txt and include/smartthings4cpp/capabilities.h."
