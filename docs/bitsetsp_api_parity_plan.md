# Plan: BitsetSp with the same API as Bitset

Branch: `develop_sparse`. Companion document: [bitsetsp_api_parity.md](bitsetsp_api_parity.md) (function-by-function gap table and progress notes).

## Goal

`BitsetSp` (sparse, `src/bitscan/bbset_sparse.h`) offers the same API as `Bitset` (dense, `src/bitscan/bbset.h`), so that generic code can use either. The masking and scanning operations must stay as efficient as possible.

## Decisions

- `size()` keeps its meaning in both classes: number of stored blocks. It is not aligned.
- No `flip` / `flip_block` for now.
- No `BitsetSp(nPop, true)` (it would allocate every block and stop being sparse). No `make_bitset_full` equivalent.
- C++14 (the bitscan target uses `cxx_std_14`): no `concept`; the shared API is checked with typed gtest cases and `decltype` / `static_assert` detection.
- `vBB_` may hold zero blocks (`erase_bit(bit)` and `&=` leave them). Code must not assume every stored block is non-zero. `size()` can therefore exceed the number of non-zero blocks.

## Rules for every step

1. **Hot paths are as fast as possible.** Masking and scanning use single merge passes over the sorted blocks, exit early, and avoid virtual calls and extra copies.
2. **Reuse existing code.** New functions wrap or extend existing merge loops and block helpers; aliases are inline forwarders with no extra cost.
3. **Measure.** `bench_bitset` (`src/bitscan/benchmarks`, CMake option `BITSCAN_BUILD_BENCHMARKS`) is run before and after changes that touch hot paths. The full ctest suite must pass after each step.

## Steps and status

| # | Step | Status |
|---|---|---|
| 1 | Gap table: compare both APIs, classify each difference (OK / ADD / ALIGN / DIFF / EXCL) | Done, see [bitsetsp_api_parity.md](bitsetsp_api_parity.md) |
| 2 | Benchmark target and baseline numbers | Done |
| 3 | Non-breaking additions | Done (except free `OR_block`, see below) |
| 4 | Align conflicting signatures | Done (see list below) |
| 5 | Remove virtual dispatch from hot sparse paths | Done |
| 6 | `BBScanSp` parity with `BBScan` | Done |
| 7 | Speed up the slow spots found by the benchmark | In progress (all done except is_bit)|= done; is_bit pending) |
| 8 | Typed API tests shared by `Bitset` and `BitsetSp` | Done |

### Step 3: additions (done)

`test`, `set`, `clear`, `clear_all`, `erase_all_bits`, `and_eq`, `or_eq`, `andnot_eq`, `bit_capacity`, `assign_bit`, `set_bit(bitpos_list)`, `is_singleton`, `find_first_common`, `extract_set`, `extract_stack`, `extract_array`, `init(nPop, bits)`, constructor `BitsetSp(bitpos_list)`, `make_bitset_sp`. Tests: `BitsetSp_api.*` in `test_bitset_sparse.cpp`.
Pending from this step:
- Free `OR_block`: the dense version is a templated range operation and needs its own design.
- Galloping search in `find_first_common` and `is_disjoint` when one operand is much smaller.

### Step 4: signatures (done)

`erase_bit()` returns `BitsetSp&`; `noexcept` on `next_bit`, `prev_bit`, `is_bit`, `is_empty`, `is_disjoint`, `is_singleton`, `find_first_common`, `test`; `operator bitpos_list()` is `explicit`.
Left as is, on purpose: `num_blocks()` type (`size_t` vs `int`), no `noexcept` on `set_bit` / `erase_bit(bit)` (they may allocate), the deleted range overloads (`set_bit(first,last,rhs)`, `AND(first,last,...)`, `AND_block(first,last,rhs)`), the shape of the free `erase_bit(lhs, rhs)`.

### Step 5: virtual dispatch (done)

`BitsetSp::popcn64` (three overloads) was virtual with no overrides; it is now non-virtual and `noexcept`, so `count()` can inline. The only virtual members left in `BitsetSp` are the destructor and `print`. `BBScanSp` has none.
Dense side not touched: `Bitset::popcn64` / `is_empty` and the scanning functions of `BBScan` are virtual, and `BBSentinel::popcn64` overrides `Bitset::popcn64`. Removing `virtual` there would change `BBSentinel` and needs its own decision.

