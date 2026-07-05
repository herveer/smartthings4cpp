#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c battery capability (generated). */
	class Battery : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "battery";

		Battery(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> Quantity{ [this]() { return _quantity; } };
		ReactiveLitepp::ReadonlyProperty<int> BatteryValue{ [this]() { return _batteryValue; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Type{ [this]() { return _type; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _quantity = 0;
		int _batteryValue = 0;
		std::string _type;
	};

} // namespace standard
} // namespace smartthings4cpp
