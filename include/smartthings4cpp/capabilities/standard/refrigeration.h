#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c refrigeration capability (generated). */
	class Refrigeration : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "refrigeration";

		Refrigeration(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Defrost{ [this]() { return _defrost; } };
		ReactiveLitepp::ReadonlyProperty<std::string> RapidCooling{ [this]() { return _rapidCooling; } };
		ReactiveLitepp::ReadonlyProperty<std::string> RapidFreezing{ [this]() { return _rapidFreezing; } };

		Result<void> setDefrost(const std::string& defrost);
		Result<void> setRapidFreezing(const std::string& rapidFreezing);
		Result<void> setRapidCooling(const std::string& rapidCooling);
		std::vector<std::string> commandNames() const override { return { "setDefrost", "setRapidFreezing", "setRapidCooling" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _defrost;
		std::string _rapidCooling;
		std::string _rapidFreezing;
	};

} // namespace standard
} // namespace smartthings4cpp
