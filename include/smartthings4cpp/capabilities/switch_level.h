#pragma once

#include "../capability.h"
#include <optional>

/**
 * @file switch_level.h
 * @brief "switchLevel" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c switchLevel capability: dimmer level 0-100%
	 *
	 * Attributes: @c level (integer %), @c levelRange. Command:
	 * @c setLevel(level, rate?).
	 */
	class SwitchLevel : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "switchLevel";

		SwitchLevel(int version, std::string componentId, std::string deviceId, Client* client);

		/**
		 * @brief Get or set the dimmer level, 0-100
		 *
		 * Assigning issues @c setLevel with the new value.
		 */
		ReactiveLitepp::Property<int> Level{
			[this]() { return _level; },
			[this](int& value) {
				NotifyPropertyChanging<&SwitchLevel::Level>();
				setLevel(value);
			}
		};

		/** @brief Minimum settable level reported by @c levelRange (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<int> MinLevel{
			[this]() { return _minLevel; }
		};

		/** @brief Maximum settable level reported by @c levelRange (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<int> MaxLevel{
			[this]() { return _maxLevel; }
		};

		/**
		 * @brief Set the dimmer level (@c setLevel command)
		 * @param level Target level, 0-100
		 * @param rate Optional transition rate
		 */
		Result<void> setLevel(int level, std::optional<int> rate = std::nullopt);

		std::vector<std::string> commandNames() const override { return { "setLevel" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _level = 0;
		int _minLevel = 0;
		int _maxLevel = 100;
	};

} // namespace smartthings4cpp
