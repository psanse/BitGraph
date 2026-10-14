# BITSCAN benchmarks

Micro-benchmarks of the BITSCAN bitsets against `std::bitset` (and `std::vector<bool>`), built from
[src/bitscan/benchmarks/bench_stl.cpp](../src/bitscan/benchmarks/bench_stl.cpp).

## Setup

| | |
|---|---|
| CPU | Intel Xeon Gold 6342 @ 2.80 GHz (benchmark pinned to one core with `taskset`) |
| OS / compiler | Ubuntu 24.04, GCC 13.3, `-O3 -DNDEBUG` (no `-march=native`) |
| Bitset size | 100,000 bits (1,563 64-bit blocks) |
| Workload | A fraction of the 64-bit blocks (the *occupancy*) is populated with 8 random bits each |
| Method | best of 3 rounds, at least 300 ms per case; `&=` is net of the cost of copying the left operand |

Two consecutive runs differed by up to about 10% on most rows and by more on a few (for example
`std::bitset` `&=` at 1% occupancy took 5514 ns in the first run and 4024 ns in the second), so treat
differences under about 15% as noise. The tables below are from the second run. These are single-machine
numbers: use them as orders of magnitude and re-run on your own hardware.

- **dense** = `bitarray` (`Bitset`/`BBScan`), **sparse** = `sparse_bitarray` (`BitsetSp`/`BBScanSp`)
- *scan (portable)* is the loop `for i: if (a.test(i)) ...`, the only scan the C++ standard offers for `std::bitset`
- *scan (libstdc++)* uses `_Find_first`/`_Find_next`, a **non-portable GCC/libstdc++ extension**

## Results (ns per operation, lower is better)

### Dense bitsets: on par with `std::bitset` for bulk operations

| Operation | occupancy | `std::bitset` | BitGraph dense |
|---|---|---:|---:|
| `a &= b` | 1% / 10% / 50% | 4024 / 4051 / 4031 | 3981 / 3931 / 3961 |
| `count()` | 1% / 10% / 50% | 3655 / 3587 / 3598 | 3606 / 3600 / 3598 |
| `is_disjoint` (vs. `(a & b).none()`) | 1% | 990 | 540 |
| | 10% | 432 | 21 |
| | 50% | 413 | 3.2 |

Bulk bitwise operations and `count()` cost the same as `std::bitset`. `is_disjoint` is much faster than
building `(a & b)` and testing it, which is consistent with an early exit at the first common block (3 ns
at 50% occupancy).

### Scanning

| Occupancy | `std::bitset` portable loop | `std::bitset` libstdc++ `_Find_next` | BitGraph dense scanner | BitGraph sparse scanner |
|---|---:|---:|---:|---:|
| 1% | 82,270 | 1,106 | 1,931 | **239** |
| 10% | 79,190 | **5,596** | 6,473 | **2,077** |
| 50% | 81,668 | **24,611** | 26,035 | **10,082** |

- Against the portable `std::bitset` loop, the dense scanner is 43x / 12x / 3x faster (1% / 10% / 50%).
- Against libstdc++'s `_Find_next` extension, the **dense scanner is not faster**: it is about 1.7x slower at 1%,
  1.2x at 10% and 1.06x at 50%. The dense scanner's value is a portable, uniform API (forward, reverse,
  destructive and non-destructive scans), not raw speed over `_Find_next`.
- The **sparse scanner is the fastest**: 4.6x / 2.7x / 2.4x faster than `_Find_next`.

### Sparse bitsets: large wins when few blocks are populated

| Operation | occupancy | `std::bitset` | BitGraph sparse | speedup |
|---|---|---:|---:|---:|
| `a &= b` | 1% | 4024 | 79 | 51x |
| | 10% | 4051 | 683 | 5.9x |
| | 50% | 4031 | 3104 | 1.3x |
| `count()` | 1% | 3655 | 46 | 79x |
| | 10% | 3587 | 394 | 9.1x |
| | 50% | 3598 | 1861 | 1.9x |
| `is_disjoint` | 1% | 990 | 35 | 29x |
| | 10% | 432 | 8.1 | 53x |

**Trade-off:** random access on sparse bitsets is slower than on dense ones. Testing all the bits of a
list (`is_bit`) takes 667 ns (sparse) against 179 ns (`std::bitset`) at 1% occupancy, and 106,091 ns
against 6,076 ns at 50%. Use sparse bitsets for large, mostly empty sets where bulk operations and
scanning dominate; at high occupancy the dense type is the better choice.

## Reproducing

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBITSCAN_BUILD_BENCHMARKS=ON
cmake --build build --target bench_stl
./build/bin/benchmarks/bench_stl 300      # on Linux: taskset -c <core> ./bench_stl 300
```

`bench_bitset` and `bench_masking` in the same folder cover the BITSCAN types only.

## Graphs

No comparable third-party baseline is benchmarked for `ugraph`/`sparse_ugraph`. Graph operations are
built on the bitset operations above, so these results are the reference for graph performance.
