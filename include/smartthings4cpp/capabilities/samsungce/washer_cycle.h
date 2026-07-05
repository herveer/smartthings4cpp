#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.washerCycle capability (Samsung-proprietary; generated). */
	class WasherCycle : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.washerCycle";

		WasherCycle(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> CycleType{ [this]() { return _cycleType; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedCycles{ [this]() { return _supportedCycles; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> AvailableCycleTypes{ [this]() { return _availableCycleTypes; } };
		ReactiveLitepp::ReadonlyProperty<std::string> WasherCycleValue{ [this]() { return _washerCycleValue; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> ReferenceTable{ [this]() { return _referenceTable; } };
		ReactiveLitepp::ReadonlyProperty<int> SpecializedFunctionClassification{ [this]() { return _specializedFunctionClassification; } };

		Result<void> setWasherCycle(const std::string& washerCycle);
		Result<void> setCycleType(const std::string& cycleType, const std::string& washerCycle);
		std::vector<std::string> commandNames() const override { return { "setWasherCycle", "setCycleType" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _cycleType;
		nlohmann::json _supportedCycles;
		std::vector<std::string> _availableCycleTypes;
		std::string _washerCycleValue;
		nlohmann::json _referenceTable;
		int _specializedFunctionClassification = 0;
	};

} // namespace samsungce
} // namespace smartthings4cpp
