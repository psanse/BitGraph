# BITSCAN benchmarks

Micro-benchmarks of the BITSCAN bitsets against `std::bitset`, `std::vector<bool>` and `boost::dynamic_bitset`, built from
[bench_stl.cpp](../src/bitscan/benchmarks/bench_stl.cpp) and [bench_hotpaths.cpp](../src/bitscan/benchmarks/bench_hotpaths.cpp).

## Setup

| | Linux | Windows |
|---|---|---|
| CPU | Intel Xeon Gold 6342 @ 2.80 GHz (pinned to one core with `taskset`) | Intel Core i9-9900K @ 3.60 GHz |
| OS / compiler | Ubuntu 24.04, GCC 13.3, `-O3 -DNDEBUG` (no `-march=native` unless stated) | Windows 11, MSVC, Release (`/O2`) |
| Bitset size | 100,000 bits (1,563 64-bit blocks) | same |
| Workload | A fraction of the 64-bit blocks (the *occupancy*) is populated with 8 random bits each | same |
| Method | best of 3 rounds, at least 300 ms per case (200 ms on Windows); `&=` is net of the cost of copying the left operand | same |

Two consecutive runs on Linux agreed within about 5% on most rows, and `std::bitset` timings of a few microseconds can move by about 10% between runs, so treat differences under about 15% as noise. The tables are from the second `bench_hotpaths` run and from one `bench_stl` run. The Windows numbers are a single run, and `boost` was not installed there. These are single-machine numbers: use them as orders of magnitude and re-run on your own hardware.

- **dense** = `bitarray` (`Bitset`/`BBScan`), **sparse** = `sparse_bitarray` (`BitsetSp`/`BBScanSp`)
- *scan (portable)* is the loop `for i: if (a.test(i)) ...`, the only scan the C++ standard offers for `std::bitset`
- *scan (libstdc++)* uses `_Find_first`/`_Find_next`, a **non-portable GCC/libstdc++ extension**. The Microsoft standard library has no equivalent.

## Results on Linux (ns per operation, lower is better)

### Dense bitsets: on par with `std::bitset` for bulk operations

| Operation | occupancy | `std::bitset` | BitGraph dense |
|---|---|---:|---:|
| `a &= b` | 1% / 10% / 50% | 4022 / 4050 / 4041 | 4036 / 4063 / 4066 |
| `count()` | 1% / 10% / 50% | 3598 / 3596 / 3598 | 3587 / 3587 / 3592 |
| `is_disjoint` (vs. `(a & b).none()`) | 1% | 962 | 540 |
| | 10% | 431 | 21 |
| | 50% | 413 | 3.2 |

Bulk bitwise operations and `count()` cost the same as `std::bitset`. `is_disjoint` is much faster than building `(a & b)` and testing it, which is consistent with an early exit at the first common block (3 ns at 50% occupancy).

### Scanning

| Occupancy | `std::bitset` portable loop | `std::bitset` libstdc++ `_Find_next` | BitGraph dense scanner | BitGraph sparse scanner |
|---|---:|---:|---:|---:|
| 1% | 77,566 | 1,103 | 1,372 | **216** |
| 10% | 80,315 | 5,613 | **2,962** | **1,892** |
| 50% | 77,312 | 24,548 | **13,728** | **9,200** |

- Against the portable `std::bitset` loop, the dense scanner is 57x / 27x / 5.6x faster (1% / 10% / 50%).
- Against libstdc++'s `_Find_next` extension, the **dense scanner is about 1.9x faster at 10% and 1.8x at 50%**, and 1.2x **slower** at 1%, where the time is spent skipping empty blocks rather than finding bits.
- The **sparse scanner is the fastest**: 5.1x / 3.0x / 2.7x faster than `_Find_next`.

### Sparse bitsets: large wins when few blocks are populated

| Operation | occupancy | `std::bitset` | BitGraph sparse | speedup |
|---|---|---:|---:|---:|
| `a &= b` | 1% | 4022 | 80 | 50x |
| | 10% | 4050 | 703 | 5.8x |
| | 50% | 4041 | 3182 | 1.3x |
| `count()` | 1% | 3598 | 46 | 79x |
| | 10% | 3596 | 394 | 9.1x |
| | 50% | 3598 | 1848 | 1.9x |
| `is_disjoint` | 1% | 962 | 31 | 31x |
| | 10% | 431 | 7.7 | 56x |

**Trade-off:** random access on sparse bitsets is slower than on dense ones. Testing all the bits of a list (`is_bit`) takes 754 ns (sparse) against 144 ns (`std::bitset`) at 1% occupancy, 10,901 ns against 1,219 ns at 10%, and 114,855 ns against 6,055 ns at 50%. Use sparse bitsets for large, mostly empty sets where bulk operations and scanning dominate; at high occupancy the dense type is the better choice.

## Hot paths: scanning and scan-while-modifying (Linux)

