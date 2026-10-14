# UTILS

**The small supporting library under BITSCAN and GRAPH: containers, timers, logging, random numbers and helpers.**

UTILS is part of [BitGraph](../../README.md). [BITSCAN](../bitscan/README.md) and [GRAPH](../graph/README.md) are built on it, and its public headers are available to your own code too. It is deliberately small and has no dependencies besides the standard library.

## Contents

- [What is in UTILS](#what-is-in-utils)
- [Getting started](#getting-started)
- [A short example](#a-short-example)
- [Logging](#logging)
- [Examples and tests](#examples-and-tests)
- [Folder layout](#folder-layout)
- [License and citation](#license-and-citation)

## What is in UTILS

| Header | Provides |
|---|---|
| `fixed_stack.h` | `FixedStack`: a fixed-capacity contiguous stack for trivial types, with constant-time operations |
| `precise_timer.h` | `PrecisionTimer`: elapsed time on a monotonic clock (`std::chrono::steady_clock`) |
| `time_utils.h` | converting durations to seconds, measuring elapsed time, formatting timestamps |
| `random_utils.h` | uniform random numbers, with seeding for reproducible sequences |
| `collection_utils.h` | helpers to inspect collections (`all_equal`, `all_unique`) and print them (`print_collection`) |
| `sort_utils.h` | comparison functors that order elements by an external criterion |
| `string_utils.h` | inspecting and tokenizing strings |
| `path_utils.h` | path-string helpers such as `append_slash` and `remove_path` |
| `file_utils.h` | `File`: an RAII wrapper that opens a file on construction and closes it on destruction |
| `math_utils.h` | small mathematical helpers |
| `task_utils.h` | `run_task_async` and `run_member_task_async`: run a callable asynchronously and wait for its result |
| `batch.h` | `Batch`: a generic container for batches of test configurations |
| `logger.h` | lightweight header-only logging (see [Logging](#logging)) |
| `info/info_base.h` | result metadata and reporting interface for clique-based algorithms (`infoBase`, `paramBase`) |

`utils.h` is a facade that includes the general-purpose headers. It does **not** include `logger.h`, because that header defines macros: include it explicitly where you want logging. `task_utils.h` is also included explicitly.

## Getting started

After installing BitGraph (see the [main README](../../README.md#installation)):

```cmake
find_package(BitGraph CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE BitGraph::utils)
```

```cpp
#include "BitGraph/utils/utils.h"             // general-purpose utilities
#include "BitGraph/utils/logger.h"            // optional: logging macros
```

## A short example

```cpp
#include <iostream>
#include "BitGraph/utils/utils.h"
#include "BitGraph/utils/logger.h"

int main() {
    using namespace bitgraph::utils;

    PrecisionTimer timer;                     // wall-clock timer (monotonic clock)
    timer.wall_tic();

    FixedStack<int> stack(4);                 // fixed capacity, storage allocated once
    stack.push(10);
    stack.push(20);
    std::cout << stack.top() << ' ' << stack.size() << '\n';     // 20 2
    stack.pop();

    const double seconds = timer.wall_toc();
    LOG_INFO("elapsed: %f s", seconds);
}
```

## Logging

`logger.h` is a lightweight, header-only logger based on Logy v1.2 by Giovanni Squillero. It offers `printf`-style macros (`LOG_INFO`, `LOG_ERROR`, ...) and stream-style ones (`LOGG_INFO`, ...). The verbosity is chosen at compile time with `BITGRAPH_LOG_DEBUG`, `BITGRAPH_LOG_VERBOSE` or `BITGRAPH_LOG_ERROR_ONLY`.

When you log a message that is not a literal, pass it as an argument, for example `LOG_ERROR("%s", message)`, never as the format string itself.

## Examples and tests

| Example | Shows |
|---|---|
| [utils_collections_simple.cpp](examples/utils_collections_simple.cpp) | collection helpers and `FixedStack` |
| [utils_time_simple.cpp](examples/utils_time_simple.cpp) | `PrecisionTimer` and timestamps |
| [utils_random_simple.cpp](examples/utils_random_simple.cpp) | uniform random generators and reproducible seeding |
| [utils_file_path_simple.cpp](examples/utils_file_path_simple.cpp) | the `File` wrapper and path helpers |
| [utils_task_simple.cpp](examples/utils_task_simple.cpp) | running a callable and a member function asynchronously |

The unit tests are in [tests/](tests) and run with `ctest`.

## Folder layout

```
utils/
├── utils.h                   facade header (does not include logger.h)
├── fixed_stack.h, precise_timer.h, time_utils.h, random_utils.h, ...   public headers
├── logger.h                  logging macros
├── info/                     result and parameter classes for clique-based algorithms
├── detail/                   implementation details (not public API)
├── deprecated/               old logger kept for reference
├── examples/
└── tests/
```

## License and citation

Released under the [GNU GPL v3](../../LICENSE). To cite BitGraph, see [CITATION.cff](../../CITATION.cff) and the [citation notes](../../README.md#citation). Back to the [main README](../../README.md).
