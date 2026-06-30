# tools/ — capability code generation

The Samsung-proprietary capabilities (`samsungce.*`, `samsungvd.*`, `custom.*`,
`sec.*`, `hca.*`, `samsungim.*`) are **generated** from the SmartThings capability
schemas, because their definitions live only behind the authenticated API
(`GET /v1/capabilities/{id}/{version}`) and aren't in any public reference.

The generated classes under `include/smartthings4cpp/capabilities/<prefix>/` and
`src/capabilities/<prefix>.cpp` are committed, so you do **not** need to run these
scripts to build the library. Re-run them only to refresh or extend coverage.

## Regenerating

Requires PowerShell 7+ and a SmartThings Personal Access Token with `r:devices:*`
(the capability-read is covered by device read).

```powershell
# 1. Download the schemas your devices actually use (proprietary prefixes only).
./tools/fetch_capability_schemas.ps1 -Token <pat> -OutDir ./schemas -Proprietary

# 2. Generate the typed C++ classes from those schemas.
./tools/generate_proprietary_capabilities.ps1 -SchemaDir ./schemas
```

`generate_proprietary_capabilities.ps1` overwrites the generated headers, the six
per-prefix umbrella headers, the six per-prefix implementation TUs, and
`src/capabilities/proprietary_factory.cpp`. It does **not** edit `capability.h`,
`capabilities.h`, or `src/CMakeLists.txt`; the six current prefixes are already wired
there. If you introduce a brand-new prefix, add its umbrella `#include` and `.cpp`
source to those three files by hand (follow the existing pattern).

## What each generated class looks like

Per capability id, one class deriving from `Capability` in a `smartthings4cpp::<prefix>`
sub-namespace:

- each **attribute** → a reactive `ReadonlyProperty<T>` getter (string/int/double/bool,
  `std::vector<std::string>`, or `nlohmann::json` for objects/complex arrays),
  refreshed by `parseStatus()`;
- each **command** → a `Result<void>` method that POSTs via `executeCommand()`.

The factory `createProprietaryCapability()` maps each id to its class; ids without a
generated class fall back to `UnknownCapability`.
