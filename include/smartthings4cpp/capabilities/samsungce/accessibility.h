#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace samsungce {

	/** @brief The @c samsungce.accessibility capability (Samsung-proprietary; generated). */
	class Accessibility : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "samsungce.accessibility";

		Accessibility(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedColorInversions{ [this]() { return _supportedColorInversions; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Magnification{ [this]() { return _magnification; } };
		ReactiveLitepp::ReadonlyProperty<int> MagnificationLevel{ [this]() { return _magnificationLevel; } };
		ReactiveLitepp::ReadonlyProperty<std::string> HighContrast{ [this]() { return _highContrast; } };
		ReactiveLitepp::ReadonlyProperty<std::string> VoiceGuide{ [this]() { return _voiceGuide; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedVoiceGuides{ [this]() { return _supportedVoiceGuides; } };
		ReactiveLitepp::ReadonlyProperty<std::string> ColorInversion{ [this]() { return _colorInversion; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Grayscale{ [this]() { return _grayscale; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedReluminos{ [this]() { return _supportedReluminos; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedMagnificationLevels{ [this]() { return _supportedMagnificationLevels; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedMagnifications{ [this]() { return _supportedMagnifications; } };
		ReactiveLitepp::ReadonlyProperty<std::string> Relumino{ [this]() { return _relumino; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedOptions{ [this]() { return _supportedOptions; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedHighContrasts{ [this]() { return _supportedHighContrasts; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedFontSizes{ [this]() { return _supportedFontSizes; } };
		ReactiveLitepp::ReadonlyProperty<int> FontSize{ [this]() { return _fontSize; } };
		ReactiveLitepp::ReadonlyProperty<std::vector<std::string>> SupportedGrayscales{ [this]() { return _supportedGrayscales; } };

		Result<void> setVoiceGuide(const std::string& voiceGuide);
		Result<void> setRelumino(const std::string& relumino);
		Result<void> setMagnification(const std::string& magnification);
		Result<void> setHighContrast(const std::string& highContrast);
		Result<void> setGrayscale(const std::string& grayscale);
		Result<void> setColorInversion(const std::string& colorInversion);
		Result<void> setMagnificationLevel(int magnificationLevel);
		Result<void> setFontSize(int fontSize);
		std::vector<std::string> commandNames() const override { return { "setVoiceGuide", "setRelumino", "setMagnification", "setHighContrast", "setGrayscale", "setColorInversion", "setMagnificationLevel", "setFontSize" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::vector<std::string> _supportedColorInversions;
		std::string _magnification;
		int _magnificationLevel = 0;
		std::string _highContrast;
		std::string _voiceGuide;
		std::vector<std::string> _supportedVoiceGuides;
		std::string _colorInversion;
		std::string _grayscale;
		std::vector<std::string> _supportedReluminos;
		nlohmann::json _supportedMagnificationLevels;
		std::vector<std::string> _supportedMagnifications;
		std::string _relumino;
		std::vector<std::string> _supportedOptions;
		std::vector<std::string> _supportedHighContrasts;
		nlohmann::json _supportedFontSizes;
		int _fontSize = 0;
		std::vector<std::string> _supportedGrayscales;
	};

} // namespace samsungce
} // namespace smartthings4cpp
