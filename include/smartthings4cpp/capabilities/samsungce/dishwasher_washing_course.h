#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.dishwasherWashingCourse capability (Samsung-proprietary; generated). */
	class DishwasherWashingCourse : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.dishwasherWashingCourse";

		DishwasherWashingCourse(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> CustomCourseCandidates{ [this]() { return _customCourseCandidates; } };
		ReactiveLitepp::ReadonlyProperty<std::string> WashingCourse{ [this]() { return _washingCourse; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedCourses{ [this]() { return _supportedCourses; } };

		Result<void> setWashingCourse(const std::string& course);
		Result<void> setCustomCourse(const std::string& course);
		Result<void> startWashingCourse(const std::string& course);
		Result<void> startWashingCourseWithOptions(const std::string& course, const nlohmann::json& options);
		std::vector<std::string> commandNames() const override { return { "setWashingCourse", "setCustomCourse", "startWashingCourse", "startWashingCourseWithOptions" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _customCourseCandidates;
		std::string _washingCourse;
		std::vector<std::string> _supportedCourses;
	};

} // namespace samsungce
} // namespace smartthings4cpp
