#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace custom {

	/** @brief The @c custom.picturemode capability (Samsung-proprietary; generated). */
	class Picturemode : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "custom.picturemode";

		Picturemode(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> PictureModeValue{ [this]() { return _pictureModeValue; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedPictureModes{ [this]() { return _supportedPictureModes; } };
		ReactiveLitepp::ReadonlyProperty<nlohmann::json> SupportedPictureModesMap{ [this]() { return _supportedPictureModesMap; } };

		Result<void> setPictureMode(const std::string& mode);
		std::vector<std::string> commandNames() const override { return { "setPictureMode" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _pictureModeValue;
		nlohmann::json _supportedPictureModes;
		nlohmann::json _supportedPictureModesMap;
	};

} // namespace custom
} // namespace smartthings4cpp
