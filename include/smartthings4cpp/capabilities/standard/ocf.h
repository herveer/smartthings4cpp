#pragma once

#include "../../capability.h"

namespace smartthings4cpp {
namespace standard {

	/** @brief The @c ocf capability (generated). */
	class Ocf : public Capability {
	public:
		static constexpr const char* CAPABILITY_ID = "ocf";

		Ocf(int version, std::string componentId, std::string deviceId, Client* client);

		ReactiveLitepp::ReadonlyProperty<std::string> St = MakeReadonlyProperty<&Ocf::St>([this]() { return _st; });
		ReactiveLitepp::ReadonlyProperty<std::string> Mndt = MakeReadonlyProperty<&Ocf::Mndt>([this]() { return _mndt; });
		ReactiveLitepp::ReadonlyProperty<std::string> Mnfv = MakeReadonlyProperty<&Ocf::Mnfv>([this]() { return _mnfv; });
		ReactiveLitepp::ReadonlyProperty<std::string> Mnhw = MakeReadonlyProperty<&Ocf::Mnhw>([this]() { return _mnhw; });
		ReactiveLitepp::ReadonlyProperty<std::string> Di = MakeReadonlyProperty<&Ocf::Di>([this]() { return _di; });
		ReactiveLitepp::ReadonlyProperty<std::string> Mnsl = MakeReadonlyProperty<&Ocf::Mnsl>([this]() { return _mnsl; });
		ReactiveLitepp::ReadonlyProperty<std::string> Dmv = MakeReadonlyProperty<&Ocf::Dmv>([this]() { return _dmv; });
		ReactiveLitepp::ReadonlyProperty<std::string> N = MakeReadonlyProperty<&Ocf::N>([this]() { return _n; });
		ReactiveLitepp::ReadonlyProperty<std::string> Mnmo = MakeReadonlyProperty<&Ocf::Mnmo>([this]() { return _mnmo; });
		ReactiveLitepp::ReadonlyProperty<std::string> Vid = MakeReadonlyProperty<&Ocf::Vid>([this]() { return _vid; });
		ReactiveLitepp::ReadonlyProperty<std::string> Mnmn = MakeReadonlyProperty<&Ocf::Mnmn>([this]() { return _mnmn; });
		ReactiveLitepp::ReadonlyProperty<std::string> Mnml = MakeReadonlyProperty<&Ocf::Mnml>([this]() { return _mnml; });
		ReactiveLitepp::ReadonlyProperty<std::string> Mnpv = MakeReadonlyProperty<&Ocf::Mnpv>([this]() { return _mnpv; });
		ReactiveLitepp::ReadonlyProperty<std::string> Mnos = MakeReadonlyProperty<&Ocf::Mnos>([this]() { return _mnos; });
		ReactiveLitepp::ReadonlyProperty<std::string> Pi = MakeReadonlyProperty<&Ocf::Pi>([this]() { return _pi; });
		ReactiveLitepp::ReadonlyProperty<std::string> Icv = MakeReadonlyProperty<&Ocf::Icv>([this]() { return _icv; });

		Result<void> postOcfCommand(const std::string& href, const nlohmann::json& value);
		std::vector<std::string> commandNames() const override { return { "postOcfCommand" }; }

	protected:
		void parseStatus(const nlohmann::json& status) override;

	private:
		std::string _st;
		std::string _mndt;
		std::string _mnfv;
		std::string _mnhw;
		std::string _di;
		std::string _mnsl;
		std::string _dmv;
		std::string _n;
		std::string _mnmo;
		std::string _vid;
		std::string _mnmn;
		std::string _mnml;
		std::string _mnpv;
		std::string _mnos;
		std::string _pi;
		std::string _icv;
	};

} // namespace standard
} // namespace smartthings4cpp
