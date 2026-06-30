#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.ambient capability (Samsung-proprietary; generated). */
	class Ambient : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.ambient";

		Ambient(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Info{ [this]() { return _info; } };

		Result<void> sendData(const nlohmann::json& data);
		Result<void> setAmbientOn();
		std::vector<std::string> commandNames() const override { return { "sendData", "setAmbientOn" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _info;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
