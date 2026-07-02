#include "smartthings4cpp/device.h"
#include "smartthings4cpp/client.h"
#include "smartthings4cpp/json_utils.h"

#include <unordered_set>

namespace smartthings4cpp {

	Device::Device() = default;

	Device::Device(const std::string& id, Client* client)
		: _id(id), _client(client) {
	}

	const std::vector<Component>& Device::getComponents() const {
		ensureRefreshed();
		return _components;
	}

	Component* Device::getComponent(const std::string& id) {
		ensureRefreshed();
		for (auto& component : _components) {
			if (component.id == id) {
				return &component;
			}
		}
		return nullptr;
	}

	Capability* Device::getCapability(const std::string& capabilityId, const std::string& component) {
		Component* c = getComponent(component);
		return c ? c->get(capabilityId) : nullptr;
	}

	bool Device::hasCapability(const std::string& capabilityId) const {
		for (const auto& component : _components) {
			if (component.has(capabilityId)) {
				return true;
			}
		}
		return false;
	}

	std::vector<std::string> Device::getCapabilityIds() const {
		std::vector<std::string> ids;
		std::unordered_set<std::string> seen;

		for (const auto& component : _components) {
			for (const auto& capability : component.capabilities) {
				if (capability && seen.insert(capability->capabilityId()).second) {
					ids.push_back(capability->capabilityId());
				}
			}
		}

		return ids;
	}

	Result<void> Device::refreshStatus() const {
		if (!_client) {
			return Result<void>(ErrorCode::InvalidParameter, "No client associated with device");
		}
		return applyStatus(_client->getDeviceStatus(_id));
	}

	Result<void> Device::applyStatus(const nlohmann::json& status) const {
		if (status.is_null() || !status.is_object() || !status.contains("components")
			|| !status["components"].is_object()) {
			return Result<void>(ErrorCode::NetworkError, "Failed to fetch device status");
		}

		const auto& components = status["components"];
		for (auto& component : _components) {
			if (!components.contains(component.id) || !components[component.id].is_object()) {
				continue;
			}
			const auto& componentStatus = components[component.id];
			for (auto& capability : component.capabilities) {
				if (capability && componentStatus.contains(capability->capabilityId())) {
					capability->updateStatus(componentStatus[capability->capabilityId()]);
				}
			}
		}

		_hasBeenRefreshed = true;
		return Result<void>();
	}

	void Device::ensureRefreshed() const {
		if (!_hasBeenRefreshed) {
			refreshStatus();
		}
	}

	void Device::initFromJson(const nlohmann::json& json) {
		std::string id = json_utils::getValueOr<std::string>(json, "deviceId", _id);
		if (!id.empty()) {
			_id = id;
		}

		_name = json_utils::getValueOr<std::string>(json, "name", "");
		_label = json_utils::getValueOr<std::string>(json, "label", "");
		// The SmartThings app shows the label; fall back to the device name when
		// no label has been assigned so callers always have something readable.
		if (_label.empty()) {
			_label = _name;
		}

		_manufacturerName = json_utils::getValueOr<std::string>(json, "manufacturerName", "");
		_type = json_utils::getValueOr<std::string>(json, "type", "");
		_locationId = json_utils::getValueOr<std::string>(json, "locationId", "");
		_roomId = json_utils::getValueOr<std::string>(json, "roomId", "");
		_presentationId = json_utils::getValueOr<std::string>(json, "presentationId", "");

		// Components -> capability objects (built via the factory)
		_components.clear();
		_componentSubscriptions.clear();
		if (json.contains("components") && json["components"].is_array()) {
			// Reserved upfront so the push_back() below never reallocates: once a
			// Component is wired (addCapability() called on it), its address must
			// never change again, since each capability holds a raw Component*
			// back-pointer to it and this Device subscribes directly to its
			// PropertyChanged (see Component's move-constructor note in component.h).
			_components.reserve(json["components"].size());

			for (const auto& component_json : json["components"]) {
				Component component;
				component.id = json_utils::getValueOr<std::string>(component_json, "id", "");
				component.label = json_utils::getValueOr<std::string>(component_json, "label", "");

				if (component_json.contains("categories")
					&& component_json["categories"].is_array()) {
					for (const auto& category_json : component_json["categories"]) {
						Category category;
						category.name = json_utils::getValueOr<std::string>(category_json, "name", "");
						category.categoryType = json_utils::getValueOr<std::string>(category_json, "categoryType", "");
						if (!category.name.empty()) {
							component.categories.push_back(std::move(category));
						}
					}
				}

				// Move the still-empty (no capabilities, nothing wired) Component
				// into its final vector slot *before* adding capabilities to it,
				// so every capability's Component back-pointer ends up pointing at
				// a stable address that will never change again (reserve() above
				// guarantees no later push_back() in this loop can reallocate).
				_components.push_back(std::move(component));
				Component& stored = _components.back();
				stored.device = this;

				if (component_json.contains("capabilities")
					&& component_json["capabilities"].is_array()) {
					for (const auto& capability_json : component_json["capabilities"]) {
						std::string capabilityId =
							json_utils::getValueOr<std::string>(capability_json, "id", "");
						int version = json_utils::getValueOr<int>(capability_json, "version", 1);
						if (!capabilityId.empty()) {
							stored.addCapability(
								createCapability(capabilityId, version, stored.id, _id, _client));
						}
					}
				}

				// Relay this component's (aggregate) PropertyChanged up through
				// this device's own (inherited) PropertyChanged, so subscribing
				// once at the Device level hears about every capability change.
				_componentSubscriptions.push_back(stored.PropertyChanged.SubscribeScoped(
					[this](ObservableObject& obj, PropertyChangedArgs args) {
						PropertyChanged.Notify(obj, args);
					}));
			}
		}
	}

} // namespace smartthings4cpp
