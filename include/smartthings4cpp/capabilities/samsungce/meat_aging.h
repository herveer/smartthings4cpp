#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.meatAging capability (Samsung-proprietary; generated). */
	class MeatAging : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.meatAging";

		MeatAging(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ZoneInfo{ [this]() { return _zoneInfo; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedMeatTypes{ [this]() { return _supportedMeatTypes; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedAgingMethods{ [this]() { return _supportedAgingMethods; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Status{ [this]() { return _status; } };

		Result<void> addAging(const nlohmann::json& agingOption);
		Result<void> cancelAging(int id);
		Result<void> updateAging(int id, const nlohmann::json& agingOption);
		std::vector<std::string> commandNames() const override { return { "addAging", "cancelAging", "updateAging" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _zoneInfo;
		std::vector<std::string> _supportedMeatTypes;
		std::vector<std::string> _supportedAgingMethods;
		nlohmann::json _status;
	};

} // namespace samsungce
} // namespace smartthings4cpp
