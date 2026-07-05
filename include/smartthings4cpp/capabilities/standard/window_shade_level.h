#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c windowShadeLevel capability (generated). */
	class WindowShadeLevel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "windowShadeLevel";

		WindowShadeLevel(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> ShadeLevel = MakeReadonlyProperty<&WindowShadeLevel::ShadeLevel>([this]() { return _shadeLevel; });

		Result<void> setShadeLevel(int shadeLevel);
		std::vector<std::string> commandNames() const override { return { "setShadeLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _shadeLevel = 0;
	};

} // namespace standard
} // namespace smartthings4cpp
