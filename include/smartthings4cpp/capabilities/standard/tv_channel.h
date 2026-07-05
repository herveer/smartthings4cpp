#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c tvChannel capability (generated). */
	class TvChannel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "tvChannel";

		TvChannel(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> TvChannelValue{ [this]() { return _tvChannelValue; } };
		ReactiveLitepp::ReadonlyProperty<std::string> TvChannelName{ [this]() { return _tvChannelName; } };

		Result<void> channelDown();
		Result<void> setTvChannel(const std::string& tvChannel);
		Result<void> channelUp();
		Result<void> setTvChannelName(const std::string& tvChannelName);
		std::vector<std::string> commandNames() const override { return { "channelDown", "setTvChannel", "channelUp", "setTvChannelName" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _tvChannelValue;
		std::string _tvChannelName;
	};

} // namespace standard
} // namespace smartthings4cpp
