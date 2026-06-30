#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungvd {

	/** @brief The @c samsungvd.soundFrom capability (Samsung-proprietary; generated). */
	class SoundFrom : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungvd.soundFrom";

		SoundFrom(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<int> Mode{ [this]() { return _mode; } };
		ReactiveLitepp::ReadonlyProperty<std::string> DetailName{ [this]() { return _detailName; } };

		Result<void> setSoundFrom(int mode, const std::string& detailName);
		std::vector<std::string> commandNames() const override { return { "setSoundFrom" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		int _mode = 0;
		std::string _detailName;
	};

} // namespace samsungvd
} // namespace smartthings4cpp
