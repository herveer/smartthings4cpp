#include <catch2/catch_test_macros.hpp>

#include "smartthings4cpp/device.h"
#include "smartthings4cpp/client.h"
#include "smartthings4cpp/capabilities.h"
#include "smartthings4cpp/json_utils.h"

using namespace smartthings4cpp;

namespace {

// A representative entry of the GET /v1/devices "items" array: a Zigbee dimmer
// exposing switch / switchLevel / refresh on its "main" component.
const char* kDeviceJson = R"({
  "deviceId": "11111111-2222-3333-4444-555555555555",
  "name": "zigbee-dimmer",
  "label": "Living Room Lamp",
  "manufacturerName": "SmartThings",
  "presentationId": "PRES-123",
  "locationId": "loc-abc",
  "roomId": "room-xyz",
  "type": "ZIGBEE",
  "components": [
    {
      "id": "main",
      "label": "main",
      "capabilities": [
        { "id": "switch", "version": 1 },
        { "id": "switchLevel", "version": 1 },
        { "id": "refresh", "version": 1 }
      ],
      "categories": [ { "name": "Light", "categoryType": "manufacturer" } ]
    }
  ]
})";

} // namespace

TEST_CASE("Device::initFromJson populates metadata properties", "[device]") {
	Device device;
	device.initFromJson(json_utils::parse(kDeviceJson));

	REQUIRE(device.Id.Get() == "11111111-2222-3333-4444-555555555555");
	REQUIRE(device.Label.Get() == "Living Room Lamp");
	REQUIRE(device.Name.Get() == "zigbee-dimmer");
	REQUIRE(device.ManufacturerName.Get() == "SmartThings");
	REQUIRE(device.Type.Get() == "ZIGBEE");
	REQUIRE(device.LocationId.Get() == "loc-abc");
	REQUIRE(device.RoomId.Get() == "room-xyz");
	REQUIRE(device.PresentationId.Get() == "PRES-123");
}

TEST_CASE("Device::initFromJson builds components and capability objects", "[device]") {
	Device device;
	device.initFromJson(json_utils::parse(kDeviceJson));

	const auto& components = device.getComponents();
	REQUIRE(components.size() == 1);
	REQUIRE(components[0].id == "main");
	REQUIRE(components[0].capabilities.size() == 3);
	REQUIRE(components[0].capabilities[0]->capabilityId() == "switch");
	REQUIRE(components[0].capabilities[0]->Version.Get() == 1);

	REQUIRE(components[0].categories.size() == 1);
	REQUIRE(components[0].categories[0].name == "Light");
	REQUIRE(components[0].categories[0].categoryType == "manufacturer");
}

TEST_CASE("Device capability lookup helpers work", "[device]") {
	Device device;
	device.initFromJson(json_utils::parse(kDeviceJson));

	auto ids = device.getCapabilityIds();
	REQUIRE(ids.size() == 3);
	REQUIRE(ids[0] == "switch");

	REQUIRE(device.hasCapability("switch"));
	REQUIRE(device.hasCapability("switchLevel"));
	REQUIRE_FALSE(device.hasCapability("colorControl"));

	// By id
	REQUIRE(device.getComponent("main") != nullptr);
	REQUIRE(device.getCapability("switch") != nullptr);
	REQUIRE(device.getCapability("colorControl") == nullptr);

	// By type
	auto* sw = device.getCapability<Switch>();
	REQUIRE(sw != nullptr);
	REQUIRE(sw->capabilityId() == "switch");
	REQUIRE(device.getCapability<SwitchLevel>() != nullptr);
	REQUIRE(device.getCapability<AudioVolume>() == nullptr);
}

TEST_CASE("Device label falls back to name when label is absent", "[device]") {
	const char* noLabel = R"({
	  "deviceId": "abc",
	  "name": "sensor-1",
	  "components": []
	})";

	Device device;
	device.initFromJson(json_utils::parse(noLabel));

	REQUIRE(device.Name.Get() == "sensor-1");
	REQUIRE(device.Label.Get() == "sensor-1");
	REQUIRE(device.getComponents().empty());
	REQUIRE(device.getCapabilityIds().empty());
}

TEST_CASE("Device::getCapabilityIds de-duplicates across components", "[device]") {
	const char* multiComponent = R"({
	  "deviceId": "multi",
	  "label": "Multi",
	  "components": [
	    { "id": "main", "capabilities": [ { "id": "switch", "version": 1 } ] },
	    { "id": "outlet1", "capabilities": [ { "id": "switch", "version": 1 } ] }
	  ]
	})";

	Device device;
	device.initFromJson(json_utils::parse(multiComponent));

	REQUIRE(device.getComponents().size() == 2);
	auto ids = device.getCapabilityIds();
	REQUIRE(ids.size() == 1);
	REQUIRE(ids[0] == "switch");

	// Same-id capability resolves per component.
	REQUIRE(device.getCapability<Switch>("main") != nullptr);
	REQUIRE(device.getCapability<Switch>("outlet1") != nullptr);
}

TEST_CASE("Device starts unrefreshed and stays that way without a usable client", "[device]") {
	Device device;
	device.initFromJson(json_utils::parse(kDeviceJson));

	REQUIRE_FALSE(device.HasBeenRefreshed.Get());

	// No client attached: getComponent()/getCapability() attempt (and fail) the
	// auto-refresh, but must not crash and must not fake success.
	REQUIRE(device.getComponent("main") != nullptr);
	REQUIRE(device.getCapability<Switch>() != nullptr);
	REQUIRE_FALSE(device.HasBeenRefreshed.Get());
}

TEST_CASE("Device with an unauthenticated client still fails the auto-refresh without crashing", "[device]") {
	Client client; // no access token -> Client fails fast, no network call
	Device device("11111111-2222-3333-4444-555555555555", &client);
	device.initFromJson(json_utils::parse(kDeviceJson));

	REQUIRE_FALSE(device.HasBeenRefreshed.Get());
	REQUIRE(device.getCapability<Switch>() != nullptr);
	REQUIRE_FALSE(device.HasBeenRefreshed.Get());
}
