#include "smartthings4cpp/device.h"
#include "smartthings4cpp/json_utils.h"

#include <unordered_set>

namespace smartthings4cpp {

	Device::Device() = default;

	Device::Device(const std::string& id, Client* client)
		: _id(id), _client(client) {
	}

	const std::vector<Component>& Device::getComponents() const {
		return _components;
	}

	std::vector<std::string> Device::getCapabilityIds() const {
		std::vector<std::string> ids;
		std::unordered_set<std::string> seen;

		for (const auto& component : _components) {
			for (const auto& capability : component.capabilities) {
				if (seen.insert(capability.id).second) {
					ids.push_back(capability.id);
				}
			}
		}

		return ids;
	}

	bool Device::hasCapability(const std::string& capability_id) const {
		for (const auto& component : _components) {
			for (const auto& capability : component.capabilities) {
				if (capability.id == capability_id) {
					return true;
				}
			}
		}
		return false;
	}

	void Device::initFromJson(const nlohmann::json& json) {
		// Core metadata. deviceId may already have been set via the constructor;
		// prefer the value reported in the payload when present.
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

		// Components -> capabilities / categories
		_components.clear();
		if (json.contains("components") && json["components"].is_array()) {
			for (const auto& component_json : json["components"]) {
				Component component;
				component.id = json_utils::getValueOr<std::string>(component_json, "id", "");
				component.label = json_utils::getValueOr<std::string>(component_json, "label", "");

				if (component_json.contains("capabilities")
					&& component_json["capabilities"].is_array()) {
					for (const auto& capability_json : component_json["capabilities"]) {
						Capability capability;
						capability.id = json_utils::getValueOr<std::string>(capability_json, "id", "");
						capability.version = json_utils::getValueOr<int>(capability_json, "version", 1);
						if (!capability.id.empty()) {
							component.capabilities.push_back(std::move(capability));
						}
					}
				}

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

				_components.push_back(std::move(component));
			}
		}
	}

} // namespace smartthings4cpp
