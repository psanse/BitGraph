# UTILS API overview

The main types and functions of UTILS, grouped by task. This page is a map, not a full reference: for every signature and parameter, build the Doxygen reference (see [docs/README.md](../README.md)) or read the headers in [src/utils](../../src/utils). A guide with examples is in the [UTILS README](../../src/utils/README.md).

Include the general-purpose utilities with `#include "BitGraph/utils/utils.h"`. `logger.h` and `task_utils.h` are included separately. All names are in namespace `bitgraph::utils`.

## Containers and functors

| Name | Header | Purpose |
|---|---|---|
| `FixedStack<T>` | `fixed_stack.h` | fixed-capacity contiguous stack for trivial types. The storage is allocated once. |
| `Batch<ParameterT>` | `batch.h` | stores the parameter configurations of a batch of tests |
| `MeanValue`, `StdDevValue` | `math_utils.h` | accumulate values and compute their mean and standard deviation |
| `Scale<T>` | `math_utils.h` | function object that multiplies each value by a fixed factor |

**`FixedStack`:**

| Function | Purpose |
|---|---|
| `FixedStack(capacity)` | create a stack with the given capacity |
| `push(v)`, `pop()` | add and remove the top element |
| `top()`, `bottom()`, `at(i)`, `operator[]` | access elements |
| `size()`, `capacity()`, `empty()`, `full()` | state |
| `clear()` | remove all elements without releasing storage |
| `reset(capacity)` | reinitialize the stack with a new fixed capacity |
| `truncate(n)` | reduce the logical size |
| `data()` | pointer to the underlying storage |
| `begin()`, `end()` and the reverse and const forms | iterators |
| `push_bottom_swap(v)`, `pop_bottom_swap()` | insert at, or remove, the bottom element in constant time |
| `erase_swap(...)` | remove an element by swapping it with the top element |
| `print(os)` | print the contents |

**`Batch`:** `emplace_test(...)`, `test(i)`, `number_of_tests()`, `clear()`, `empty()`, and iterators (`begin()`, `end()`, `cbegin()`, `cend()`).

## Time

| Name | Header | Purpose |
|---|---|---|
| `PrecisionTimer` | `precise_timer.h` | elapsed time on a monotonic clock |
| `wall_tic()`, `wall_toc()` | | start the timer; seconds elapsed since the last `wall_tic()` |
| `cpu_tic()`, `cpu_toc()` | | kept for compatibility; they also measure elapsed monotonic time, not CPU usage |
| `PrecisionTimer::local_timestamp()` | | the current local timestamp as text |
| `to_seconds(duration)` | `time_utils.h` | convert a duration to seconds |
| `elapsed_time(start)` | `time_utils.h` | seconds (a `double`) elapsed since the `steady_clock` time point `start` |
| `print_duration(out, duration)` | `time_utils.h` | write a duration's tick count and period |
| `to_local_timestamp(time_point)`, `makeTimePoint(...)` | `time_utils.h` | convert between system-clock time points and local calendar components |

## Collections and strings

| Function | Header | Purpose |
|---|---|---|
| `all_equal(c)` | `collection_utils.h` | whether all elements are equal |
| `all_unique(c)` | `collection_utils.h` | whether all elements are different |
| `print_collection(c, os, ...)` | `collection_utils.h` | write the elements, followed by the size |
| `min3(a, b, c)` | `math_utils.h` | the smallest of three values |
| `number_of_words(text)` | `string_utils.h` | count word-like sequences in a string |
| `has_val`, `has_val_diff`, `has_val_prod`, `has_val_with_tb` | `sort_utils.h` | comparison functors that order elements by an external criterion, with variants for differences, products and tie-breaking |
| `has_bigger_size`, `has_smaller_size` | `sort_utils.h` | order collections by decreasing or increasing size |
| `insert_ordered_non_increasing(...)` | `sort_utils.h` | insert an item into arrays ordered by non-increasing score |

## Files and paths

| Name | Header | Purpose |
|---|---|---|
| `File` | `file_utils.h` | RAII wrapper around `std::fstream`: it opens the file when constructed and closes it when destroyed. Modes: `File::Mode::READ`, `WRITE`, `READ_WRITE`, `APPEND`. |
| `File::stream()`, `is_open()`, `good()` | | access the stream and check its state |
| `append_slash(path)` | `path_utils.h` | append a slash to `path`, in place, if it does not end with one |
| `remove_path(filename)` | `path_utils.h` | strip the directory from a file name |

## Random numbers (`random_utils.h`)

| Function | Purpose |
|---|---|
| `uniform_dist(p)` | a Bernoulli trial: `true` with probability `p`. It uses a shared generator that is **not thread-safe**. |

Seeding the shared generators for reproducible runs goes through the handles in `utils::detail` (`iugen::seed(...)`, `rugen::seed(...)`), as shown in [utils_random_simple.cpp](../../src/utils/examples/utils_random_simple.cpp). The `detail` namespace is not a stable API.

## Asynchronous tasks (`task_utils.h`)

| Function | Purpose |
|---|---|
| `run_task_async(f, ...)` | run a callable asynchronously and wait for its result |
| `run_member_task_async(object, member, ...)` | run an object's member function asynchronously and wait for it |

## Logging (`logger.h`)

Include `logger.h` explicitly, because it defines macros. It is a lightweight header-only logger based on Logy.

| Macro | Style |
|---|---|
| `LOG_DEBUG`, `LOG_INFO`, `LOG_WARNING`, `LOG_ERROR` | `printf`-style |
| `LOGG_DEBUG`, `LOGG_INFO`, `LOGG_WARNING`, `LOGG_ERROR` | stream-style |
| `LOG_PRINT` | same as `LOG_INFO` |
| `LOG_LINE()` | log a separator line |
| `LOG_PAK()` | log the message "press any key to continue" (it does not wait for a key) |

Verbosity is chosen at compile time with `BITGRAPH_LOG_DEBUG`, `BITGRAPH_LOG_VERBOSE` or `BITGRAPH_LOG_ERROR_ONLY`. Never pass a non-literal message as the format string: write `LOG_ERROR("%s", message)`.

## Algorithm information (`info/info_base.h`)

| Name | Purpose |
|---|---|
| `infoBase` | result metadata and reporting interface for clique-based algorithms: graph size, time-outs, parsing, preprocessing, search and incumbent times, thread count |
| `paramBase` | the matching parameter base class |
| `startTimer(...)`, `readTimer(...)`, `clearTimer(...)`, `clearAllTimers()` | phase timers on `infoBase` |
| `printReport(os)`, `printParams(os)`, `printTimers(os)` | report output |
