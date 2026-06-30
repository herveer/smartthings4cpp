#pragma once

#include "../capability.h"
#include <string>

/**
 * @file ocf.h
 * @brief "ocf" capability (deprecated device-information capability)
 */

namespace smartthings4cpp {

	/**
	 * @brief The @c ocf capability: Open Connectivity Foundation device metadata
	 *
	 * Exposes informational string attributes (device name, manufacturer, model,
	 * firmware, ...). The most useful ones are surfaced as named read-only
	 * properties; any raw OCF field is reachable via field(). Command:
	 * @c postOcfCommand(href, value).
	 *
	 * @note Deprecated in the SmartThings reference but still reported by OCF
	 *       devices (Samsung appliances, TVs, ...).
	 */
	class Ocf : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "ocf";

		Ocf(int version, std::string componentId, std::string deviceId, Client* client);

		/** @brief Device name (OCF @c n) (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> DeviceName{ [this]() { return _n; } };
		/** @brief Manufacturer name (OCF @c mnmn) (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> ManufacturerName{ [this]() { return _mnmn; } };
		/** @brief Model number (OCF @c mnmo) (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> ModelNumber{ [this]() { return _mnmo; } };
		/** @brief Vendor id (OCF @c vid) (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> VendorId{ [this]() { return _vid; } };
		/** @brief Device id (OCF @c di) (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> DeviceId{ [this]() { return _di; } };
		/** @brief Firmware version (OCF @c mnfv) (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> FirmwareVersion{ [this]() { return _mnfv; } };
		/** @brief Platform version (OCF @c mnpv) (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> PlatformVersion{ [this]() { return _mnpv; } };
		/** @brief OS version (OCF @c mnos) (reactive, read-only) */
		ReactiveLitepp::ReadonlyProperty<std::string> OsVersion{ [this]() { return _mnos; } };

		/**
		 * @brief Read any OCF field by its short name (e.g. "mnhw", "icv", "pi")
		 * @param name OCF attribute name
		 * @return The field value, or an empty string if absent
		 */
		std::string field(const std::string& name) const { return attributeString(name); }

		/** @brief Post a raw OCF command (@c postOcfCommand) */
		Result<void> postOcfCommand(const std::string& href, const nlohmann::json& value);

		std::vector<std::string> commandNames() const override { return { "postOcfCommand" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _n, _mnmn, _mnmo, _vid, _di, _mnfv, _mnpv, _mnos;
	};

} // namespace smartthings4cpp
