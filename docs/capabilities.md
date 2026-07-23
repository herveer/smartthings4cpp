[📚 Documentation](README.md) › **Capability reference**

# Capability reference

Every typed capability class in smartthings4cpp — **143 classes**, all
**generated from the live SmartThings capability schemas**
(`GET /v1/capabilities/{id}/{version}`). There are no hand-written capability
classes, so coverage is uniform: every attribute becomes a reactive typed
getter, every command becomes a method.

- [Anatomy of a capability class](#anatomy-of-a-capability-class)
- [Namespaces](#namespaces)
- [Standard capabilities](#standard-capabilities-smartthings4cppstandard)
- [Samsung proprietary: `samsungce`](#samsungce-smartthings4cppsamsungce)
- [Samsung proprietary: `custom`](#custom-smartthings4cppcustom)
- [Samsung proprietary: `samsungvd`](#samsungvd-smartthings4cppsamsungvd)
- [Samsung proprietary: `sec`, `hca`, `samsungim`](#sec-hca-samsungim)
- [The fallback: UnknownCapability](#the-fallback-unknowncapability)
- [Regenerating](#regenerating)

---

## Anatomy of a capability class

Each class derives from `Capability` and follows the same shape — here is the
real generated `switch`:

```cpp
namespace smartthings4cpp::standard {

class Switch : public Capability {
public:
    static constexpr const char* CAPABILITY_ID = "switch";

    // attribute -> reactive typed getter
    ReadonlyProperty<std::string> SwitchValue;   // "on" / "off"

    // commands -> Result<void> methods
    Result<void> on();
    Result<void> off();

    std::vector<std::string> commandNames() const override; // {"off","on"}
};

} // namespace smartthings4cpp::standard
```

Mapping rules:

| Schema element | Generated as |
|---|---|
| Attribute (string/number/boolean) | `ReadonlyProperty<std::string / int / double / bool>` |
| Attribute (string array) | `ReadonlyProperty<std::vector<std::string>>` |
| Attribute (object / complex array) | `ReadonlyProperty<nlohmann::json>` |
| Command | `Result<void> name(args…)` posting via the commands API |

Getters read cached state and update reactively — see
[Reading state](device-model.md#reading-state). `CAPABILITY_ID` is what
`getCapability<T>()` matches on.

## Namespaces

| Namespace | Prefix | Classes | Example |
|---|---|---|---|
| `smartthings4cpp::standard` | *(none)* | 36 | `standard::Switch` |
| `smartthings4cpp::samsungce` | `samsungce.` | 57 | `samsungce::WasherOperatingState` |
| `smartthings4cpp::custom` | `custom.` | 28 | `custom::DryerDryLevel` |
| `smartthings4cpp::samsungvd` | `samsungvd.` | 16 | `samsungvd::Ambient` |
| `smartthings4cpp::sec` | `sec.` | 3 | `sec::DiagnosticsInformation` |
| `smartthings4cpp::samsungim` | `samsungim.` | 2 | `samsungim::HueSyncMode` |
| `smartthings4cpp::hca` | `hca.` | 1 | `hca::WasherMode` |

`#include <smartthings4cpp/smartthings4cpp.h>` (or
`<smartthings4cpp/capabilities.h>`) brings in all of them; per-group umbrellas
(`capabilities/standard.h`, `capabilities/samsungce.h`, …) and per-class
headers exist for finer includes.

## Standard capabilities (`smartthings4cpp::standard`)

| Capability id | Class |
|---|---|
| `audioMute` | `AudioMute` |
| `audioNotification` | `AudioNotification` |
| `audioTrackData` | `AudioTrackData` |
| `audioVolume` | `AudioVolume` |
| `battery` | `Battery` |
| `bridge` | `Bridge` |
| `colorControl` | `ColorControl` |
| `colorTemperature` | `ColorTemperature` |
| `contactSensor` | `ContactSensor` |
| `demandResponseLoadControl` | `DemandResponseLoadControl` |
| `dishwasherOperatingState` | `DishwasherOperatingState` |
| `execute` | `Execute` |
| `healthCheck` | `HealthCheck` |
| `illuminanceMeasurement` | `IlluminanceMeasurement` |
| `mediaInputSource` | `MediaInputSource` |
| `mediaPlayback` | `MediaPlayback` |
| `mediaPresets` | `MediaPresets` |
| `mediaTrackControl` | `MediaTrackControl` |
| `motionSensor` | `MotionSensor` |
| `movementSensor` | `MovementSensor` |
| `multipleZonePresence` | `MultipleZonePresence` |
| `ocf` | `Ocf` |
| `powerConsumptionReport` | `PowerConsumptionReport` |
| `presenceSensor` | `PresenceSensor` |
| `refresh` | `Refresh` |
| `refrigeration` | `Refrigeration` |
| `relativeBrightness` | `RelativeBrightness` |
| `remoteControlStatus` | `RemoteControlStatus` |
| `soundDetection` | `SoundDetection` |
| `switch` | `Switch` |
| `switchLevel` | `SwitchLevel` |
| `temperatureMeasurement` | `TemperatureMeasurement` |
| `thermostatCoolingSetpoint` | `ThermostatCoolingSetpoint` |
| `tvChannel` | `TvChannel` |
| `washerOperatingState` | `WasherOperatingState` |
| `windowShadeLevel` | `WindowShadeLevel` |

## `samsungce` (`smartthings4cpp::samsungce`)

Samsung Consumer Electronics — washers, dishwashers, fridges, and shared
appliance plumbing.

| Capability id | Class |
|---|---|
| `samsungce.accessibility` | `Accessibility` |
| `samsungce.audioVolumeLevel` | `AudioVolumeLevel` |
| `samsungce.autoDispenseDetergent` | `AutoDispenseDetergent` |
| `samsungce.autoDispenseSoftener` | `AutoDispenseSoftener` |
| `samsungce.autoOpenDoor` | `AutoOpenDoor` |
| `samsungce.connectionState` | `ConnectionState` |
| `samsungce.detergentAutoReplenishment` | `DetergentAutoReplenishment` |
| `samsungce.detergentOrder` | `DetergentOrder` |
| `samsungce.detergentState` | `DetergentState` |
| `samsungce.deviceApplicationsAndSettings` | `DeviceApplicationsAndSettings` |
| `samsungce.deviceIdentification` | `DeviceIdentification` |
| `samsungce.dishwasherJobState` | `DishwasherJobState` |
| `samsungce.dishwasherOperation` | `DishwasherOperation` |
| `samsungce.dishwasherWashingCourse` | `DishwasherWashingCourse` |
| `samsungce.dishwasherWashingCourseDetails` | `DishwasherWashingCourseDetails` |
| `samsungce.dishwasherWashingOptions` | `DishwasherWashingOptions` |
| `samsungce.dongleSoftwareInstallation` | `DongleSoftwareInstallation` |
| `samsungce.driverState` | `DriverState` |
| `samsungce.driverVersion` | `DriverVersion` |
| `samsungce.energyPlanner` | `EnergyPlanner` |
| `samsungce.foodDefrost` | `FoodDefrost` |
| `samsungce.freezerConvertMode` | `FreezerConvertMode` |
| `samsungce.fridgeIcemakerInfo` | `FridgeIcemakerInfo` |
| `samsungce.fridgePantryInfo` | `FridgePantryInfo` |
| `samsungce.fridgePantryMode` | `FridgePantryMode` |
| `samsungce.fridgeVacationMode` | `FridgeVacationMode` |
| `samsungce.fridgeWelcomeLighting` | `FridgeWelcomeLighting` |
| `samsungce.fridgeZoneInfo` | `FridgeZoneInfo` |
| `samsungce.kidsLock` | `KidsLock` |
| `samsungce.meatAging` | `MeatAging` |
| `samsungce.operationOrigin` | `OperationOrigin` |
| `samsungce.powerCool` | `PowerCool` |
| `samsungce.powerFreeze` | `PowerFreeze` |
| `samsungce.quickControl` | `QuickControl` |
| `samsungce.sabbathMode` | `SabbathMode` |
| `samsungce.selfCheck` | `SelfCheck` |
| `samsungce.softenerAutoReplenishment` | `SoftenerAutoReplenishment` |
| `samsungce.softenerOrder` | `SoftenerOrder` |
| `samsungce.softenerState` | `SoftenerState` |
| `samsungce.softwareUpdate` | `SoftwareUpdate` |
| `samsungce.softwareVersion` | `SoftwareVersion` |
| `samsungce.temperatureSetting` | `TemperatureSetting` |
| `samsungce.unavailableCapabilities` | `UnavailableCapabilities` |
| `samsungce.viewInside` | `ViewInside` |
| `samsungce.washerBubbleSoak` | `WasherBubbleSoak` |
| `samsungce.washerCycle` | `WasherCycle` |
| `samsungce.washerCyclePreset` | `WasherCyclePreset` |
| `samsungce.washerDelayEnd` | `WasherDelayEnd` |
| `samsungce.washerFreezePrevent` | `WasherFreezePrevent` |
| `samsungce.washerOperatingState` | `WasherOperatingState` |
| `samsungce.washerWashingTime` | `WasherWashingTime` |
| `samsungce.washerWaterLevel` | `WasherWaterLevel` |
| `samsungce.washerWaterValve` | `WasherWaterValve` |
| `samsungce.waterConsumptionReport` | `WaterConsumptionReport` |
| `samsungce.weightMeasurement` | `WeightMeasurement` |
| `samsungce.weightMeasurementCalibration` | `WeightMeasurementCalibration` |
| `samsungce.welcomeMessage` | `WelcomeMessage` |

## `custom` (`smartthings4cpp::custom`)

Samsung's `custom.*` capabilities — appliance options and TV settings.

| Capability id | Class |
|---|---|
| `custom.accessibility` | `Accessibility` |
| `custom.deodorFilter` | `DeodorFilter` |
| `custom.deviceReportStateConfiguration` | `DeviceReportStateConfiguration` |
| `custom.disabledCapabilities` | `DisabledCapabilities` |
| `custom.disabledComponents` | `DisabledComponents` |
| `custom.dishwasherDelayStartTime` | `DishwasherDelayStartTime` |
| `custom.dishwasherOperatingPercentage` | `DishwasherOperatingPercentage` |
| `custom.dishwasherOperatingProgress` | `DishwasherOperatingProgress` |
| `custom.dryerDryLevel` | `DryerDryLevel` |
| `custom.dustFilter` | `DustFilter` |
| `custom.energyType` | `EnergyType` |
| `custom.error` | `Error` |
| `custom.fridgeMode` | `FridgeMode` |
| `custom.jobBeginningStatus` | `JobBeginningStatus` |
| `custom.launchapp` | `Launchapp` |
| `custom.picturemode` | `Picturemode` |
| `custom.recording` | `Recording` |
| `custom.soundmode` | `Soundmode` |
| `custom.supportedOptions` | `SupportedOptions` |
| `custom.thermostatSetpointControl` | `ThermostatSetpointControl` |
| `custom.tvsearch` | `Tvsearch` |
| `custom.washerAutoDetergent` | `WasherAutoDetergent` |
| `custom.washerAutoSoftener` | `WasherAutoSoftener` |
| `custom.washerRinseCycles` | `WasherRinseCycles` |
| `custom.washerSoilLevel` | `WasherSoilLevel` |
| `custom.washerSpinLevel` | `WasherSpinLevel` |
| `custom.washerWaterTemperature` | `WasherWaterTemperature` |
| `custom.waterFilter` | `WaterFilter` |

## `samsungvd` (`smartthings4cpp::samsungvd`)

Samsung Visual Display — TVs and monitors.

| Capability id | Class |
|---|---|
| `samsungvd.ambient` | `Ambient` |
| `samsungvd.ambientContent` | `AmbientContent` |
| `samsungvd.art` | `Art` |
| `samsungvd.audioGroupInfo` | `AudioGroupInfo` |
| `samsungvd.audioInputSource` | `AudioInputSource` |
| `samsungvd.deviceCategory` | `DeviceCategory` |
| `samsungvd.firmwareVersion` | `FirmwareVersion` |
| `samsungvd.launchService` | `LaunchService` |
| `samsungvd.lightControl` | `LightControl` |
| `samsungvd.mediaInputSource` | `MediaInputSource` |
| `samsungvd.remoteControl` | `RemoteControl` |
| `samsungvd.soundDetection` | `SoundDetection` |
| `samsungvd.soundFrom` | `SoundFrom` |
| `samsungvd.supportsFeatures` | `SupportsFeatures` |
| `samsungvd.supportsPowerOnByOcf` | `SupportsPowerOnByOcf` |
| `samsungvd.thingStatus` | `ThingStatus` |

## `sec`, `hca`, `samsungim`

| Capability id | Class |
|---|---|
| `sec.diagnosticsInformation` | `sec::DiagnosticsInformation` |
| `sec.smartthingsHub` | `sec::SmartthingsHub` |
| `sec.wifiConfiguration` | `sec::WifiConfiguration` |
| `hca.washerMode` | `hca::WasherMode` |
| `samsungim.fixedFindNode` | `samsungim::FixedFindNode` |
| `samsungim.hueSyncMode` | `samsungim::HueSyncMode` |

## The fallback: UnknownCapability

Any id without a class above materializes as `UnknownCapability` — the one
hand-maintained capability type. It carries the id/version, exposes the raw
cached status via `statusJson()`, and participates fully in the reactive
relay; it just has no typed getters or command methods. See
[Unknown capabilities](device-model.md#unknown-capabilities).

## Regenerating

The generator lives in [`tools/`](../tools/README.md). Generated files are
committed, so you never need to run it to build — run it to **extend coverage**
to capabilities your devices expose that aren't listed above (Samsung's
proprietary schemas only exist behind the authenticated API, which is why they
must be fetched):

```powershell
# 1. Download the schemas your devices actually use (full set - see tools/README.md)
./tools/fetch_capability_schemas.ps1 -Token <pat> -OutDir ./tools/schemas

# 2. Regenerate the typed classes, factories, and build wiring
./tools/generate_capabilities.ps1 -SchemaDir ./tools/schemas
```

The generator rewrites the per-capability headers, the umbrella headers, the
factory translation units, and auto-updates the wiring regions in
`src/CMakeLists.txt` and `capabilities.h`. Details and caveats:
[`tools/README.md`](../tools/README.md).

---

<div align="center">

[← Extending the library](extending.md) · **Next:** [Examples tour →](examples.md)

</div>
