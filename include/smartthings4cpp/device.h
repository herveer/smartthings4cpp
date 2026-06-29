#pragma once

#include "types.h"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <ReactiveLitepp/ObservableObject.h>

/**
 * @file device.h
 * @brief SmartThings device representation with its components and capabilities
 */

namespace smartthings4cpp {
	using namespace ReactiveLitepp;

	// Forward declarations
	class Client;

	/**
	 * @brief Represents a single SmartThings device
	 *
	 * A device aggregates the product metadata reported by the SmartThings cloud
	 * (label, manufacturer, type, location, ...) together with the components it
	 * exposes. Each component carries the list of capabilities (e.g. "switch",
	 * "switchLevel", "colorControl") the device supports.
	 *
	 * Device, like the reactive objects in the sibling hue4cpp library, derives
	 * from ReactiveLitepp::ObservableObject and surfaces its metadata through
	 * ReadonlyProperty members, so subscribers can react to changes. It is
	 * therefore non-copyable and non-movable; obtain instances through
	 * Client::getDevices() or Client::getDevice().
	 *
	 * @note This iteration exposes discovery/metadata only. Reading live attribute
	 *       values and sending commands is handled by the device-control iteration;
	 *       the @c Client back-pointer is kept for that purpose.
	 */
	class Device : public ObservableObject {
	public:
		/**
		 * @brief Default constructor
		 */
		Device();

		/**
		 * @brief Construct a Device with ID and parent client
		 * @param id Device unique identifier (deviceId)
		 * @param client Pointer to the owning Client
		 */
		Device(const std::string& id, Client* client);

		/**
		 * @brief Destructor
		 */
		~Device() = default;

		// Prevent copying and moving (ObservableObject holds non-copyable events)
		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(Device&&) = delete;

		/**
		 * @brief The device's unique identifier (read-only)
		 */
		ReadonlyProperty<std::string> Id{
			[this]() { return _id; }
		};

		/**
		 * @brief The user-given label of the device (read-only)
		 *
		 * This is the friendly name shown in the SmartThings app. Falls back to
		 * the device @c name when no label is set.
		 */
		ReadonlyProperty<std::string> Label{
			[this]() { return _label; }
		};

		/**
		 * @brief The device type name reported by the integration (read-only)
		 */
		ReadonlyProperty<std::string> Name{
			[this]() { return _name; }
		};

		/**
		 * @brief Name of the device manufacturer (read-only)
		 */
		ReadonlyProperty<std::string> ManufacturerName{
			[this]() { return _manufacturerName; }
		};

		/**
		 * @brief Integration type of the device, e.g. "ZIGBEE", "ZWAVE", "LAN",
		 *        "OCF", "VIRTUAL" (read-only)
		 */
		ReadonlyProperty<std::string> Type{
			[this]() { return _type; }
		};

		/**
		 * @brief Identifier of the location the device belongs to (read-only)
		 */
		ReadonlyProperty<std::string> LocationId{
			[this]() { return _locationId; }
		};

		/**
		 * @brief Identifier of the room the device is assigned to (read-only)
		 */
		ReadonlyProperty<std::string> RoomId{
			[this]() { return _roomId; }
		};

		/**
		 * @brief Presentation identifier used by the SmartThings UI (read-only)
		 */
		ReadonlyProperty<std::string> PresentationId{
			[this]() { return _presentationId; }
		};

		/**
		 * @brief Get the components owned by this device
		 * @return Const reference to the device's component list (declared order)
		 */
		const std::vector<Component>& getComponents() const;

		/**
		 * @brief Get the flattened, de-duplicated list of capability ids across all components
		 * @return Vector of unique capability ids in first-seen order
		 */
		std::vector<std::string> getCapabilityIds() const;

		/**
		 * @brief Check whether the device exposes a capability on any component
		 * @param capability_id The capability id to look for (e.g. "switch")
		 * @return true if any component declares the capability, false otherwise
		 */
		bool hasCapability(const std::string& capability_id) const;

		/**
		 * @brief Initialize device metadata from JSON data
		 * @param json JSON object describing a device (an entry of the "items" array
		 *             returned by GET /v1/devices, or a single device resource)
		 * @note Internal method used by Client to populate device data.
		 */
		void initFromJson(const nlohmann::json& json);

	private:
		std::string _id;
		std::string _label;
		std::string _name;
		std::string _manufacturerName;
		std::string _type;
		std::string _locationId;
		std::string _roomId;
		std::string _presentationId;

		std::vector<Component> _components;

		Client* _client = nullptr; ///< Reserved for status/commands (device-control iteration)

		friend class Client;
	};

} // namespace smartthings4cpp
