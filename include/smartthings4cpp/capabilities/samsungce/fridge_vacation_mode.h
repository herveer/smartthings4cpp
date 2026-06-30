#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.fridgeVacationMode capability (Samsung-proprietary; generated). */
	class FridgeVacationMode : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.fridgeVacationMode";

		FridgeVacationMode(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> VacationMode{ [this]() { return _vacationMode; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _vacationMode;
	};

} // namespace samsungce
} // namespace smartthings4cpp
