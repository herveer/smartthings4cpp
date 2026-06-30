#pragma once

#include "../capability.h"

/**
 * @file unknown_capability.h
 * @brief Generic fallback capability for ids without a dedicated class
 */

namespace smartthings4cpp {

	/**
	 * @brief Generic capability used for any id without a typed class yet
	 *
	 * Represents proprietary capabilities (e.g. @c samsungce.*, @c custom.*) and
	 * documented capabilities not yet implemented. It carries the id/version and
	 * exposes the raw cached status via Capability::statusJson(); typed accessors
	 * and commands are added later by promoting it to a dedicated class.
	 */
	class UnknownCapability : public Capability {
	public:
		UnknownCapability(std::string id, int version, std::string componentId,
			std::string deviceId, Client* client);
	};

} // namespace smartthings4cpp
