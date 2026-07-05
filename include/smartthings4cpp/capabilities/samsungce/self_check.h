#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.selfCheck capability (Samsung-proprietary; generated). */
	class SelfCheck : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.selfCheck";

		SelfCheck(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> ResultAttr = MakeReadonlyProperty<&SelfCheck::ResultAttr>([this]() { return _resultAttr; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedActions = MakeReadonlyProperty<&SelfCheck::SupportedActions>([this]() { return _supportedActions; });
		ReactiveLitepp::ReadonlyProperty<int> Progress = MakeReadonlyProperty<&SelfCheck::Progress>([this]() { return _progress; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> Errors = MakeReadonlyProperty<&SelfCheck::Errors>([this]() { return _errors; });
		ReactiveLitepp::ReadonlyProperty<std::string> Status = MakeReadonlyProperty<&SelfCheck::Status>([this]() { return _status; });

		Result<void> cancelSelfCheck();
		Result<void> startSelfCheck();
		std::vector<std::string> commandNames() const override { return { "cancelSelfCheck", "startSelfCheck" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _resultAttr;
		std::vector<std::string> _supportedActions;
		int _progress = 0;
		nlohmann::json _errors;
		std::string _status;
	};

} // namespace samsungce
} // namespace smartthings4cpp
