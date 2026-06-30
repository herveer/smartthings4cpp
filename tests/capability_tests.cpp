#include <catch2/catch_test_macros.hpp>

#include <type_traits>

#include "smartthings4cpp/capability.h"
#include "smartthings4cpp/capabilities.h"
#include "smartthings4cpp/json_utils.h"

using namespace smartthings4cpp;
using nlohmann::json;

TEST_CASE("createCapability maps known ids to typed classes", "[capability]") {
	auto sw = createCapability("switch", 1, "main", "dev", nullptr);
	REQUIRE(dynamic_cast<Switch*>(sw.get()) != nullptr);

	auto level = createCapability("switchLevel", 1, "main", "dev", nullptr);
	REQUIRE(dynamic_cast<SwitchLevel*>(level.get()) != nullptr);

	auto temp = createCapability("temperatureMeasurement", 1, "main", "dev", nullptr);
	REQUIRE(dynamic_cast<TemperatureMeasurement*>(temp.get()) != nullptr);

	REQUIRE(sw->capabilityId() == "switch");
	REQUIRE(sw->componentId() == "main");
}

TEST_CASE("createCapability falls back to UnknownCapability", "[capability]") {
	auto cap = createCapability("vendor.totallyMadeUpCapability", 1, "main", "dev", nullptr);
	REQUIRE(cap != nullptr);
	REQUIRE(dynamic_cast<UnknownCapability*>(cap.get()) != nullptr);
	REQUIRE(cap->capabilityId() == "vendor.totallyMadeUpCapability");
}

TEST_CASE("createCapability resolves Samsung-proprietary ids to typed classes", "[capability]") {
	auto kidsLock = createCapability("samsungce.kidsLock", 1, "main", "dev", nullptr);
	REQUIRE(dynamic_cast<samsungce::KidsLock*>(kidsLock.get()) != nullptr);

	auto washerMode = createCapability("hca.washerMode", 1, "main", "dev", nullptr);
	REQUIRE(dynamic_cast<hca::WasherMode*>(washerMode.get()) != nullptr);

	// Proprietary typed capabilities still parse status into their getters.
	auto* lock = dynamic_cast<samsungce::KidsLock*>(kidsLock.get());
	lock->updateStatus(json_utils::parse(R"({"lockState":{"value":"locked"}})"));
	REQUIRE(lock->LockState.Get() == "locked");
}

TEST_CASE("Switch parses status into typed getters", "[capability]") {
	Switch sw(1, "main", "dev", nullptr);
	REQUIRE_FALSE(sw.IsOn.Get());

	sw.updateStatus(json_utils::parse(R"({"switch":{"value":"on"}})"));
	REQUIRE(sw.State.Get() == "on");
	REQUIRE(sw.IsOn.Get());

	sw.updateStatus(json_utils::parse(R"({"switch":{"value":"off"}})"));
	REQUIRE_FALSE(sw.IsOn.Get());
}

TEST_CASE("SwitchLevel parses level and levelRange", "[capability]") {
	SwitchLevel level(1, "main", "dev", nullptr);
	level.updateStatus(json_utils::parse(
		R"({"level":{"value":42,"unit":"%"},"levelRange":{"value":{"minimum":10,"maximum":90}}})"));
	REQUIRE(level.Level.Get() == 42);
	REQUIRE(level.MinLevel.Get() == 10);
	REQUIRE(level.MaxLevel.Get() == 90);
}

TEST_CASE("AudioVolume and AudioMute parse status", "[capability]") {
	AudioVolume volume(1, "main", "dev", nullptr);
	volume.updateStatus(json_utils::parse(R"({"volume":{"value":30,"unit":"%"}})"));
	REQUIRE(volume.Volume.Get() == 30);

	AudioMute mute(1, "main", "dev", nullptr);
	mute.updateStatus(json_utils::parse(R"({"mute":{"value":"muted"}})"));
	REQUIRE(mute.IsMuted.Get());
	REQUIRE(mute.MuteState.Get() == "muted");
}

TEST_CASE("Sensor-like capabilities parse numeric/string status", "[capability]") {
	ContactSensor contact(1, "cooler", "dev", nullptr);
	contact.updateStatus(json_utils::parse(R"({"contact":{"value":"open"}})"));
	REQUIRE(contact.IsOpen.Get());

	TemperatureMeasurement temp(1, "cooler", "dev", nullptr);
	temp.updateStatus(json_utils::parse(R"({"temperature":{"value":4.5,"unit":"C"}})"));
	REQUIRE(temp.Temperature.Get() == 4.5);
	REQUIRE(temp.Unit.Get() == "C");

	ThermostatCoolingSetpoint setpoint(1, "cooler", "dev", nullptr);
	setpoint.updateStatus(json_utils::parse(R"({"coolingSetpoint":{"value":7,"unit":"C"}})"));
	REQUIRE(setpoint.CoolingSetpoint.Get() == 7.0);
	REQUIRE(setpoint.Unit.Get() == "C");
}

TEST_CASE("PropertyChanged fires when status changes", "[capability]") {
	Switch sw(1, "main", "dev", nullptr);
	int changes = 0;
	auto sub = sw.PropertyChanged.SubscribeScoped(
		[&](ObservableObject&, PropertyChangedArgs) { ++changes; });

	sw.updateStatus(json_utils::parse(R"({"switch":{"value":"on"}})"));
	REQUIRE(changes > 0);
}

