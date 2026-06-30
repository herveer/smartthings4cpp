#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.deviceIdentification capability (Samsung-proprietary; generated). */
	class DeviceIdentification : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.deviceIdentification";

		DeviceIdentification(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> MicomAssayCode{ [this]() { return _micomAssayCode; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ModelName{ [this]() { return _modelName; } };
		ReactiveLitepp::ReadonlyProperty<std::string> SerialNumber{ [this]() { return _serialNumber; } };
		ReactiveLitepp::ReadonlyProperty<std::string> SerialNumberExtra{ [this]() { return _serialNumberExtra; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ReleaseCountry{ [this]() { return _releaseCountry; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ModelClassificationCode{ [this]() { return _modelClassificationCode; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Description{ [this]() { return _description; } };
		ReactiveLitepp::ReadonlyProperty<int> ReleaseYear{ [this]() { return _releaseYear; } };
		ReactiveLitepp::ReadonlyProperty<std::string> BinaryId{ [this]() { return _binaryId; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _micomAssayCode;
		std::string _modelName;
		std::string _serialNumber;
		std::string _serialNumberExtra;
		std::string _releaseCountry;
		std::string _modelClassificationCode;
		std::string _description;
		int _releaseYear = 0;
		std::string _binaryId;
	};

} // namespace samsungce
} // namespace smartthings4cpp
