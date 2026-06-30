#include "smartthings4cpp/capabilities/contact_sensor.h"

#include <utility>

namespace smartthings4cpp {

	ContactSensor::ContactSensor(int version, std::string componentId, std::string deviceId, Client* client)
		: Capability("contactSensor", version, std::move(componentId), std::move(deviceId), client) {
	}

	void ContactSensor::parseStatus(const nlohmann::json&) {
		std::string value = attributeString("contact", _contact);
		if (SetPropertyValueAndNotify<&ContactSensor::Contact>(_contact, value)) {
			NotifyPropertyChanged<&ContactSensor::IsOpen>();
		}
	}

} // namespace smartthings4cpp
