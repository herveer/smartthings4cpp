#pragma once

#include "types.h"
#include "capability.h"
#include <memory>
#include <string>
#include <vector>

/**
 * @file component.h
 * @brief A device component: a named group of capabilities
 */

namespace smartthings4cpp {
	using namespace ReactiveLitepp;

	// Forward declarations
	class Device;

	/**
	 * @brief A component of a device (e.g. "main", "freezer", "cooler")
	 *
	 * A component groups the capability objects it exposes, plus optional category
	 * hints. It owns its capabilities (polymorphic Capability instances), so it is
	 * move-only.
	 *
	 * PropertyChanged relays every owned capability's PropertyChanged - see
	 * addCapability(). Device::PropertyChanged does the same one level up,
	 * relaying every component's PropertyChanged, so a change on any capability
	 * anywhere on a device is observable by subscribing once at whichever level
	 * (capability/component/device) suits the caller.
	 */
	struct Component {
		std::string id;                                       ///< Component id (e.g. "main")
		std::string label;                                    ///< Human readable component label
		std::vector<std::unique_ptr<Capability>> capabilities;///< Owned capability objects
		std::vector<Category> categories;                     ///< Category hints for this component

		/// Owning device. Set once this Component has been placed at its final,
		/// stable address (see Device::initFromJson()) - nullptr before then.
		Device* device = nullptr;

		/**
		 * @brief Fires whenever any owned capability's PropertyChanged fires
		 *
		 * Forwards the original event unchanged: the sender argument your
		 * handler receives is still the Capability that actually changed, not
		 * this Component, so you can tell which capability/attribute fired
		 * (e.g. via @c static_cast<Capability&>(sender).component()). Wired up
		 * automatically by addCapability() - nothing to subscribe manually.
		 */
		Event<ObservableObject&, PropertyChangedArgs> PropertyChanged;

		Component() = default;
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		/**
		 * @note PropertyChanged and its capability subscriptions are NOT moved -
		 *       the moved-to Component gets a fresh, empty PropertyChanged with
		 *       no subscribers. This is safe only because a Component is never
		 *       moved after addCapability() has wired anything up: Device::
		 *       initFromJson() reserves the vector's capacity and moves each
		 *       Component into place while it still has zero capabilities,
		 *       *then* calls addCapability() on the now-final address. Written
		 *       by hand rather than `= default` because ReactiveLitepp::Event
		 *       has no move constructor of its own (only a deleted copy
		 *       constructor), which would otherwise make this constructor
		 *       implicitly deleted and break storing Component in a std::vector.
		 */
		Component(Component&& other) noexcept
			: id(std::move(other.id)),
			label(std::move(other.label)),
			capabilities(std::move(other.capabilities)),
			categories(std::move(other.categories)),
			device(other.device) {
		}

		/// @copydoc Component(Component&&)
		Component& operator=(Component&& other) noexcept {
			if (this != &other) {
				id = std::move(other.id);
				label = std::move(other.label);
				capabilities = std::move(other.capabilities);
				categories = std::move(other.categories);
				device = other.device;
			}
			return *this;
		}

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

		/**
		 * @brief Add a capability: wires its Component back-pointer and relays
		 *        its PropertyChanged through this Component's own PropertyChanged
		 * @param cap The capability to add; ignored if null
		 * @note Only call once this Component is at its final, stable address -
		 *       see the move constructor's note above.
		 */
		void addCapability(std::unique_ptr<Capability> cap) {
			if (!cap) {
				return;
			}
			cap->setComponent(this);
			_capabilitySubscriptions.push_back(cap->PropertyChanged.SubscribeScoped(
				[this](ObservableObject& obj, PropertyChangedArgs args) {
					PropertyChanged.Notify(obj, args);
				}));
			capabilities.push_back(std::move(cap));
		}

	private:
		// Declared after `capabilities` so it is destroyed first (members are
		// destroyed in reverse declaration order): unsubscribing before the
		// Capability objects these subscriptions reference are destroyed.
		std::vector<ScopedSubscription> _capabilitySubscriptions;
	};

} // namespace smartthings4cpp