TEST_CASE("buildCommandsBody produces the SmartThings command shape", "[capability]") {
	auto body = buildCommandsBody("main", "switchLevel", "setLevel", json::array({ 50 }));

	REQUIRE(body.contains("commands"));
	REQUIRE(body["commands"].is_array());
	REQUIRE(body["commands"].size() == 1);

	const auto& entry = body["commands"][0];
	REQUIRE(entry["component"] == "main");
	REQUIRE(entry["capability"] == "switchLevel");
	REQUIRE(entry["command"] == "setLevel");
	REQUIRE(entry["arguments"] == json::array({ 50 }));
}

TEST_CASE("buildCommandsBody normalizes non-array arguments to an empty array", "[capability]") {
	auto body = buildCommandsBody("main", "switch", "on", json(nullptr));
	REQUIRE(body["commands"][0]["arguments"] == json::array());
}

TEST_CASE("createCapability maps every supported id to its typed class", "[capability]") {
	auto check = [](const char* id, auto* tag) {
		using T = std::remove_pointer_t<decltype(tag)>;
		auto cap = createCapability(id, 1, "main", "dev", nullptr);
		REQUIRE(dynamic_cast<T*>(cap.get()) != nullptr);
		REQUIRE(cap->capabilityId() == id);
	};
	check("audioVolume", static_cast<AudioVolume*>(nullptr));
	check("audioMute", static_cast<AudioMute*>(nullptr));
	check("contactSensor", static_cast<ContactSensor*>(nullptr));
	check("thermostatCoolingSetpoint", static_cast<ThermostatCoolingSetpoint*>(nullptr));
	check("refresh", static_cast<Refresh*>(nullptr));
	check("mediaPlayback", static_cast<MediaPlayback*>(nullptr));
	check("mediaTrackControl", static_cast<MediaTrackControl*>(nullptr));
	check("mediaInputSource", static_cast<MediaInputSource*>(nullptr));
	check("tvChannel", static_cast<TvChannel*>(nullptr));
	check("powerConsumptionReport", static_cast<PowerConsumptionReport*>(nullptr));
	check("remoteControlStatus", static_cast<RemoteControlStatus*>(nullptr));
	check("demandResponseLoadControl", static_cast<DemandResponseLoadControl*>(nullptr));
	check("washerOperatingState", static_cast<WasherOperatingState*>(nullptr));
	check("refrigeration", static_cast<Refrigeration*>(nullptr));
	check("execute", static_cast<Execute*>(nullptr));
	check("ocf", static_cast<Ocf*>(nullptr));
}

TEST_CASE("MediaPlayback parses status and lists supported commands", "[capability]") {
	MediaPlayback playback(1, "main", "dev", nullptr);
	playback.updateStatus(json_utils::parse(
		R"({"playbackStatus":{"value":"playing"},"supportedPlaybackCommands":{"value":["play","pause","stop"]}})"));
	REQUIRE(playback.PlaybackStatus.Get() == "playing");
	REQUIRE(playback.SupportedCommands.Get().size() == 3);
}

TEST_CASE("WasherOperatingState parses machine/job state", "[capability]") {
	WasherOperatingState washer(1, "main", "dev", nullptr);
	washer.updateStatus(json_utils::parse(
		R"({"machineState":{"value":"run"},"washerJobState":{"value":"wash"},"completionTime":{"value":"2026-06-30T12:00:00Z"}})"));
	REQUIRE(washer.MachineState.Get() == "run");
	REQUIRE(washer.JobState.Get() == "wash");
	REQUIRE(washer.CompletionTime.Get() == "2026-06-30T12:00:00Z");
}

TEST_CASE("Refrigeration exposes on/off modes as booleans", "[capability]") {
	Refrigeration fridge(1, "main", "dev", nullptr);
	fridge.updateStatus(json_utils::parse(
		R"({"rapidCooling":{"value":"on"},"rapidFreezing":{"value":"off"},"defrost":{"value":"off"}})"));
	REQUIRE(fridge.RapidCooling.Get());
	REQUIRE_FALSE(fridge.RapidFreezing.Get());
	REQUIRE_FALSE(fridge.Defrost.Get());
}

TEST_CASE("PowerConsumptionReport parses nested power/energy fields", "[capability]") {
	PowerConsumptionReport report(1, "main", "dev", nullptr);
	report.updateStatus(json_utils::parse(
		R"({"powerConsumption":{"value":{"power":120.5,"energy":3400,"deltaEnergy":15}}})"));
	REQUIRE(report.Power.Get() == 120.5);
	REQUIRE(report.Energy.Get() == 3400.0);
	REQUIRE(report.DeltaEnergy.Get() == 15.0);
}

TEST_CASE("RemoteControlStatus and Ocf parse string attributes", "[capability]") {
	RemoteControlStatus remote(1, "main", "dev", nullptr);
	remote.updateStatus(json_utils::parse(R"({"remoteControlEnabled":{"value":"true"}})"));
	REQUIRE(remote.Enabled.Get());

	Ocf ocf(1, "main", "dev", nullptr);
	ocf.updateStatus(json_utils::parse(R"({"n":{"value":"Fridge"},"mnmn":{"value":"Samsung"}})"));
	REQUIRE(ocf.DeviceName.Get() == "Fridge");
	REQUIRE(ocf.ManufacturerName.Get() == "Samsung");
	REQUIRE(ocf.field("n") == "Fridge");
}

TEST_CASE("DemandResponseLoadControl builds the requestDrlcAction arguments", "[capability]") {
	// reportingPeriod omitted -> four positional arguments.
	auto body = buildCommandsBody("main", "demandResponseLoadControl", "requestDrlcAction",
		json::array({ 1, 2, "2026-06-30T12:00:00Z", 30 }));
	REQUIRE(body["commands"][0]["arguments"].size() == 4);
	REQUIRE(body["commands"][0]["arguments"][0] == 1);
}
