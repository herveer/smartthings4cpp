#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.supportsFeatures capability (Samsung-proprietary; generated). */
	class SupportsFeatures : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.supportsFeatures";

		SupportsFeatures(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<bool> MediaOutputSupported = MakeReadonlyProperty<&SupportsFeatures::MediaOutputSupported>([this]() { return _mediaOutputSupported; });
		ReactiveLitepp::ReadonlyProperty<bool> ImeAdvSupported = MakeReadonlyProperty<&SupportsFeatures::ImeAdvSupported>([this]() { return _imeAdvSupported; });
		ReactiveLitepp::ReadonlyProperty<bool> WifiUpdateSupport = MakeReadonlyProperty<&SupportsFeatures::WifiUpdateSupport>([this]() { return _wifiUpdateSupport; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> ExecutableServiceList = MakeReadonlyProperty<&SupportsFeatures::ExecutableServiceList>([this]() { return _executableServiceList; });
		ReactiveLitepp::ReadonlyProperty<bool> RemotelessSupported = MakeReadonlyProperty<&SupportsFeatures::RemotelessSupported>([this]() { return _remotelessSupported; });
		ReactiveLitepp::ReadonlyProperty<bool> ArtSupported = MakeReadonlyProperty<&SupportsFeatures::ArtSupported>([this]() { return _artSupported; });
		ReactiveLitepp::ReadonlyProperty<bool> MobileCamSupported = MakeReadonlyProperty<&SupportsFeatures::MobileCamSupported>([this]() { return _mobileCamSupported; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		bool _mediaOutputSupported = false;
		bool _imeAdvSupported = false;
		bool _wifiUpdateSupport = false;
		std::vector<std::string> _executableServiceList;
		bool _remotelessSupported = false;
		bool _artSupported = false;
		bool _mobileCamSupported = false;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
