#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.viewInside capability (Samsung-proprietary; generated). */
	class ViewInside : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.viewInside";

		ViewInside(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedFocusAreas{ [this]() { return _supportedFocusAreas; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Contents{ [this]() { return _contents; } };
		ReactiveLitepp::ReadonlyProperty<std::string> LastUpdatedTime{ [this]() { return _lastUpdatedTime; } };

		Result<void> refreshSpecificArea(const std::string& focusArea);
		Result<void> refreshAll();
		Result<void> refresh(const std::string& fileId);
		std::vector<std::string> commandNames() const override { return { "refreshSpecificArea", "refreshAll", "refresh" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedFocusAreas;
		nlohmann::json _contents;
		std::string _lastUpdatedTime;
	};

} // namespace samsungce
} // namespace smartthings4cpp
