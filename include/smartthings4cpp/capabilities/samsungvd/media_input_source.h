#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.mediaInputSource capability (Samsung-proprietary; generated). */
	class MediaInputSource : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.mediaInputSource";

		MediaInputSource(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedInputSourcesMap = MakeReadonlyProperty<&MediaInputSource::SupportedInputSourcesMap>([this]() { return _supportedInputSourcesMap; });
		ReactiveLitepp::ReadonlyProperty<std::string> InputSource = MakeReadonlyProperty<&MediaInputSource::InputSource>([this]() { return _inputSource; });

		Result<void> setInputSource(const std::string& id);
		std::vector<std::string> commandNames() const override { return { "setInputSource" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _supportedInputSourcesMap;
		std::string _inputSource;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
