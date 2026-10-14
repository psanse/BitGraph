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

## Hot paths: scanning and scan-while-modifying

The tables above come from `bench_stl`. A second benchmark, [bench_hotpaths.cpp](../src/bitscan/benchmarks/bench_hotpaths.cpp), looks at where the scanning bitsets are meant to be used. It adds `boost::dynamic_bitset` (1.83) as a competitor. This matters because `std::bitset<N>` needs its size at compile time, which real graphs rarely allow, so `boost::dynamic_bitset` is the like-for-like baseline for runtime-sized sets. Same machine and flags as above (second run, `-O3`, one core).

### Scanning a dense bitset (100,000 bits, ns for the whole scan, lower is better)

| Method | 1% | 10% | 50% |
|---|---:|---:|---:|
| `std::bitset` `_Find_next` (GCC only) | 1,439 | 5,689 | 24,956 |
| `boost::dynamic_bitset` `find_next` | 1,292 | 6,310 | 28,666 |
| `std::bitset` portable `test(i)` loop | 102,889 | 82,962 | 83,623 |
| BITSCAN `bitarray::scan` (persistent scanner) | 2,464 | 6,514 | 26,237 |
| BITSCAN `view::scan` (independent view) | 2,247 | 5,057 | 19,007 |
| BITSCAN stateless `next_bit(bit)` | 1,853 | 8,543 | 38,471 |
| hand-written loop over `block(i)` with `bblock::lsb` | 1,094 | 1,645 | 4,208 |

What this shows:

- The persistent scanner is **not faster** than `_Find_next` or `boost::dynamic_bitset::find_next`. It is about equal at 10% and 50% occupancy, and 1.7 to 1.9 times slower at 1% (against `_Find_next` and `boost::dynamic_bitset`). The cursor is kept in the bitset and written back on every bit.
- `view::scan` is 1.1 to 1.3 times faster than `_Find_next` at 10% and 50% occupancy, and 1.25 to 1.5 times faster than `boost::dynamic_bitset`.
- A scan written by hand over the exposed 64-bit blocks (`block(i)`, `bblock::lsb`, clear the lowest bit) is **3.5 to 6 times faster** than `_Find_next` at 10% and 50%. BITSCAN exposes its blocks, so this is available; `std::bitset` offers no equivalent. The library's own scanners do not yet reach that speed, because they keep the cursor in memory and must tolerate changes to the set while scanning.

### Reverse scan (100,000 bits, ns for the whole scan)

| Method | 1% | 10% | 50% |
|---|---:|---:|---:|
| `std::bitset` descending `test(i)` loop (the only portable option) | 78,013 | 77,950 | 78,045 |
| BITSCAN `bitarray::scanR` | 1,791 | 5,161 | 19,241 |

Neither `std::bitset` nor `boost::dynamic_bitset` has a reverse find. `scanR` is 44, 15 and 4 times faster than the descending loop.

### Scan while modifying: bit-parallel greedy colouring (microseconds per colouring)

This is the loop at the core of exact algorithms such as BBMC: take the lowest vertex of a candidate set, remove its neighbours from the set, and continue. The set is changed while it is scanned. All implementations run the same algorithm on the same random graph (`n` vertices, edge probability `p`) and must produce the same number of colours, which the benchmark checks.

| n | p | `std::bitset` full width | `std::bitset` precomputed rows | `boost::dynamic_bitset` | BITSCAN destructive scan | BITSCAN destructive scan + range |
|---:|---:|---:|---:|---:|---:|---:|
| 256 | 0.5 | 1.6 | 1.7 | 2.4 | 3.0 | 2.7 |
| 1024 | 0.1 | 8.8 | 9.0 | 14.2 | 15.1 | 13.7 |
| 1024 | 0.5 | 9.1 | 8.9 | 14.5 | 15.8 | 15.0 |
| 1024 | 0.9 | 9.7 | 9.2 | 15.8 | 18.5 | 17.9 |
| 4096 | 0.1 | 274 | 141 | 186 | 201 | 171 |
| 4096 | 0.5 | 296 | 157 | 195 | 213 | 196 |
| 4096 | 0.9 | 338 | 182 | 225 | 255 | 234 |

- "Destructive scan" is `next_bit_del()` with `andnot_eq`. "+ range" restricts the erase to the blocks from the cursor on (`erase_block(block(v), npos, row)`), which is possible because the lower blocks are already empty. It saves 3 to 15% over the full-width erase.
- Against the **runtime-sized** competitor `boost::dynamic_bitset`, BITSCAN is at **parity**: between 0.88 and 1.08 times its speed.
- Against `std::bitset<N>`, whose size is fixed at compile time, BITSCAN is **slower**: about 1.5 to 2 times at `n = 256` and `1024`, and 1.2 to 1.3 times at `n = 4096`. With `-march=native` (AVX2 and AVX-512 on this CPU) the full-width `std::bitset<4096>` version speeds up by 2.3 to 2.5 times, while BITSCAN changes by about 10%, so the gap at `n = 4096` becomes 1.4 to 1.7 times.
- A version of the loop written directly on `block(i)` is not consistently better than the API version: between 0.4 and 1.0 times the speed of `std::bitset` with precomputed rows, depending on `n`, and its timings varied between runs.
- `std::bitset` with precomputed non-adjacency rows (`nadj[v]`) needs twice the memory for the graph.

