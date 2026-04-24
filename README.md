# CONDUIT Runtime Environment ⚡  

**CONDUIT** is an event routing and processing framework designed for High-Frequency Trading (HFT).

Built strictly on C++20 Concepts, CONDUIT eliminates Object-Oriented runtime overhead (no RTTI, no virtual tables).

🚀 Quickstart: The Physical Topography

CONDUIT requires explicit architectural definitions. You do not just "push" an event; you define a logical pipeline, bind it to a physical sink, map it to a conduit, and route it through a cluster.

📂 Repository Structure

- /include/conduit/
- /examples/ 
- /tests/
- /docs/
- /benchmarks/

Built for the microsecond. Architected for the nanosecond.

## ⚡ Performance & Determinism
CONDUIT is a fully standard C++20 library.

| Benchmark               | Latency (Mean) | Iterations   |
|-------------------------|----------------|--------------|
| **BM_Conduit_Push**     | **1.77–1.87 ns** | 407,272,727  |
| **BM_Conduit_FullFlux** | **11.8 ns**      | 64,000,000   |

> **Environment**: Windows 11 Pro, MSVC v143 (Release), AMD Ryzen 5 9600X.

## 💻 Compiler & Platform Compatibility
CONDUIT is a C++20 library, ensuring maximum portability across high-performance environments.

**Current Reference Implementation**

The current reference build targets the following toolchain:

- Compiler: MSVC C++20 (v143) 
- Architecture: Windows x64 
- Standard Library: C++20 STL 

### Roadmap & Cross-Compiler Support
While the core logic relies strictly on standard C++ features (Concepts, Atomics, Memory Barriers), cross-compiler support is being actively integrated into the public roadmap:
- **Clang 15+**
- **GCC 12+**

## ⚖️ Licensing
CONDUIT is released under the GNU Affero General Public License v3 (AGPLv3). For proprietary environments requiring closed-source integration, contact the author for commercial licensing.
