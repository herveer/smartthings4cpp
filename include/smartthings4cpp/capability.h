#pragma once

#include "types.h"
#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include <ReactiveLitepp/ObservableObject.h>

/**
 * @file capability.h
 * @brief Base class for all SmartThings capabilities
 */

namespace smartthings4cpp {
	using namespace ReactiveLitepp;

	// Forward declarations
	class Client;
	struct Component;

	/**
	 * @brief Abstract base class for a capability exposed by a device component
	 *
	 * In the SmartThings model a capability (e.g. "switch", "switchLevel",
	 * "temperatureMeasurement") describes the attributes (state) and commands
	 * (actions) a device supports. Each concrete capability is implemented as a
	 * class deriving from Capability that exposes typed getters (attributes) and
	 * setters/commands, following the SmartThings capabilities reference.
	 *
	 * Capabilities are reactive ObservableObjects: attribute getters read cached
	 * backing fields, and parseStatus() refreshes those fields and fires
	 * PropertyChanged whenever new status is received. Capability-specific code
	 * lives entirely in the derived classes; this base only provides the shared
	 * plumbing (identity, status caching, command execution).
	 *
	 * Instances are created by the createCapability() factory and owned by the
	 * Component they belong to. Capability is non-copyable and non-movable.
	 */
	class Capability : public ObservableObject {
	public:
		virtual ~Capability() = default;

		Capability(const Capability&) = delete;
		Capability& operator=(const Capability&) = delete;
		Capability(Capability&&) = delete;
		Capability& operator=(Capability&&) = delete;

		/** @brief Capability identifier, e.g. "switch" (reactive, read-only) */
		ReadonlyProperty<std::string> Id = MakeReadonlyProperty<&Capability::Id>([this]() { return _capabilityId; });

		/** @brief Capability version (reactive, read-only) */
		ReadonlyProperty<int> Version = MakeReadonlyProperty<&Capability::Version>([this]() { return _version; });

		/** @brief Identifier of the component this capability belongs to (reactive, read-only) */
		ReadonlyProperty<std::string> ComponentId = MakeReadonlyProperty<&Capability::ComponentId>([this]() { return _componentId; });

		/** @brief Plain accessor for the capability id (used for lookups) */
		const std::string& capabilityId() const { return _capabilityId; }

		/** @brief Plain accessor for the component id */
		const std::string& componentId() const { return _componentId; }

		/**
		 * @brief Get the owning Component
		 * @return Pointer to the Component that owns this capability, or nullptr
		 *         if not wired to one (e.g. a capability constructed directly in
		 *         a test rather than obtained through a Device)
		 */
		Component* component() const { return _component; }

		/**
		 * @brief Re-fetch this single capability's status from the cloud
		 *
		 * GET /v1/devices/{deviceId}/components/{componentId}/capabilities/{capabilityId}/status
		 * @return Result indicating success or failure
		 */
		Result<void> refreshStatus();

		/**
		 * @brief Feed this capability a status JSON blob and refresh typed state
		 *
		 * @param capStatus The per-capability status object, i.e.
		 *        @c {"<attr>":{"value":...,"unit":...,"timestamp":...}, ...}
		 * @note Used by Device::refreshStatus() and refreshStatus(). Caches the raw
		 *       status and calls parseStatus() so derived classes update their
		 *       backing fields and fire PropertyChanged.
		 */
		void updateStatus(const nlohmann::json& capStatus);

		/**
		 * @brief Apply a single attribute change delivered by a webhook event
		 *
		 * Merges just @p attr's new @p value into the cached status (leaving the
		 * capability's other attributes untouched) and re-parses, so typed
		 * getters and PropertyChanged reflect it immediately - the same reactive
		 * path a status refresh or an optimistic command update takes.
		 * @param attr Attribute name from the SmartThings deviceEvent (e.g. "switch")
		 * @param value The new attribute value
		 * @note Used by Device::applyDeviceEvent(); it is exactly setLocalAttribute()
		 *       exposed for event dispatch, hence public-but-internal like updateStatus().
		 */
		void applyAttributeEvent(const std::string& attr, const nlohmann::json& value) {
			setLocalAttribute(attr, value);
		}

		/** @brief Raw cached status object for this capability */
		const nlohmann::json& statusJson() const { return _status; }

		/**
		 * @brief Names of the commands this capability declares (for discovery/reflection)
		 *
		 * The base returns an empty list; each typed capability overrides this to
		 * report the SmartThings command names it supports. Attribute names and their
		 * values are available directly from statusJson() after a status refresh.
		 */
		virtual std::vector<std::string> commandNames() const { return {}; }

	protected:
		/**
		 * @brief Protected constructor - only derived classes (and the factory) construct
		 * @param capabilityId Capability identifier (e.g. "switch")
		 * @param version Capability version
		 * @param componentId Owning component id (e.g. "main")
		 * @param deviceId Owning device id
		 * @param client Owning Client (used for status/command API calls; may be nullptr in tests)
		 */
		Capability(std::string capabilityId, int version, std::string componentId,
			std::string deviceId, Client* client);

