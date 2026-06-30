#pragma once

#include "../capability.h"

/**
 * @file execute.h
 * @brief "execute" capability (deprecated, but still present on many devices)
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c execute capability: run a raw command and read back data
	 *
	 * Attribute: @c data (an arbitrary JSON object). Command:
	 * @c execute(command, args?).
	 *
	 * @note This capability is deprecated in the SmartThings reference but is still
	 *       reported by many appliances; it is provided for completeness.
	 */
	class Execute : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "execute";

		Execute(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief The last @c data payload returned by the device (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Data{
			[this]() { return _data; }
		};

		/**
		 * @brief Run a raw command (@c execute command)
		 * @param command The command string (often an OCF resource href)
		 * @param args Optional JSON object of arguments
		 */
		Result<void> execute(const std::string& command,
			const nlohmann::json& args = nlohmann::json::object());

		std::vector<std::string> commandNames() const override { return { "execute" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _data = nlohmann::json::object();
	};

} // namespace smartthings4cpp
