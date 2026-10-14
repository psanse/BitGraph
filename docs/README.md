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

## Building the full API reference

The headers carry Doxygen comments. To generate the HTML reference you need [Doxygen](https://www.doxygen.nl/) (1.12 or newer is recommended) and, optionally, [Graphviz](https://graphviz.org/) for class diagrams:

```
cmake -S . -B build -DBITGRAPH_BUILD_DOCS=ON
cmake --build build --target docs
```

The result is `build/docs/html/index.html`. Warnings are written to `build/docs/doxygen_warnings.log`. The target is off by default, so a normal build never needs Doxygen. Pass `-DDOXYGEN_EXECUTABLE=<path>` if Doxygen is not on your `PATH`, and `-DBITGRAPH_DOCS_XML=ON` to also produce the XML output.

Only documented public API is included: `detail/`, tests, examples and benchmarks are excluded. To also list every function whose parameters or return value are not documented, set `WARN_NO_PARAMDOC = YES` in [Doxyfile.in](Doxyfile.in).

## Reference material

- [benchmarks.md](benchmarks.md): measured performance of the BITSCAN bitsets against `std::bitset`, with the method and the commands to reproduce it.
- [../CHANGELOG.md](../CHANGELOG.md): what changed in 2.0, with a migration table from 1.x.
- [../CITATION.cff](../CITATION.cff): how to cite BitGraph.

## Development notes

These files record design decisions made during development. They are not user documentation.

- [bitsetsp_api_parity.md](bitsetsp_api_parity.md): function-by-function comparison of the `Bitset` and `BitsetSp` APIs.
- [bitsetsp_api_parity_plan.md](bitsetsp_api_parity_plan.md): the plan and decisions behind that comparison.
