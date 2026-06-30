#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.washerFreezePrevent capability (Samsung-proprietary; generated). */
	class WasherFreezePrevent : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.washerFreezePrevent";

		WasherFreezePrevent(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> OperatingState{ [this]() { return _operatingState; } };

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _operatingState;
	};

} // namespace samsungce
} // namespace smartthings4cpp