`bench_hotpaths` looks at where the scanning bitsets are meant to be used. It adds `boost::dynamic_bitset` (1.83) as a competitor. This matters because `std::bitset<N>` needs its size at compile time, which real graphs rarely allow, so `boost::dynamic_bitset` is the like-for-like baseline for runtime-sized sets.

### Scanning a dense bitset (100,000 bits, ns for the whole scan, lower is better)

| Method | 1% | 10% | 50% |
|---|---:|---:|---:|
| `std::bitset` `_Find_next` (GCC only) | 1,141 | 5,687 | 24,988 |
| `boost::dynamic_bitset` `find_next` | 1,025 | 6,333 | 29,009 |
| `std::bitset` portable `test(i)` loop | 77,142 | 77,360 | 77,126 |
| BITSCAN `bitarray::scan` (persistent scanner) | 1,369 | 2,984 | 11,912 |
| BITSCAN `view::scan` (independent view) | 1,589 | 2,313 | 7,488 |
| BITSCAN stateless `next_bit(bit)` | 1,453 | 5,493 | 24,404 |
| hand-written loop over `block(i)` with `bblock::lsb` | 1,093 | 1,653 | 4,227 |

- The persistent scanner is **1.9 to 2.1 times faster** than `_Find_next` and **2.1 to 2.4 times faster** than `boost::dynamic_bitset::find_next` at 10% and 50% occupancy. At 1% it is 1.2 times slower than `_Find_next` and 1.3 times slower than `boost`.
- `view::scan` is 2.5 to 3.3 times faster than `_Find_next` at 10% and 50%.
- The stateless `next_bit(bit)` is equal to `_Find_next` at 10% and 50% (it recomputes the block at every call).
- A scan written by hand over the exposed 64-bit blocks (`block(i)`, `bblock::lsb`, clear the lowest bit) is 3.4 to 5.9 times faster than `_Find_next` at 10% and 50%, and 1.8 to 2.8 times faster than the persistent scanner. BITSCAN exposes its blocks, so this is available; `std::bitset` offers no equivalent.

### Reverse scan (100,000 bits, ns for the whole scan)

| Method | 1% | 10% | 50% |
|---|---:|---:|---:|
| `std::bitset` descending `test(i)` loop (the only portable option) | 77,942 | 78,058 | 77,962 |
| BITSCAN `bitarray::scanR` | 1,261 | 4,646 | 21,937 |

Neither `std::bitset` nor `boost::dynamic_bitset` has a reverse find. `scanR` is 62, 17 and 3.6 times faster than the descending loop. With `-march=native` (`lzcnt`) it is about 2 times faster still (single run: 2,434 ns at 10% and 9,486 ns at 50%).

### Scan while modifying: bit-parallel greedy colouring (microseconds per colouring)

This is the loop at the core of exact algorithms such as BBMC: take the lowest vertex of a candidate set, remove its neighbours from the set, and continue. The set is changed while it is scanned. All implementations run the same algorithm on the same random graph (`n` vertices, edge probability `p`) and must produce the same number of colours, which the benchmark checks.

| n | p | `std::bitset` full width | `std::bitset` precomputed rows | `boost::dynamic_bitset` | BITSCAN destructive scan | BITSCAN destructive scan + range |
|---:|---:|---:|---:|---:|---:|---:|
| 256 | 0.5 | 1.6 | 1.7 | 2.5 | 2.5 | 2.4 |
| 1024 | 0.1 | 8.9 | 9.0 | 14.4 | 14.4 | 13.2 |
| 1024 | 0.5 | 9.2 | 8.8 | 14.0 | 14.5 | 13.7 |
| 1024 | 0.9 | 9.6 | 9.2 | 15.6 | 16.7 | 15.7 |
| 4096 | 0.1 | 295 | 141 | 191 | 200 | 162 |
| 4096 | 0.5 | 326 | 157 | 200 | 212 | 188 |
| 4096 | 0.9 | 368 | 190 | 227 | 246 | 221 |

- "Destructive scan" is `next_bit_del()` with `andnot_eq`. "+ range" restricts the erase to the blocks from the cursor on (`erase_block(block(v), npos, row)`), which is possible because the lower blocks are already empty. It saves 4 to 19% over the full-width erase.
- Against the **runtime-sized** competitor `boost::dynamic_bitset`, BITSCAN is at **parity or slightly better**: between 1.0 and 1.2 times its speed.
- Against `std::bitset<N>`, whose size is fixed at compile time, BITSCAN is **slower**: about 1.4 to 1.7 times at `n = 256` and `1024`, and 1.15 to 1.2 times against precomputed rows at `n = 4096` (it is 1.7 to 1.8 times faster than the full-width `std::bitset<4096>` version). With `-march=native` (AVX2 and AVX-512 on this CPU) the full-width `std::bitset<4096>` version speeds up by about 2.5 times, while BITSCAN changes by 2 to 6%, so the gap at `n = 4096` becomes 1.2 to 1.6 times (single run).
- A version of the loop written directly on `block(i)` is not consistently better than the API version: between 0.4 and 1.0 times the speed of `std::bitset` with precomputed rows, depending on `n`.
- `std::bitset` with precomputed non-adjacency rows (`nadj[v]`) needs twice the memory for the graph.

