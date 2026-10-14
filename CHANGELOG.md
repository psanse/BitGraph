# Changelog

## Unreleased

Changes since the 2.0.0 tag.

### Fixed

- `extract_first_k(k, bitset, vec)` did not compile for `bitarray`, and its loop condition (`||` instead of `&&`) did not stop after `k` bits. It now returns the first `k` set bits, or all of them if there are fewer.
- `Graph::create_subgraph_first_k(first_k)` (by-value overload for directed graphs) did not compile when used; it now calls the existing `create_subgraph_first_k(first_k, out)`.
- `bitarray` and `sparse_bitarray` now expose the stateless `prev_bit(bit)` next to `next_bit(bit)`; it was hidden by the stateful `prev_bit()` overloads.

### Added

- Opt-in Doxygen API reference: configure with `-DBITGRAPH_BUILD_DOCS=ON` and build the `docs` target (see [docs/README.md](docs/README.md)).
- API overview pages for [BITSCAN](docs/api/bitscan.md), [GRAPH](docs/api/graph.md) and [UTILS](docs/api/utils.md), a [benchmark report](docs/benchmarks.md), [CITATION.cff](CITATION.cff) and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
- Rewritten landing and library READMEs, with a banner and compiled examples.

### Documentation

- NIST is acknowledged as the source of the Matrix Market I/O code (`mmio.h`, `mmio.cpp`).
- Class-level descriptions for the main classes (`Bitset`, `BitsetSp`, `BBScanSp`, `Graph`, `infoBase`, ...), and many corrected `@param` names and Doxygen tags in the headers.

## 2.0.0

Major refactoring of the BITSCAN, GRAPH and UTILS libraries. **This release is not source-compatible with 1.x**: see [Migrating from 1.x](#migrating-from-1x). The last 1.x release is v1.4.3 and is frozen.

### BITSCAN

- New scanning API. The `BBObject::Scan`, `ScanRev`, `ScanDest` and `ScanDestRev` helper structs (`bbo::Scan<bitarray>`) are replaced by persistent scanners nested in the bitset types:
  `bitarray::scan`, `scanR`, `scanD`, `scanDR` (and the same for `sparse_bitarray`). Implementation in `bitscan/detail/persistent_scan.h`.
- New independent-cursor views, `bitgraph::view::scan`, `scanR`, `scanD`, `scanDR` and `view::sparse::*` (`bbscan_view.h`, `bbscan_view_sparse.h`). A view keeps its own cursor and does not touch the bitset's scan state.
- New objects never inherit a scan cursor on copy or move.
- `Bitset` and `BitsetSp` provide `and_assign()` and `or_assign()` (forwarders of `&=` and `|=`). They are named so because `and_eq`/`or_eq` are reserved operator keywords in standard C++ and cannot be used as identifiers with GCC or Clang.
- `BitsetSp` (sparse) extended towards the API of `Bitset` (see [docs/bitsetsp_api_parity.md](docs/bitsetsp_api_parity.md)).
- Implementation code moved to `bitscan/detail/` (`bbset_imp.h`, `bbscan_imp.h`, ...). `bbexcep_hand.h` removed.
- `BBSentinel` moved from `_impl::` to `experimental::` (`watched_bitarray`).
- New fixed-width and index aliases in `bbtypes.h` (`block_t`, `bit_index_t`, `block_index_t`, `bit_indices`, `bit_index_set`).

### GRAPH

- Headers renamed and split. Use the facade `graph/graph.h`; it pulls in `graph_unweighted.h`, `ugraph_vertex_weighted.h` and `ugraph_edge_weighted.h`.
- Vertex-weighted graphs renamed from `*_W` to `*_VW`: `Graph_W` -> `Graph_VW`, `Base_Graph_W` -> `Base_Graph_VW`, `ugraph_w` -> `ugraph_vw`, `ugraph_wi` -> `ugraph_vwi`, `GraphFastRootSort_W` -> `GraphFastRootSort_VW`, `WeightGen` -> `VertexWeightGen`. Edge-weighted aliases are `ugraph_ew` and `ugraph_ewi`.
- Graph algorithms regrouped by graph kind in namespace `graph_utils`: `unweighted_graph_utils.h`, `vertex_weighted_graph_utils.h`, `edge_weighted_graph_utils.h`. The old `gfunc`/`gbbs` namespaces and `graph_func.h`, `graph_sort.h` are gone.
- `Decode` / `DecodeVertex` -> `OrderingDecoder`; `GraphMap` -> `OrderingMap` (`algorithms/ordering_decoder.h`, `ordering_map.h`).
- File formats now live in `graph/formats/detail/` under namespace `io` (was `gio`): DIMACS, edge list, Matrix Market, vertex-set.
- New: `algorithm_info.h` (`BasicAlgorithmInfo`, `AlgorithmParameters`, `CriterionCollection`), `graph_benchmark.h` (`BasicGraphBenchmark`, replaces `utils::Benchmark`), `vertex_utils.h`, `weighted_constants.h`.
- `kcore`, `clq_func` and `graph_sort` are not part of the public API in this release; they are parked in `graph/detail/toMOVE/`.
- New experimental, unstable component: `bitgraph::experimental::VertexRange` (`graph/detail/vertex_range.h`).

### UTILS

- Monolithic `common.h` (namespace `com`) split into `collection_utils.h`, `math_utils.h`, `string_utils.h`, `sort_utils.h`, `random_utils.h`, `time_utils.h`, `path_utils.h`, `file_utils.h`, `task_utils.h`; namespace is now `utils`.
- `prec_timer.h` -> `precise_timer.h`; `FixedStack` is now `fixed_stack.h`.
- Old logger kept under `utils/deprecated/`.

### Build and docs

- Version is now 2.0.0. C++14 and CMake >= 3.14 are still required.
- Verified with MSVC (Visual Studio 2026) and GCC 13.3 (Ubuntu 24.04): full build, test suite, examples, and installation consumed through `find_package(BitGraph)`.
- Many new tests and examples for BITSCAN and GRAPH.

## Migrating from 1.x

| 1.x | 2.0 |
|---|---|
| `bbo::Scan<bitarray> sc(bb);` | `bitarray::scan sc(bb);` |
| `bbo::ScanRev` / `ScanDest` / `ScanDestRev` | `bitarray::scanR` / `scanD` / `scanDR` |
| `Graph_W<ugraph,double>`, `ugraph_w`, `ugraph_wi` | `Graph_VW<ugraph,double>`, `ugraph_vw`, `ugraph_vwi` |
| `GraphFastRootSort_W` | `GraphFastRootSort_VW` |
| `Decode`, `GraphMap` | `OrderingDecoder`, `OrderingMap` |
| namespace `gio` | namespace `io` |
| namespace `com`, `common.h` | namespace `utils`, `*_utils.h` headers |
| namespaces `gfunc`, `gbbs` | namespace `graph_utils` |
| `utils::Benchmark` | `BasicGraphBenchmark` |
| `#include "graph/graph_basic.h"`, `graph_vertex_weighted.h`, `graph_edge_weighted.h` | `#include "graph/graph.h"` |
| `_impl::BBSentinel` | `experimental::BBSentinel` |

Installed headers are still included as `#include "BitGraph/bitscan/bitscan.h"`, `"BitGraph/graph/graph.h"`; CMake targets are still `BitGraph::utils`, `BitGraph::bitscan`, `BitGraph::graph`.
