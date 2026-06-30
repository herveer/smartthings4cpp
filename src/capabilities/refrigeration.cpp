#include "smartthings4cpp/capabilities/refrigeration.h"

#include <utility>

namespace smartthings4cpp {

	Refrigeration::Refrigeration(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("refrigeration", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Refrigeration::parseStatus(const nlohmann::json&) {
		// Backing fields are strings ("on"/"off") while the properties are bool, so
		// notify the bool properties manually when a value changes.
		std::string rapidCooling = attributeString("rapidCooling", _rapidCooling);
		if (rapidCooling != _rapidCooling) {
			NotifyPropertyChanging<&Refrigeration::RapidCooling>();
			_rapidCooling = rapidCooling;
			NotifyPropertyChanged<&Refrigeration::RapidCooling>();
		}

		std::string rapidFreezing = attributeString("rapidFreezing", _rapidFreezing);
		if (rapidFreezing != _rapidFreezing) {
			NotifyPropertyChanging<&Refrigeration::RapidFreezing>();
			_rapidFreezing = rapidFreezing;
			NotifyPropertyChanged<&Refrigeration::RapidFreezing>();
		}

		std::string defrost = attributeString("defrost", _defrost);
		if (defrost != _defrost) {
			NotifyPropertyChanging<&Refrigeration::Defrost>();
			_defrost = defrost;
			NotifyPropertyChanged<&Refrigeration::Defrost>();
		}
	}

	Result<void> Refrigeration::setRapidCooling(const std::string& mode) {
		auto result = executeCommand("setRapidCooling", nlohmann::json::array({ mode }));
		if (result.isSuccess()) {
			setLocalAttribute("rapidCooling", mode);
		}
		return result;
	}

	Result<void> Refrigeration::setRapidFreezing(const std::string& mode) {
		auto result = executeCommand("setRapidFreezing", nlohmann::json::array({ mode }));
		if (result.isSuccess()) {
			setLocalAttribute("rapidFreezing", mode);
		}
		return result;
	}

	Result<void> Refrigeration::setDefrost(const std::string& mode) {
		auto result = executeCommand("setDefrost", nlohmann::json::array({ mode }));
		if (result.isSuccess()) {
			setLocalAttribute("defrost", mode);
		}
		return result;
	}

} // namespace smartthings4cpp