### Step 6: `BBScanSp` parity (to do)

- Check that `BBScanSp` offers the same scanning API as `BBScan`: `init_scan`, `next_bit` / `prev_bit` and their destructive variants, and nested `scan`, `scanR`, `scanD`, `scanDR` types.
- Apply the copy and move rule: a newly constructed object never inherits an active scan (default-initialized cursor); assignment copies the bitstring only and leaves the target's cursor alone. This is already implemented for `BBScan` (`bbscan.h`, tests in `test_bbscan.cpp`).
- Check how the cursor is interpreted in the sparse case (position in `vBB_` vs block index), and that a scan costs O(stored blocks + set bits).
- Add a sparse version of the independent `view::` scanners (`bbscan_view.h` only supports `BBScan` today), keeping the persistent scanners inline.

### Step 7: performance (to do)

Baseline (`nPop` = 100000, ns per operation; sparse vs dense): at 10% occupancy the `next_bit` loop is about 11 times slower (77 µs vs 6.9 µs), `is_bit` over a list about 14 times slower, `set_bit` building from a list about 4 times slower, and `|=` allocates. At 1% occupancy sparse wins on masking (for example `&=`: 47 ns vs 2055 ns).
Ideas, to be tried one at a time and measured:
1. `next_bit` / `prev_bit`: avoid a full block search per call; scan from the current block (the scanning classes can keep a block position).
2. `is_bit`: cache the last accessed block position as a hint, to make repeated nearby lookups O(1).
3. `set_bit`: faster path for appending in increasing order (list construction); bulk construction by sort + merge.
4. `|=`: merge in place or into a reused buffer instead of allocating a new vector.
5. `extract` (the existing one) can use the block scan already used by the new `extract_*` functions.

### Step 8: tests (to do)

- Shared gtest cases written once and run for both `Bitset` and `BitsetSp` (typed tests), so a drift in either API fails the tests.
- A C++14 detection check (`decltype` / `static_assert`) that the shared member functions exist with compatible return types.
- Keep the benchmark out of ctest (timing programs are not tests).

## Open points

- Free `OR_block` (step 3).
- Dense-side virtual functions (step 5 note).
- (done) `size()` and `num_blocks()` doc comments in `bbset_sparse.h` now say "stored blocks" (can include zero blocks) and "blocks of the universe".
- `num_blocks()` type difference, low priority.

## Step 7 progress (2026-10-13)

Changes (all in `bbset_sparse.h` / `bbset_sparse.cpp`, tests `BitsetSp_perf.*`):
- `next_bit(bit)` / `prev_bit(bit)`: find the starting block with `lower_bound` (O(log blocks)) instead of scanning from the first block on every call, which made a full `next_bit` loop quadratic.
- `set_bit(bit)`: O(1) fast paths for a bit in a block after the last one (append) and for a bit in the last block.
- `operator |=`: counts the new blocks, then merges from the back in place. No `sort()` of the whole vector, no per-block `emplace_back`.

Benchmark, `nPop` = 100000, ns per operation (before -> after):

| Operation | 1% occupancy | 10% occupancy |
|---|---|---|
| `next_bit` loop | 1241 -> 560 | 77460 -> 14923 |
| `\|=` (net of copy) | 254 -> 105 | 3201 -> 917 |
| `set_bit` building from a list | 929 -> 539 | 10157 -> 2751 |

Also done: `^=` uses the same in-place backward merge as `|=` (shared `merge_blocks_in_place`; `x ^= x` zeroes the blocks), and `extract` uses the block scan. Pending: `is_bit` (about 21 ns per call at 10% occupancy, a binary search per call; a cached position would need `mutable` state and would not be thread-safe for const use, so it is not done).

## Step 6 progress (2026-10-14)

