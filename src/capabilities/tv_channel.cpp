#include "smartthings4cpp/capabilities/tv_channel.h"

#include <utility>

namespace smartthings4cpp {

	TvChannel::TvChannel(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("tvChannel", version, std::move(componentId), std::move(deviceId), client) {
	}

	void TvChannel::parseStatus(const nlohmann::json&) {
		std::string channel = attributeString("tvChannel", _channel);
		SetPropertyValueAndNotify<&TvChannel::Channel>(_channel, channel);

		std::string name = attributeString("tvChannelName", _channelName);
		SetPropertyValueAndNotify<&TvChannel::ChannelName>(_channelName, name);
	}

	Result<void> TvChannel::setTvChannel(const std::string& tvChannel) {
		auto result = executeCommand("setTvChannel", nlohmann::json::array({ tvChannel }));
		if (result.isSuccess()) {
			setLocalAttribute("tvChannel", tvChannel);
		}
		return result;
	}

	Result<void> TvChannel::setTvChannelName(const std::string& tvChannelName) {
		auto result = executeCommand("setTvChannelName", nlohmann::json::array({ tvChannelName }));
		if (result.isSuccess()) {
			setLocalAttribute("tvChannelName", tvChannelName);
		}
		return result;
	}

	Result<void> TvChannel::channelUp() {
		return executeCommand("channelUp");
	}

	Result<void> TvChannel::channelDown() {
		return executeCommand("channelDown");
	}

} // namespace smartthings4cpp
