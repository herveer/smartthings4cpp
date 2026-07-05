#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.kidsLock capability (Samsung-proprietary; generated). */
	class KidsLock : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.kidsLock";

		KidsLock(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> LockState = MakeReadonlyProperty<&KidsLock::LockState>([this]() { return _lockState; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _lockState;
	};

} // namespace samsungce
} // namespace smartthings4cpp
