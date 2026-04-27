/*/*
 * SPDX-License-Identifier: AGPL-3.0-only OR LicenseRef-Commercial
 *
 * Conduit Runtime Environment (CRE)
 * Copyright (c) 2026 Kristóf Barta (https://github.com/kbartadev). All rights reserved.
 *
 * For the full licensing terms and commercial options, please refer to the LICENSE
 * and NOTICE files located in the root directory of this distribution.
 */

#pragma once

#include <cstddef>
#include <new>

namespace cre::core {

// ============================================================
// PHYSICAL CONSTANTS
// Fixing hardware limits for the sake of determinism.
// ============================================================

// 1. L1 Cache Line Size (typically 64 bytes)
// Prevents false sharing (resolves the first contradiction)
constexpr size_t CACHE_LINE_SIZE = 64;

// 2. Cache-line padding wrapper
// Wraps any data so it occupies its own cache line.
template <typename T>
struct alignas(CACHE_LINE_SIZE) padded_wrapper {
    T data;
    // The compiler automatically pads the rest to 64 bytes.
};

// 3. NUMA-aware allocator (principle)
// CONDUIT Core performs all memory allocations on the physical CPU socket
// where the given thread is running.
struct numa_allocator {
    static void* allocate(size_t size, int node_id) {
        // OS-specific calls go here:
        // Linux: numa_alloc_onnode()
        // Windows: VirtualAllocExNuma()
        // This ensures the “Practical Limit” is not violated due to memory latency.
        return ::operator new(size);  // Current fallback until libnuma is used
    }

    static void deallocate(void* ptr, size_t size) { ::operator delete(ptr); }
};

}  // namespace cre::core
