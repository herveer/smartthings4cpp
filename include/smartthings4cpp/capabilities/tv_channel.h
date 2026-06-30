#pragma once

#include "../capability.h"
#include <string>

/**
 * @file tv_channel.h
 * @brief "tvChannel" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c tvChannel capability: the current TV channel
	 *
	 * Attributes: @c tvChannel, @c tvChannelName. Commands: @c setTvChannel,
	 * @c setTvChannelName, @c channelUp, @c channelDown.
	 */
	class TvChannel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "tvChannel";

		TvChannel(int version, std::string componentId, std::string deviceId, Client* client);

		/**
		 * @brief Get or set the channel number/identifier
		 *
		 * Assigning issues @c setTvChannel with the new value.
		 */
		ReactiveLitepp::Property<std::string> Channel{
			[this]() { return _channel; },
			[this](std::string& value) {
				NotifyPropertyChanging<&TvChannel::Channel>();
				setTvChannel(value);
			}
		};

		/**
		 * @brief Get or set the channel name
		 *
		 * Assigning issues @c setTvChannelName with the new value.
		 */
		ReactiveLitepp::Property<std::string> ChannelName{
			[this]() { return _channelName; },
			[this](std::string& value) {
				NotifyPropertyChanging<&TvChannel::ChannelName>();
				setTvChannelName(value);
			}
		};

		Result<void> setTvChannel(const std::string& tvChannel);
		Result<void> setTvChannelName(const std::string& tvChannelName);
		Result<void> channelUp();
		Result<void> channelDown();

		std::vector<std::string> commandNames() const override {
			return { "setTvChannel", "setTvChannelName", "channelUp", "channelDown" };
		}

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _channel;
		std::string _channelName;
	};

} // namespace smartthings4cpp
