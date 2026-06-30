#pragma once

#include "../capability.h"
#include <string>

/**
 * @file refrigeration.h
 * @brief "refrigeration" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c refrigeration capability: fridge modes
	 *
	 * Attributes: @c defrost, @c rapidCooling, @c rapidFreezing (each "on"/"off").
	 * Commands: @c setDefrost, @c setRapidCooling, @c setRapidFreezing.
	 */
	class Refrigeration : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "refrigeration";

		Refrigeration(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Get or set rapid cooling (@c setRapidCooling command) */
		ReactiveLitepp::Property<bool> RapidCooling{
			[this]() { return _rapidCooling == "on"; },
			[this](bool& value) {
				NotifyPropertyChanging<&Refrigeration::RapidCooling>();
				setRapidCooling(value ? "on" : "off");
			}
		};

		/** @brief Get or set rapid freezing (@c setRapidFreezing command) */
		ReactiveLitepp::Property<bool> RapidFreezing{
			[this]() { return _rapidFreezing == "on"; },
			[this](bool& value) {
				NotifyPropertyChanging<&Refrigeration::RapidFreezing>();
				setRapidFreezing(value ? "on" : "off");
			}
		};

		/** @brief Get or set defrost (@c setDefrost command) */
		ReactiveLitepp::Property<bool> Defrost{
			[this]() { return _defrost == "on"; },
			[this](bool& value) {
				NotifyPropertyChanging<&Refrigeration::Defrost>();
				setDefrost(value ? "on" : "off");
			}
		};

		/** @brief Set rapid cooling; @p mode is "on" or "off" (@c setRapidCooling command) */
		Result<void> setRapidCooling(const std::string& mode);
		/** @brief Set rapid freezing; @p mode is "on" or "off" (@c setRapidFreezing command) */
		Result<void> setRapidFreezing(const std::string& mode);
		/** @brief Set defrost; @p mode is "on" or "off" (@c setDefrost command) */
		Result<void> setDefrost(const std::string& mode);

		std::vector<std::string> commandNames() const override {
			return { "setDefrost", "setRapidCooling", "setRapidFreezing" };
		}

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _rapidCooling = "off";
		std::string _rapidFreezing = "off";
		std::string _defrost = "off";
	};

} // namespace smartthings4cpp
