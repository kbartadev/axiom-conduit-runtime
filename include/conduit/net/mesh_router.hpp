/*
 * SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-Commercial
 *
 * Conduit Runtime Environment (CRE)
 * Copyright (c) 2026 Kristóf Barta (https://github.com/kbartadev). All rights reserved.
 *
 * For the full licensing terms and commercial options, please refer to the LICENSE
 * and NOTICE files located in the root directory of this distribution.
 */

#pragma once

#include <array>

#include "../core.hpp"
#include "networked_conduit.hpp"

namespace cre::net {

// ============================================================
// 13. INVARIANT: NODE-TO-NODE ROUTING SYSTEM
// O(1) deterministic network distributor (Switch).
// ============================================================

// A concept guaranteeing that the event can tell where it is headed
template <typename Event>
concept RoutableEvent = cre::net::ConduitEvent<Event> && requires(Event ev) {
    { ev.target_node_id } -> std::convertible_to<uint16_t>;
};

template <RoutableEvent Event, size_t MaxNodes, size_t ConduitCapacity = 1024>
class mesh_router {
    // O(1) flat array of conduits leading to remote servers.
    // Index = target Node ID.
    std::array<networked_conduit<Event, ConduitCapacity>*, MaxNodes> routes_{};

   public:
    mesh_router() { routes_.fill(nullptr); }

    // Building the network topology (called by the Orchestrator at startup or during failover)
    void bind_route(uint16_t node_id, networked_conduit<Event, ConduitCapacity>& conduit) noexcept {
        if (node_id < MaxNodes) {
            routes_[node_id] = &conduit;
        }
    }

    void unbind_route(uint16_t node_id) noexcept {
        if (node_id < MaxNodes) {
            routes_[node_id] = nullptr;
        }
    }

    // ============================================================
    // HOT PATH — Zero synchronization, zero copying
    // ============================================================
    void on(conduit::event_ptr<Event>& ev) noexcept {
        if (!ev) return;

        const uint16_t target = ev->target_node_id;

        // 1. O(1) safety check
        if (target >= MaxNodes) {
            // Invalid destination (CONDUIT physics: immediate drop)
            return;
        }

        auto* conduit = routes_[target];

        // 2. O(1) push into the network gate
        if (conduit && conduit->is_alive()) {
            // The network pipe takes ownership of the memory (move).
            // The actual TCP send does not happen here, but on the network thread.
            conduit->push(ev);
        }
        // 3. If the conduit is dead or null, 'ev' goes out of scope,
        // and the memory returns to the L1 cache pool with zero overhead.
    }
};

}  // namespace cre::net
