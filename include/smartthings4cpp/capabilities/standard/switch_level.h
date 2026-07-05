#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c switchLevel capability (generated). */
	class SwitchLevel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "switchLevel";

		SwitchLevel(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> LevelRange{ [this]() { return _levelRange; } };
		ReactiveLitepp::ReadonlyProperty<int> Level{ [this]() { return _level; } };

		Result<void> setLevel(int level, int rate);
		std::vector<std::string> commandNames() const override { return { "setLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _levelRange;
		int _level = 0;
	};

} // namespace standard
} // namespace smartthings4cpp
