#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.ambientContent capability (Samsung-proprietary; generated). */
	class AmbientContent : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.ambientContent";

		AmbientContent(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedAmbientApps{ [this]() { return _supportedAmbientApps; } };

		Result<void> setAmbientContent(const std::string& id);
		std::vector<std::string> commandNames() const override { return { "setAmbientContent" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _supportedAmbientApps;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