## Results on Windows (MSVC): where `std::bitset` has no find-next

`_Find_first` and `_Find_next` are libstdc++ extensions, so on MSVC the only way to scan a `std::bitset` is a loop over `test(i)`. The comparison is therefore against that portable loop: the baseline `std::bitset` offers there, not a tuned one.

| Scan of 100,000 bits | 1% | 10% | 50% |
|---|---:|---:|---:|
| `std::bitset` forward `test(i)` loop (ns) | 64,299 | 64,338 | 62,854 |
| BITSCAN `bitarray::scan` (ns) | 1,537 | 5,012 | 23,084 |
| speedup | 42x | 12.8x | 2.7x |
| `std::bitset` descending `test(i)` loop (ns) | 67,516 | 67,296 | 69,848 |
| BITSCAN `bitarray::scanR` (ns) | 1,328 | 3,364 | 17,060 |
| speedup | 51x | 20x | 4.1x |

Greedy colouring, microseconds per colouring, against the better of the two `std::bitset` variants:

| n | p | `std::bitset` full width | `std::bitset` precomputed rows | BITSCAN destructive scan + range | speedup |
|---:|---:|---:|---:|---:|---:|
| 256 | 0.1 | 2.4 | 2.0 | 1.8 | 1.1x |
| 256 | 0.5 | 7.9 | 7.0 | 2.0 | 3.5x |
| 256 | 0.9 | 18.0 | 16.5 | 2.3 | 7.2x |
| 1024 | 0.1 | 30.4 | 26.2 | 11.3 | 2.3x |
| 1024 | 0.5 | 93.0 | 88.9 | 13.7 | 6.5x |
| 1024 | 0.9 | 222.5 | 211.5 | 17.9 | 11.8x |
| 4096 | 0.1 | 380 | 302 | 119 | 2.5x |
| 4096 | 0.5 | 1,150 | 996 | 137 | 7.3x |
| 4096 | 0.9 | 2,732 | 2,469 | 177 | 13.9x |

The advantage grows with the number of vertices and the density, because each scan step in the portable `std::bitset` loop costs a `test(i)` per bit position.

## What this means

- Dense BITSCAN is **not faster** than `std::bitset` or `boost::dynamic_bitset` for bulk operations (`&=`, `count()`): it costs the same.
- For **scanning** on Linux with GCC, dense BITSCAN is about 2 times faster than libstdc++'s `_Find_next` and `boost::dynamic_bitset::find_next` at 10% and 50% occupancy, and 1.2 to 1.3 times slower at 1%. The sparse scanner is 2.7 to 5.1 times faster than `_Find_next` at every occupancy tested.
- In the **scan-while-modifying** colouring loop BITSCAN matches `boost::dynamic_bitset` (1.0 to 1.2 times its speed) and is 1.15 to 1.7 times slower than `std::bitset<N>` when `N` is a compile-time constant.
- **Portability is a real advantage.** Where the standard library offers no find-next (MSVC), BITSCAN scans 2.7 to 42 times faster than the only portable `std::bitset` loop, and its colouring loop is 1.1 to 14 times faster.
- BITSCAN's strengths are elsewhere as well: sizes chosen at run time with a portable API, **reverse and destructive scans** that other libraries do not offer (reverse is 3.6 to 62 times faster than the only alternative), independent scan views, range-restricted operations, direct access to the 64-bit blocks, and the **sparse** types, which are the large wins in this report.
- There is measurable headroom in the library's own dense paths: a hand-written block loop scans 1.8 to 2.8 times faster than the persistent scanner, and `std::bitset<N>` vectorizes its bulk operations where BITSCAN's runtime-length loops do not.

## Reproducing

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBITSCAN_BUILD_BENCHMARKS=ON
cmake --build build --target bench_stl
./build/bin/benchmarks/bench_stl 300      # on Linux: taskset -c <core> ./bench_stl 300
```

`bench_hotpaths` compares scanning and the scan-while-modifying loop (greedy colouring) with `std::bitset` and, if Boost is installed, `boost::dynamic_bitset`:

```
cmake --build build --target bench_hotpaths
taskset -c <core> ./build/bin/benchmarks/bench_hotpaths 300
```

`bench_bitset` and `bench_masking` in the same folder cover the BITSCAN types only.

## Graphs

There is no counterpart to compare GRAPH with: no standard library, and no widely used library, offers graphs stored
as bitset adjacency matrices with these operations. GRAPH (`ugraph`, `sparse_ugraph`) is therefore not benchmarked
against another library, and no speedup is claimed for it. Its performance is that of the BITSCAN operations it is built
on, measured above; the colouring benchmark is a graph algorithm written on bitset rows.
