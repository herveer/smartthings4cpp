#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.error capability (Samsung-proprietary; generated). */
	class Error : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.error";

		Error(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ErrorValue = MakeReadonlyProperty<&Error::ErrorValue>([this]() { return _errorValue; });

		Result<void> setError(const nlohmann::json& error);
		std::vector<std::string> commandNames() const override { return { "setError" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _errorValue;
	};

} // namespace custom
} // namespace smartthings4cpp
