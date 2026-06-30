#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace hca {

	/** @brief The @c hca.washerMode capability (Samsung-proprietary; generated). */
	class WasherMode : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "hca.washerMode";

		WasherMode(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Mode{ [this]() { return _mode; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedModes{ [this]() { return _supportedModes; } };

		Result<void> setMode(const std::string& mode);
		std::vector<std::string> commandNames() const override { return { "setMode" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _mode;
		std::vector<std::string> _supportedModes;
	};

} // namespace hca
} // namespace smartthings4cpp
