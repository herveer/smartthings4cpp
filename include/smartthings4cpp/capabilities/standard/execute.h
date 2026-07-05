#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c execute capability (generated). */
	class Execute : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "execute";

		Execute(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Data{ [this]() { return _data; } };

		Result<void> execute(const std::string& command, const nlohmann::json& args);
		std::vector<std::string> commandNames() const override { return { "execute" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		nlohmann::json _data;
	};

} // namespace standard
} // namespace smartthings4cpp