		/**
		 * @brief Read an attribute's cached value
		 * @param attr Attribute name
		 * @return The attribute's "value" node, or a null json if absent
		 */
		nlohmann::json attributeValue(const std::string& attr) const;

		/** @brief Convenience: read a string attribute value (or @p def if absent/not a string) */
		std::string attributeString(const std::string& attr, const std::string& def = "") const;

		/** @brief Convenience: read a numeric attribute value (or @p def if absent/not a number) */
		double attributeNumber(const std::string& attr, double def = 0.0) const;

		/** @brief Convenience: read a boolean attribute value (or @p def if absent/not a bool) */
		bool attributeBool(const std::string& attr, bool def = false) const;

		/** @brief Convenience: read an attribute's "unit" string (or @p def if absent) */
		std::string attributeUnit(const std::string& attr, const std::string& def = "") const;

		/** @brief Convenience: read a string-array attribute value (empty if absent/not an array) */
		std::vector<std::string> attributeStringArray(const std::string& attr) const;

		/**
		 * @brief Execute a command for this capability on its component
		 *
		 * POST /v1/devices/{deviceId}/commands with a single command targeting this
		 * capability/component.
		 * @param command Command name (e.g. "setLevel")
		 * @param args Command arguments as a JSON array (e.g. @c [50])
		 * @return Result indicating success or failure
		 */
		Result<void> executeCommand(const std::string& command,
			const nlohmann::json& args = nlohmann::json::array());

		/**
		 * @brief Optimistically set an attribute's cached value and re-parse
		 *
		 * Updates the cached status for @p attr then calls parseStatus() so typed
		 * getters and PropertyChanged reflect the new value immediately, without
		 * waiting for a status round-trip after a command.
		 */
		void setLocalAttribute(const std::string& attr, const nlohmann::json& value);

		/**
		 * @brief Update typed backing fields from a status object (derived override)
		 *
		 * Default implementation does nothing (used by UnknownCapability). Derived
		 * classes parse the attributes they care about and call
		 * SetPropertyValueAndNotify to fire PropertyChanged.
		 */
		virtual void parseStatus(const nlohmann::json& status);

		std::string _capabilityId;
		int _version;
		std::string _componentId;
		std::string _deviceId;
		Client* _client;
		nlohmann::json _status = nlohmann::json::object();
		Component* _component = nullptr;

		/// Sets the owning Component back-pointer. Called by
		/// Component::addCapability() once the capability has been placed at
		/// its final, stable address.
		void setComponent(Component* component) { _component = component; }

		friend struct Component;
	};

	/**
	 * @brief Build the request body for POST /v1/devices/{id}/commands
	 *
	 * Produces @c {"commands":[{"component":...,"capability":...,"command":...,"arguments":[...]}]}.
	 * Exposed (and pure) so it can be unit-tested without a network.
	 * @param component Component id
	 * @param capability Capability id
	 * @param command Command name
	 * @param args Command arguments as a JSON array
	 * @return The command request body
	 */
	nlohmann::json buildCommandsBody(const std::string& component,
		const std::string& capability,
		const std::string& command,
		const nlohmann::json& args);

	/**
	 * @brief Factory: create the concrete Capability for a capability id
	 *
	 * Returns a typed capability for supported ids, or an UnknownCapability for any
	 * id without a dedicated class yet (proprietary or not-yet-implemented).
	 * @param id Capability id
	 * @param version Capability version
	 * @param componentId Owning component id
	 * @param deviceId Owning device id
	 * @param client Owning Client
	 * @return Owning pointer to the created capability (never nullptr)
	 */
	std::unique_ptr<Capability> createCapability(const std::string& id, int version,
		const std::string& componentId, const std::string& deviceId, Client* client);

	/**
	 * @brief Factory for the generated standard capabilities (non-proprietary)
	 *
	 * Returns a typed capability for standard ids (no vendor prefix) that don't have
	 * a curated hand-written class registered directly in createCapability(), or
	 * nullptr for any other id. Implemented in the generated
	 * src/capabilities/standard_factory.cpp and consulted by createCapability()
	 * after the hand-written classes and before createProprietaryCapability().
	 */
	std::unique_ptr<Capability> createStandardCapability(const std::string& id, int version,
		const std::string& componentId, const std::string& deviceId, Client* client);

	/**
	 * @brief Factory for the Samsung-proprietary capabilities (generated)
	 *
	 * Returns a typed capability for proprietary ids (@c samsungce.*, @c samsungvd.*,
	 * @c custom.*, @c sec.*, @c hca.*, @c samsungim.*), or nullptr for any other id.
	 * Implemented in the generated src/capabilities/proprietary_factory.cpp and
	 * consulted by createCapability() before falling back to UnknownCapability.
	 */
	std::unique_ptr<Capability> createProprietaryCapability(const std::string& id, int version,
		const std::string& componentId, const std::string& deviceId, Client* client);

} // namespace smartthings4cpp
