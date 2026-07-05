#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.supportedOptions capability (Samsung-proprietary; generated). */
	class SupportedOptions : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.supportedOptions";

		SupportedOptions(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> Course = MakeReadonlyProperty<&SupportedOptions::Course>([this]() { return _course; });
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ReferenceTable = MakeReadonlyProperty<&SupportedOptions::ReferenceTable>([this]() { return _referenceTable; });
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedCourses = MakeReadonlyProperty<&SupportedOptions::SupportedCourses>([this]() { return _supportedCourses; });

		Result<void> setCourse(const std::string& course);
		std::vector<std::string> commandNames() const override { return { "setCourse" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _course;
		nlohmann::json _referenceTable;
		std::vector<std::string> _supportedCourses;
	};

} // namespace custom
} // namespace smartthings4cpp