Done, in `bbscan_sparse.h`, `detail/bbscan_sparse_imp.h` and the new `bbscan_view_sparse.h`; tests `BBScanSp_*` and `BBScanViewSp.*` in `test_bbscan_sparse.cpp`:
- **Copy/move rule:** `BBScanSp` has explicit default, copy and move constructors (fresh cursor, move is `noexcept`) and copy/move assignment that copy the bitstring only and keep the target's cursor, as `BBScan`. Before, the implicit copy carried the scan cursor along.
- **Cursor safety:** `has_valid_cursor()` is now one unsigned comparison against `vBB_.size()`, so a cursor left over after the blocks were removed (`clear_all`, `erase_bit()`) gives `noBit` instead of reading out of range. Same cost as before.
- **Const scans:** `next_bit()` and `prev_bit()` are `const` and `scan_` is `mutable`, as in `BBScan`.
- **Bug fix:** `init_scan(firstBit, NON_DESTRUCTIVE_REVERSE)` started from the next block with a greater index when the block of `firstBit` was absent, so the reverse scan returned bits above `firstBit`. It now starts at the last block with a lower index (or has no scan if there is none).
- **Sparse views:** `BBScanViewSpT<BitsetT, ScanType>` (independent cursor over the block collection, same semantics as `BBScanViewT`), aliases `view::ForwardSp/ReverseSp/DestructiveForwardSp/DestructiveReverseSp`, and the facade `view::sparse::scan/scanR/scanD/scanDR` in `bitscan.h`.

Differences kept on purpose: `scan_block()` / `scan_bit()` setters stay public in `BBScanSp` (the sparse graph classes use them); scanning functions of `BBScanSp` are non-virtual (those of `BBScan` are virtual); `init_scan(firstBit, DESTRUCTIVE*)` still terminates in `BBScanSp` (not available), while the views support it.

## Step 8 progress (2026-10-14)

Done: `src/bitscan/tests/test_bitset_api.cpp`, typed tests (`BitsetApiTest`) run for `Bitset` and `BitsetSp`.
- **Compile-time group:** return types of the shared member functions, `noexcept` of the queries, construction / conversion traits (explicit only), the free functions `AND`, `OR`, `erase_bit` found by argument-dependent lookup. All matched on the first build, so the API parity of steps 3 and 4 holds.
- **Run-time group (not covered by the differential test):** construction forms, copy / move / comparison, `reset` / `init` / `shrink_to_fit`, bit ranges, `set_bit` from a list or bitset, block ranges (`set_block`, `erase_block`), extraction and conversions, `print` / `to_string`.
- **Gap found and fixed:** `Bitset::to_string()` was non-`const` and returned a trailing newline (it called `print` with the default end of line); `BitsetSp::to_string() const` had none. Now `Bitset::to_string() const` has no newline.
- Deliberately not tested as shared: `size()`, `num_blocks()`, `block()`, `bitset()` and the sparse-only functions, `flip`, the `(nPop, true)` constructor, `count(first, last)` / `popcn64` (public in `Bitset`, protected in `BitsetSp`).

## Dense masking vs std::bitset (2026-10-14)

`bench_stl` showed dense `&=`, `|=`, `^=` about 1.6 times slower than `std::bitset` (about 2.0 µs vs 1.3 µs per operation at 100000 bits). `bench_masking` isolated the cause: the loops in `bbset.cpp` accessed `vBB_` and `nBB_` through `this`. MSVC does not use type-based aliasing, so a store to a `BITBOARD` may modify those members, and they were reloaded in every iteration: no unrolling, no vectorization. The same loop with the pointers and the bound hoisted into locals takes the same time as `std::bitset` (1064 ns -> 259 ns for `&=`, 1563 words, in cache).
Fixed in `bbset.cpp`: `&=`, `|=`, `^=`, `flip`, `erase_bit()`, `erase_bit(rhs)`, `erase_bit(lhs, rhs)`, `set_bit(bitset)`, `assign_bit` and the free `AND`, `OR`, `erase_bit` (with result). Read-only loops (counting, `is_disjoint`, search) have no stores, so they were not affected. Not checked yet: the merge loops of `BitsetSp` (they compare against `vBB_.end()` after stores to the blocks, which may have the same problem).
