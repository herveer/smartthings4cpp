#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.energyPlanner capability (Samsung-proprietary; generated). */
	class EnergyPlanner : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.energyPlanner";

		EnergyPlanner(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Data{ [this]() { return _data; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Plan{ [this]() { return _plan; } };

		Result<void> setPlan(const std::string& plan);
		Result<void> setData(const nlohmann::json& data);
		std::vector<std::string> commandNames() const override { return { "setPlan", "setData" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _data;
		std::string _plan;
	};

} // namespace samsungce
} // namespace smartthings4cpp
