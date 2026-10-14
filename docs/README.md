# BitGraph documentation

![BitGraph 2.0](images/bitgraph-banner.svg)

Start with the [main README](../README.md) for an overview, a quick start and installation.

## Guides

| Library | Guide | API overview |
|---|---|---|
| BITSCAN: bitsets and bit scanning | [src/bitscan/README.md](../src/bitscan/README.md) | [api/bitscan.md](api/bitscan.md) |
| GRAPH: graphs stored as bitsets | [src/graph/README.md](../src/graph/README.md) | [api/graph.md](api/graph.md) |
| UTILS: supporting utilities | [src/utils/README.md](../src/utils/README.md) | [api/utils.md](api/utils.md) |

Each guide explains what the library is for and walks through examples. Each API overview lists the main classes and functions, grouped by task.

## Reference material

- [benchmarks.md](benchmarks.md): measured performance of the BITSCAN bitsets against `std::bitset`, with the method and the commands to reproduce it.
- [../CHANGELOG.md](../CHANGELOG.md): what changed in 2.0, with a migration table from 1.x.
- [../CITATION.cff](../CITATION.cff): how to cite BitGraph.

## Development notes

These files record design decisions made during development. They are not user documentation.

- [bitsetsp_api_parity.md](bitsetsp_api_parity.md): function-by-function comparison of the `Bitset` and `BitsetSp` APIs.
- [bitsetsp_api_parity_plan.md](bitsetsp_api_parity_plan.md): the plan and decisions behind that comparison.
