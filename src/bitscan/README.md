# BITSCAN

**Bitsets built for scanning: dense or sparse, forward or reverse, destructive or not.**

BITSCAN is the bitset library of [BitGraph](../../README.md), and the foundation of [GRAPH](../graph/README.md). A bitset encodes a set of integers: element *i* is in the set when bit *i* is 1. BITSCAN is tuned for the operation that exact combinatorial algorithms repeat the most, which is enumerating the elements of a set (*bitscanning*), and for the bulk operations around it: intersection, union, difference, counting and overlap tests.

## Contents

- [Why BITSCAN](#why-bitscan)
- [Features](#features)
- [Getting started](#getting-started)
- [Bitset types](#bitset-types)
- [The core API](#the-core-api)
- [Bit scanning](#bit-scanning)
- [Sparse bitsets](#sparse-bitsets)
- [Performance](#performance)
- [Examples, tests and benchmarks](#examples-tests-and-benchmarks)
- [Folder layout](#folder-layout)
- [License and citation](#license-and-citation)

## Why BITSCAN

The C++ standard library has no scan for `std::bitset`: to enumerate its elements portably you test every position. BITSCAN gives you a scanner that jumps from one set bit to the next, in four flavours, behind one small interface, plus the same API on a sparse bitset for very large sets:

```cpp
#include <iostream>
#include "BitGraph/bitscan/bitscan.h"

int main() {
    using namespace bitgraph;

    bitarray bits(130);
    bits.set(2).set(10).set(64).set(129);

    bitarray::scan fwd(bits);                 // non-destructive, forward
    for (int b = fwd.next_bit(); b != BBObject::noBit; b = fwd.next_bit())
        std::cout << b << ' ';                // 2 10 64 129
}
```

## Features

- **Four scanners.** Forward, reverse, destructive forward and destructive reverse (`scan`, `scanR`, `scanD`, `scanDR`). A destructive scan removes each bit as it reads it, which suits algorithms that consume a candidate set.
- **Independent views.** `view::scan` and its siblings each own their cursor, so several scans over the same bitset can run at once.
- **Dense and sparse.** `bitarray` stores every 64-bit block. `sparse_bitarray` stores only the non-empty blocks, with the same API, for large sets with few elements.
- **Rich bulk operations.** Set and clear single bits and closed ranges; flip the whole set; intersection, union and difference; `count()`, `intersects()`, `is_disjoint()` and more.
- **64-bit helpers.** The `bblock` namespace has masks, counting and lowest/highest set bit for a single 64-bit word.
- **Portable C++14.** Built and tested with GCC and MSVC.

## Getting started

After installing BitGraph (see the [main README](../../README.md#installation)):

```cmake
find_package(BitGraph CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE BitGraph::bitscan BitGraph::utils)
```

```cpp
#include "BitGraph/bitscan/bitscan.h"       // the one header for BITSCAN
```

## Bitset types

| Type | Description |
|---|---|
| `bitarray` | main dense bitset, with scanning (recommended) |
| `simple_bitarray` | dense bitset without the scanning state |
| `sparse_bitarray` | main sparse bitset, with scanning (recommended for large, mostly empty sets) |
| `simple_sparse_bitarray` | sparse bitset without the scanning state |
| `watched_bitarray` | **experimental** (`BBSentinel`): restricts operations to the range of non-zero blocks, tracked by a lower and an upper sentinel |

`bitarray` is `BBScan` and `simple_bitarray` is `Bitset`. The sparse counterparts are `BBScanSp` and `BitsetSp`.

## The core API

Positions are zero-based and ranges are closed: both ends are included. Modifiers return `*this`, so calls chain.

```cpp
using namespace bitgraph;

bitarray a(130);                          // 130 bits, all clear
a.set(3).set(10).set_range(20, 25);       // closed range [20,25]
a.clear(10).clear_range(22, 24);          // a = {3, 20, 21, 25}

bitarray b(130);
b.set(3).set(25).set(64);                 // b = {3, 25, 64}

a.count();                                // 4
a.test(3);                                // true
a.intersects(b);                          // true

bitarray inter = a;  inter.and_assign(b);     // {3, 25}
bitarray uni   = a;  uni.or_assign(b);        // {3, 20, 21, 25, 64}
bitarray diff  = a;  diff.andnot_eq(b);       // {20, 21}
```

`and_assign` and `or_assign` are the named forms of `&=` and `|=`. The full tour is in [examples/bitset_core_api.cpp](examples/bitset_core_api.cpp).

## Bit scanning

In a *non-destructive* scan the bitset is unchanged. In a *destructive* scan each bit is removed as it is read, so the bitset is empty at the end. Reverse scans go from the highest bit down.

```cpp
bitarray bits(130);
bits.set(2).set(10).set(64).set(129);

bitarray::scan  fwd(bits);                // 2 10 64 129
bitarray::scanR rev(bits);                // 129 64 10 2
bitarray::scanD des(bits);                // reads 2 10 64 129 and empties 'bits'

for (int b = fwd.next_bit(); b != BBObject::noBit; b = fwd.next_bit()) { /* use b */ }
```

`BBObject::noBit` marks the end of a scan. A scanner built on a bitset uses the cursor stored in that bitset, so only one such scan should be active on a given bitset at a time. When you need several scans at once, use **independent views**, which each own their cursor:

```cpp
bitarray s(130);
s.set(5).set(70);
view::scan  v1(s);                        // forward view
view::scanR v2(s);                        // reverse view
int f1 = v1.next_bit();                   // 5
int r1 = v2.next_bit();                   // 70
int f2 = v1.next_bit();                   // 70: v1 continues independently of v2
```

A view must not outlive the bitset it refers to. See [basic_scanning.cpp](examples/basic_scanning.cpp), [basic_destructive_scanning.cpp](examples/basic_destructive_scanning.cpp) and [independent_views.cpp](examples/independent_views.cpp).

## Sparse bitsets

For sets with millions of possible elements and few of them present, use `sparse_bitarray`. It has the same scanners and operations:

```cpp
sparse_bitarray big(10000000);            // 10M elements
big.set_bit(0);
big.set_bit(900000);
big.set_bit(9999999);

sparse_bitarray::scan scan(big);
for (int bit = scan.next_bit(); bit != BBObject::noBit; bit = scan.next_bit())
    std::cout << bit << ' ';              // 0 900000 9999999
```

## Performance

Measured on a Xeon Gold 6342 with GCC 13 (`-O3`), on 100,000-bit sets. Details and caveats are in [docs/benchmarks.md](../../docs/benchmarks.md).

- Dense `&=` and `count()` run at the same speed as `std::bitset`: not faster.
- The dense forward scanner is 3 to 43 times faster than the portable `test(i)` loop, but not faster than libstdc++'s non-portable `_Find_next`. The reverse scanner `scanR` is 4 to 44 times faster than the only portable alternative.
- Scanning while modifying a set (greedy colouring) runs at parity with `boost::dynamic_bitset` and is 1.2 to 2 times slower than `std::bitset<N>` with a compile-time `N`.
- Where the standard library has no find-next (MSVC), the forward scanner is 3 to 42 times faster than the portable `std::bitset` loop and the colouring loop is 1.05 to 14 times faster.
- Sparse bitsets are about 50 times faster than `std::bitset` for `&=` and about 80 times for `count()` at 1% occupancy, and their scanner is 2 to 5 times faster than `_Find_next`. The price is slower random access (`test`), so use them for large, mostly empty sets.

## Examples, tests and benchmarks

| Example | Shows |
|---|---|
| [bitset_core_api.cpp](examples/bitset_core_api.cpp) | the core dense API: queries, ranges, set operations |
| [basic_scanning.cpp](examples/basic_scanning.cpp) | forward and reverse persistent scans |
| [basic_destructive_scanning.cpp](examples/basic_destructive_scanning.cpp) | destructive scans |
| [independent_views.cpp](examples/independent_views.cpp) | scan views with independent cursors |

The unit tests are in [tests/](tests) and run with `ctest`. The benchmarks in [benchmarks/](benchmarks) are off by default: configure with `-DBITSCAN_BUILD_BENCHMARKS=ON` and build in Release.

## Folder layout

```
bitscan/
├── bitscan.h                 facade header: include this one
├── bbset.h, bbset.cpp        Bitset: dense bitset
├── bbscan.h                  BBScan: Bitset with persistent scanning
├── bbset_sparse.h, .cpp      BitsetSp: sparse bitset
├── bbscan_sparse.h           BBScanSp: BitsetSp with persistent scanning
├── bbscan_view.h, bbscan_view_sparse.h    independent scan views
├── bbsentinel.h              experimental bitset with block sentinels
├── bitblock.h, bitblock.cpp  bblock: operations on 64-bit words
├── bbobject.h, bbtypes.h, bbutils.h, tables.h    shared types and helpers
├── detail/                   implementation details (not public API)
├── examples/  tests/  benchmarks/
```

## License and citation

Released under the [GNU GPL v3](../../LICENSE). To cite BitGraph, see [CITATION.cff](../../CITATION.cff) and the [citation notes](../../README.md#citation). Back to the [main README](../../README.md).
