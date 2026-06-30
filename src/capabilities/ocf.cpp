#include "smartthings4cpp/capabilities/ocf.h"

#include <utility>

namespace smartthings4cpp {

	Ocf::Ocf(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("ocf", version, std::move(componentId), std::move(deviceId), client) {
	}

	void Ocf::parseStatus(const nlohmann::json&) {
		SetPropertyValueAndNotify<&Ocf::DeviceName>(_n, attributeString("n", _n));
		SetPropertyValueAndNotify<&Ocf::ManufacturerName>(_mnmn, attributeString("mnmn", _mnmn));
		SetPropertyValueAndNotify<&Ocf::ModelNumber>(_mnmo, attributeString("mnmo", _mnmo));
		SetPropertyValueAndNotify<&Ocf::VendorId>(_vid, attributeString("vid", _vid));
		SetPropertyValueAndNotify<&Ocf::DeviceId>(_di, attributeString("di", _di));
		SetPropertyValueAndNotify<&Ocf::FirmwareVersion>(_mnfv, attributeString("mnfv", _mnfv));
		SetPropertyValueAndNotify<&Ocf::PlatformVersion>(_mnpv, attributeString("mnpv", _mnpv));
		SetPropertyValueAndNotify<&Ocf::OsVersion>(_mnos, attributeString("mnos", _mnos));
	}

	Result<void> Ocf::postOcfCommand(const std::string& href, const nlohmann::json& value) {
		return executeCommand("postOcfCommand", nlohmann::json::array({ href, value }));
	}

} // namespace smartthings4cpp