### On MSVC: where `std::bitset` has no find-next

`_Find_first` and `_Find_next` are libstdc++ extensions. The Microsoft standard library has no equivalent, so on MSVC the only way to scan a `std::bitset` is a loop over `test(i)`. The same benchmark on Windows (Intel Core i9-9900K at 3.6 GHz, Windows 11, MSVC 19.5 Release, `/O2`, a single run with 200 ms per case; `boost` was not installed) therefore compares BITSCAN against that portable loop. This is a different result from the Linux one above and should be read as such: it is the baseline `std::bitset` offers there, not a tuned one.

| Scan of 100,000 bits | 1% | 10% | 50% |
|---|---:|---:|---:|
| `std::bitset` forward `test(i)` loop (ns) | 63,221 | 62,427 | 62,548 |
| BITSCAN `bitarray::scan` (ns) | 1,488 | 4,887 | 22,695 |
| speedup | 42x | 12.8x | 2.8x |
| `std::bitset` descending `test(i)` loop (ns) | 67,278 | 68,512 | 68,810 |
| BITSCAN `bitarray::scanR` (ns) | 1,335 | 3,473 | 17,143 |
| speedup | 50x | 19.7x | 4.0x |

Greedy colouring, microseconds per colouring, against the better of the two `std::bitset` variants:

| n | p | `std::bitset` full width | `std::bitset` precomputed rows | BITSCAN destructive scan + range | speedup |
|---:|---:|---:|---:|---:|---:|
| 256 | 0.1 | 2.4 | 2.0 | 1.9 | 1.05x |
| 256 | 0.5 | 7.6 | 6.7 | 1.9 | 3.5x |
| 256 | 0.9 | 17.4 | 16.0 | 2.3 | 7.0x |
| 1024 | 0.1 | 30.7 | 27.5 | 13.3 | 2.1x |
| 1024 | 0.5 | 87.5 | 85.1 | 15.8 | 5.4x |
| 1024 | 0.9 | 208.2 | 205.9 | 18.6 | 11.1x |
| 4096 | 0.1 | 363 | 289 | 119 | 2.4x |
| 4096 | 0.5 | 1,119 | 976 | 140 | 7.0x |
| 4096 | 0.9 | 2,732 | 2,452 | 175 | 14.0x |

The advantage grows with the number of vertices and the density, because each scan step in the portable `std::bitset` loop costs a `test(i)` per bit position.

### What this means

- Dense BITSCAN is **not faster** than `std::bitset` or `boost::dynamic_bitset` for bulk operations or for plain scanning. On Linux with GCC it matches `boost::dynamic_bitset` and trails `std::bitset<N>` when `N` is a compile-time constant. It matches the runtime-sized alternative and loses to `std::bitset<N>` when `N` is known at compile time.
- **Portability is a real advantage.** Where the standard library offers no find-next (MSVC), BITSCAN scans 3 to 42 times faster than the only portable `std::bitset` loop, and its scan-while-modifying colouring loop is 1.05 to 14 times faster (see the MSVC section above).
- BITSCAN's strengths are elsewhere: sizes chosen at run time with a portable API, **reverse and destructive scans** that other libraries do not offer (reverse is 4 to 44 times faster than the only alternative), independent scan views, range-restricted operations, direct access to the 64-bit blocks, and the **sparse** types, which are the large wins in the first part of this report.
- There is measurable headroom in the library's own dense paths: a hand-written block loop scans 3.5 to 6 times faster than the persistent scanner, and `std::bitset<N>` vectorizes its bulk operations where BITSCAN's runtime-length loops do not.

## Reproducing

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBITSCAN_BUILD_BENCHMARKS=ON
cmake --build build --target bench_stl
./build/bin/benchmarks/bench_stl 300      # on Linux: taskset -c <core> ./bench_stl 300
```

`bench_hotpaths` compares scanning and the scan-while-modifying loop (greedy colouring) with `std::bitset` and, if Boost is installed, `boost::dynamic_bitset`:

```
cmake --build build --target bench_hotpaths
taskset -c <core> ./build/bin/benchmarks/bench_hotpaths 200
```

`bench_bitset` and `bench_masking` in the same folder cover the BITSCAN types only.

## Graphs

No comparable third-party baseline is benchmarked for `ugraph`/`sparse_ugraph`. Graph operations are
built on the bitset operations above, so these results are the reference for graph performance.
