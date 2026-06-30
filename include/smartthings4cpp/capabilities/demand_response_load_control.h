#pragma once

#include "../capability.h"
#include <optional>
#include <string>

/**
 * @file demand_response_load_control.h
 * @brief "demandResponseLoadControl" capability
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c demandResponseLoadControl capability (DRLC)
	 *
	 * Attribute: @c drlcStatus (object with @c drlcType, @c drlcLevel, @c start,
	 * @c duration, @c override). Commands: @c overrideDrlcAction(value),
	 * @c requestDrlcAction(drlcType, drlcLevel, start, duration, reportingPeriod?).
	 */
	class DemandResponseLoadControl : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "demandResponseLoadControl";

		DemandResponseLoadControl(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief DRLC type (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<int> DrlcType{ [this]() { return _drlcType; } };
		/** @brief DRLC level (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<int> DrlcLevel{ [this]() { return _drlcLevel; } };
		/** @brief DRLC duration in minutes (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<int> Duration{ [this]() { return _duration; } };
		/** @brief Whether the load-control action is overridden (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<bool> Override{ [this]() { return _override; } };
		/** @brief DRLC start time (ISO-8601) (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> Start{ [this]() { return _start; } };

		/** @brief Override the active DRLC action (@c overrideDrlcAction command) */
		Result<void> overrideDrlcAction(bool value);

		/** @brief Request a DRLC action (@c requestDrlcAction command) */
		Result<void> requestDrlcAction(int drlcType, int drlcLevel, const std::string& start,
			int duration, std::optional<int> reportingPeriod = std::nullopt);

		std::vector<std::string> commandNames() const override {
			return { "overrideDrlcAction", "requestDrlcAction" };
		}

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _drlcType = 0;
		int _drlcLevel = 0;
		int _duration = 0;
		bool _override = false;
		std::string _start;
	};

} // namespace smartthings4cpp
