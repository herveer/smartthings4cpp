#pragma once

#include "types.h"
#include "component.h"
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
	 * exposes. Each component owns the capability objects (switch, switchLevel,
	 * temperatureMeasurement, ...) the device supports.
	 *
	 * Device derives from ReactiveLitepp::ObservableObject and surfaces its
	 * metadata through ReadonlyProperty members. It is non-copyable and
	 * non-movable; obtain instances through Client::getDevices() or
	 * Client::getDevice().
	 *
	 * After discovery, capability attribute values are empty until refreshStatus()
	 * is called (which fetches GET /v1/devices/{id}/status and distributes it to
	 * each capability).
	 */
	class Device : public ObservableObject {
	public:
		Device();
		Device(const std::string& id, Client* client);
		~Device() = default;

		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(Device&&) = delete;

		/** @brief The device's unique identifier (read-only) */
		ReadonlyProperty<std::string> Id{ [this]() { return _id; } };
		/** @brief The user-given label of the device, falling back to its name (read-only) */
		ReadonlyProperty<std::string> Label{ [this]() { return _label; } };
		/** @brief The device type name reported by the integration (read-only) */
		ReadonlyProperty<std::string> Name{ [this]() { return _name; } };
		/** @brief Name of the device manufacturer (read-only) */
		ReadonlyProperty<std::string> ManufacturerName{ [this]() { return _manufacturerName; } };
		/** @brief Integration type, e.g. "ZIGBEE", "ZWAVE", "OCF", "VIRTUAL" (read-only) */
		ReadonlyProperty<std::string> Type{ [this]() { return _type; } };
		/** @brief Identifier of the location the device belongs to (read-only) */
		ReadonlyProperty<std::string> LocationId{ [this]() { return _locationId; } };
		/** @brief Identifier of the room the device is assigned to (read-only) */
		ReadonlyProperty<std::string> RoomId{ [this]() { return _roomId; } };
		/** @brief Presentation identifier used by the SmartThings UI (read-only) */
		ReadonlyProperty<std::string> PresentationId{ [this]() { return _presentationId; } };

		/**
		 * @brief Get the components owned by this device
		 * @return Const reference to the device's component list (declared order)
		 */
		const std::vector<Component>& getComponents() const;

		/**
		 * @brief Get a component by id
		 * @param id Component id (default "main")
		 * @return Pointer to the component, or nullptr if absent (owned by this device)
		 */
		Component* getComponent(const std::string& id = "main");

		/**
		 * @brief Get a capability by id on a component (base pointer)
		 * @param capabilityId Capability id (e.g. "switch")
		 * @param component Component id (default "main")
		 * @return Pointer to the capability, or nullptr if absent
		 */
		Capability* getCapability(const std::string& capabilityId,
			const std::string& component = "main");

		/**
		 * @brief Get a capability by its concrete type on a component
		 *
		 * Example:
		 * @code
		 * if (auto* sw = device->getCapability<Switch>()) sw->on();
		 * @endcode
		 * @tparam T A concrete Capability subclass exposing @c CAPABILITY_ID
		 * @param component Component id (default "main")
		 * @return Typed pointer to the capability, or nullptr if absent
		 */
		template<typename T>
		T* getCapability(const std::string& component = "main") {
			Component* c = getComponent(component);
			return c ? c->template get<T>() : nullptr;
		}

		/**
		 * @brief Whether the device exposes a capability on any component
		 * @param capabilityId Capability id to look for (e.g. "switch")
		 */
		bool hasCapability(const std::string& capabilityId) const;

		/**
		 * @brief Whether the device exposes a capability on any component
		 * @tparam T A concrete Capability subclass exposing @c CAPABILITY_ID
		 */
		template<typename T>
		bool hasCapability() const {
			return hasCapability(T::CAPABILITY_ID);
		}

		/**
		 * @brief Get the flattened, de-duplicated list of capability ids across all components
		 * @return Vector of unique capability ids in first-seen order
		 */
		std::vector<std::string> getCapabilityIds() const;

		/**
		 * @brief Fetch the device status and distribute it to each capability
		 *
		 * GET /v1/devices/{id}/status, then updateStatus() each capability so typed
		 * attribute getters return live values.
		 * @return Result indicating success or failure
		 */
		Result<void> refreshStatus();

		/**
		 * @brief Initialize device metadata and build capability objects from JSON
		 * @param json JSON object describing a device (a GET /v1/devices item or a
		 *             single device resource)
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

		Client* _client = nullptr;

		friend class Client;
	};

} // namespace smartthings4cpp
