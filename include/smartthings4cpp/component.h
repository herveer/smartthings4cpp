#pragma once

#include "types.h"
#include "capability.h"
#include <string>
#include <vector>
#include <memory>

/**
 * @file component.h
 * @brief A device component: a named group of capabilities
 */

namespace smartthings4cpp {

	/**
	 * @brief A component of a device (e.g. "main", "freezer", "cooler")
	 *
	 * A component groups the capability objects it exposes, plus optional category
	 * hints. It owns its capabilities (polymorphic Capability instances), so it is
	 * move-only.
	 */
	struct Component {
		std::string id;                                       ///< Component id (e.g. "main")
		std::string label;                                    ///< Human readable component label
		std::vector<std::unique_ptr<Capability>> capabilities;///< Owned capability objects
		std::vector<Category> categories;                     ///< Category hints for this component

		Component() = default;
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;
		Component(Component&&) = default;
		Component& operator=(Component&&) = default;

		/**
		 * @brief Whether this component declares a capability by id
		 * @param capabilityId Capability id to look for (e.g. "switch")
		 */
		bool has(const std::string& capabilityId) const {
			return get(capabilityId) != nullptr;
		}

		/**
		 * @brief Get a capability by id (base pointer)
		 * @param capabilityId Capability id to look for
		 * @return Pointer to the capability, or nullptr if absent (owned by this component)
		 */
		Capability* get(const std::string& capabilityId) const {
			for (const auto& capability : capabilities) {
				if (capability && capability->capabilityId() == capabilityId) {
					return capability.get();
				}
			}
			return nullptr;
		}

		/**
		 * @brief Get a capability by its concrete type
		 *
		 * Matches on @c T::CAPABILITY_ID and down-casts. Example:
		 * @code
		 * if (auto* sw = component.get<Switch>()) sw->on();
		 * @endcode
		 * @tparam T A concrete Capability subclass exposing @c CAPABILITY_ID
		 * @return Typed pointer to the capability, or nullptr if absent
		 */
		template<typename T>
		T* get() const {
			for (const auto& capability : capabilities) {
				if (capability && capability->capabilityId() == T::CAPABILITY_ID) {
					return dynamic_cast<T*>(capability.get());
				}
			}
			return nullptr;
		}
	};

} // namespace smartthings4cpp
