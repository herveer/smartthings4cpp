#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c contactSensor capability (generated). */
	class ContactSensor : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "contactSensor";

		ContactSensor(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Contact = MakeReadonlyProperty<&ContactSensor::Contact>([this]() { return _contact; });

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _contact;
	};

} // namespace standard
} // namespace smartthings4cpp
