#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.supportsPowerOnByOcf capability (Samsung-proprietary; generated). */
	class SupportsPowerOnByOcf : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.supportsPowerOnByOcf";

		SupportsPowerOnByOcf(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> SupportsPowerOnByOcfValue = MakeReadonlyProperty<&SupportsPowerOnByOcf::SupportsPowerOnByOcfValue>([this]() { return _supportsPowerOnByOcfValue; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _supportsPowerOnByOcfValue;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
