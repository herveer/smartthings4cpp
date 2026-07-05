# tools/ — capability code generation

**Every** typed capability class in this library is **generated** from the
SmartThings capability schemas (`GET /v1/capabilities/{id}/{version}`) — there are
no hand-written capability classes:

- **Standard** capabilities (no vendor prefix, e.g. `battery`, `colorControl`)
  land in `smartthings4cpp::standard` under
  `include/smartthings4cpp/capabilities/standard/` and `src/capabilities/standard.cpp`.
- **Samsung-proprietary** capabilities (`samsungce.*`, `samsungvd.*`, `custom.*`,
  `sec.*`, `hca.*`, `samsungim.*`) land in a `smartthings4cpp::<prefix>` namespace
  under `include/smartthings4cpp/capabilities/<prefix>/` and `src/capabilities/<prefix>.cpp`.
  Their definitions live only behind the authenticated API and aren't in any public
  reference, which is why they must be fetched and generated.

`UnknownCapability` (in `capabilities/unknown_capability.h`) is the only
hand-maintained capability type — it is the null-object fallback for ids without a
generated class, not a schema-derived class.

The generated files are committed, so you do **not** need to run these scripts to
build the library. Re-run them only to refresh or extend coverage.

## Regenerating

Requires PowerShell 7+ and a SmartThings Personal Access Token with `r:devices:*`
(the capability-read is covered by device read).

```powershell
# 1. Download the schemas your devices actually use. Omit -Proprietary so the
#    standard (non-proprietary) capabilities are fetched too.
./tools/fetch_capability_schemas.ps1 -Token <pat> -OutDir ./schemas

# 2. Generate the typed C++ classes from those schemas.
./tools/generate_capabilities.ps1 -SchemaDir ./schemas
```

> Always regenerate from the **full** schema set (don't pass `-Proprietary` to the
> fetch step). The generator rewrites every generated file and both wiring regions
> from the schemas you give it, so a partial set would drop classes.

`generate_capabilities.ps1` overwrites:

- the per-capability generated headers under `capabilities/<group>/`,
- the per-group umbrella headers (`standard.h`, `samsungce.h`, …),
- the per-group implementation TUs (`standard.cpp`, `samsungce.cpp`, …), and
- the two factory TUs (`standard_factory.cpp`, `proprietary_factory.cpp`).

It also **auto-updates the wiring** — the generated-source list in
`src/CMakeLists.txt` and the generated-umbrella `#include`s in
`include/smartthings4cpp/capabilities.h`, each delimited by
`BEGIN/END GENERATED CAPABILITY …` marker comments. The script rewrites the region
between those markers, so the markers must stay in place; it never touches
`capability.h` or `capability.cpp`.

## What each generated class looks like

Per capability id, one class deriving from `Capability` in a
`smartthings4cpp::<group>` sub-namespace (`standard` for non-proprietary):

- each **attribute** → a reactive `ReadonlyProperty<T>` getter (string/int/double/bool,
  `std::vector<std::string>`, or `nlohmann::json` for objects/complex arrays),
  refreshed by `parseStatus()`;
- each **command** → a `Result<void>` method that POSTs via `executeCommand()`.

`createStandardCapability()` maps each standard id to its class and
`createProprietaryCapability()` maps each proprietary id to its class;
`createCapability()` consults those two factories and finally falls back to
`UnknownCapability`.
