#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.dishwasherOperation capability (Samsung-proprietary; generated). */
	class DishwasherOperation : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.dishwasherOperation";

		DishwasherOperation(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedOperatingState{ [this]() { return _supportedOperatingState; } };
		ReactiveLitepp::ReadonlyProperty<std::string> OperatingState{ [this]() { return _operatingState; } };
		ReactiveLitepp::ReadonlyProperty<bool> Reservable{ [this]() { return _reservable; } };
		ReactiveLitepp::ReadonlyProperty<double> ProgressPercentage{ [this]() { return _progressPercentage; } };
		ReactiveLitepp::ReadonlyProperty<std::string> RemainingTimeStr{ [this]() { return _remainingTimeStr; } };
		ReactiveLitepp::ReadonlyProperty<double> OperationTime{ [this]() { return _operationTime; } };
		ReactiveLitepp::ReadonlyProperty<double> RemainingTime{ [this]() { return _remainingTime; } };
		ReactiveLitepp::ReadonlyProperty<double> TimeLeftToStart{ [this]() { return _timeLeftToStart; } };

		Result<void> resume();
		Result<void> cancel(bool drain);
		Result<void> setOperatingState(const std::string& operatingState);
		Result<void> start(const nlohmann::json& option);
		Result<void> startLater(double delay);
		Result<void> pause();
		std::vector<std::string> commandNames() const override { return { "resume", "cancel", "setOperatingState", "start", "startLater", "pause" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedOperatingState;
		std::string _operatingState;
		bool _reservable = false;
		double _progressPercentage = 0.0;
		std::string _remainingTimeStr;
		double _operationTime = 0.0;
		double _remainingTime = 0.0;
		double _timeLeftToStart = 0.0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
