# BitsetSp vs Bitset: API gap table

Step 1 of the BitsetSp API parity plan. Source: declarations in `src/bitscan/bbset.h` (dense `Bitset`) and `src/bitscan/bbset_sparse.h` (`BitsetSp`).
Built from a read of the public declarations, not from compiling; confirm each row when implementing it.

**Out of scope (decided):** `size()` keeps its meaning (stored blocks) in both classes; no `flip` / `flip_block`; no `BitsetSp(nPop, true)`. C++14.
**Rules:** hot paths (masking, scanning) as efficient as possible; reuse existing code. The cost column is the expected cost on a sparse set with `B` stored blocks.

Action codes: **OK** already matches, **ADD** missing, **ALIGN** exists with a different signature, **DIFF** intentionally different, **EXCL** excluded.

## 1. Construction, lifetime, sizing

| Bitset | BitsetSp | Action | Notes / how |
|---|---|---|---|
| `Bitset()` | `BitsetSp() noexcept` | OK | |
| `Bitset(nPop, bool=false)` | `BitsetSp(nPop)` | OK | `bool` form excluded for `true`; `value=false` is the same as `BitsetSp(nPop)`. A defaulted `bool` is not needed |
| `Bitset(const bit_indices&)` | none | ADD | O(n log n) via sorted insert; reuse `BitsetSp(nPop, bitpos_list)` after computing nPop (max bit + 1) |
| `Bitset(nPop, const ColT&)` (template) | `BitsetSp(nPop, const bitpos_list&)` | ALIGN | generalize to any container; delegate to the existing constructor |
| `Bitset(nPop, init_list)` | same | OK | |
| copy / move ctor and assignment | defaulted | OK | BitsetSp already has them (my earlier plan said they were missing) |
| `make_bitset(n)`, `make_bitset_full(n)`, `make_bitset(n, {..})` | none | ADD | `make_bitset_sp(n)` and `make_bitset_sp(n, {..})` only; no `_full` (excluded) |
| `reset(nPop)` | `reset(nPop, bool is_popsize=true)` | ALIGN | `reset(nPop)` already works via the default argument; check the `bool` semantic |
| `reset(nPop, bit_indices)` | `reset(nPop, bitpos_list)` | OK | same type (`std::vector<int>`) |
| `init(nPop)`, `init(nPop, bits)` | `init(nPop, bool)` only | ADD | add `init(nPop, bitpos_list)` forwarding to `reset` |
| `shrink_to_fit()` | same | OK | |
| `num_blocks()` returns `int` | returns `size_t` | ALIGN | same meaning (universe blocks); type differs, low priority |
| `size()` | `size()` | EXCL | unchanged |
| `bit_capacity()` | none | ADD | `num_blocks() * WORD_SIZE`, trivial inline |

## 2. Access to storage

| Bitset | BitsetSp | Action | Notes |
|---|---|---|---|
| `block(i)` (value, ref) | `block(i)` (value, ref) | DIFF | dense index = block id; sparse index = position in `vBB_`. Same name, different meaning: document, do not unify |
| `bitset()` -> `DenseBlockVec` | `bitset()` -> `SparseBlockVec` | DIFF | storage type differs by design |
| none | `pBlock`, `find_block*`, `begin/end/cbegin/cend`, `sort`, `from_num_blocks` | DIFF | sparse-only; stays |

## 3. Bit queries and scanning

| Bitset | BitsetSp | Action | Cost / notes |
|---|---|---|---|
| `msb()`, `lsb()` | same (+ `(int& block)` overloads) | OK | O(1) |
| `next_bit(bit)`, `prev_bit(bit)` `noexcept` | same, not `noexcept` | ALIGN | add `noexcept` only if no throwing call. Slow at 10% occupancy in the baseline benchmark (step 7) |
| `count()`, `popcn64()` | same | OK | O(B) |
| `popcn64(first, last)` | `count(first,last)`, `popcn64(first[,last])` | OK | |
| `is_bit(bit)` `noexcept` | `is_bit(bit)` | ALIGN | `noexcept`; O(log B). Slow in the benchmark (step 7) |
| `test(bit)` alias | none | ADD | inline forwarder to `is_bit` |
| `is_empty()` virtual | `is_empty()` non-virtual | OK | non-virtual is what we want for the hot path |
| `is_singleton()` | none | ADD | O(1)-O(B), early exit; `B==1 && single-bit block` (blocks are non-empty only if erased blocks are removed: verify) |
| `is_disjoint(rhs)` `noexcept` | `is_disjoint(rhs)` | OK/ALIGN | add `noexcept` if possible; merge with early exit |
| `is_disjoint_block(first,last,rhs)` | same | OK | |
| `find_first_common(rhs)` | none | ADD | merge pass with early exit; model it on `is_disjoint` (reuse its loop) |

## 4. Setting and erasing bits

| Bitset | BitsetSp | Action | Cost / notes |
|---|---|---|---|
| `set_bit(bit)` `noexcept` | `set_bit(bit)` | ALIGN | O(log B) search + O(B) insertion; benchmark slow spot |
| `set(bit)` alias | none | ADD | inline forwarder |
| `set_bit(first,last)` | same | OK | |
| `set_bit(const Bitset&)` | `set_bit(const BitsetSp&)` | OK | merge |
| `assign_bit(rhs)` | none | ADD | `*this = rhs` semantics (what `Bitset` does; check) |
| `set_bit(lastBit, rhs)` | `set_bit(first,last,rhs)` deleted | ALIGN | TODO in sparse code; low priority |
| `set_bit(const bit_indices&)` | none | ADD | loop of `set_bit`, or sort + merge for large lists |
| `erase_bit(bit)` | same | OK | |
| `clear(bit)` alias | none | ADD | forwarder to `erase_bit(bit)` |
| `erase_bit(first,last)` | same | OK | |
| `erase_bit()` returns `Bitset&` | returns `void` | ALIGN | return `*this` |
| `erase_all_bits()`, `clear_all()` | none | ADD | forwarders to `erase_bit()`; `Bitset::clear_all()` now takes no argument (fixed) |
| `erase_bit(rhs)` | same | OK | merge |
| `andnot_eq(rhs)` | none | ADD | forwarder to `erase_bit(rhs)` |
| `erase_bit(lhs, rhs)` (member, 2 args) | free `erase_bit(lhs, rhs, res)` | ALIGN | different shape; low priority |
| `set_block`, `erase_block` (ranges) | same | OK | |
| `reset_bit` (sparse) | none | DIFF | `reset_bit(bit)` = "THIS becomes the singleton"; no dense equivalent |
| none | `clear_bit(first,last)` | DIFF | marked experimental in the sparse header |

## 5. Masking

| Bitset | BitsetSp | Action | Notes |
|---|---|---|---|
| `&=`, `\|=`, `^=` | same | OK | merge passes; `\|=` allocates (benchmark, step 7) |
| `and_assign`, `or_assign` | none | ADD | inline forwarders |
| `AND_block`/`OR_block` member (first,last) | `AND_block(first, rhs)`; the (first,last) form is deleted | ALIGN | |
| free `AND(lhs, rhs, res)`, `AND(lhs, rhs)`, `OR(lhs, rhs, res)` | same | OK | |
| free `AND_block`, `OR_block` | `AND_block` only | ADD | `OR_block` |
| `AND(first,last,lhs,rhs,res)` | deleted | ALIGN | TODO in sparse code |
| `flip`, `flip_block` | none | EXCL | |
| `operator==`, `!=` | same | OK | |

## 6. Conversion and I/O

| Bitset | BitsetSp | Action | Notes |
|---|---|---|---|
| `extract(bit_indices&)` | `extract(bitpos_list&)` | OK | same type |
| `extract_set(bit_index_set&)` | none | ADD | |
| `extract_stack(FixedStack&)` | none | ADD | |
| `extract_array(int*, size_t&, rev)` | none | ADD | |
| `explicit operator bit_indices()`, `explicit operator bit_index_set()` | **implicit** `operator bitpos_list()` | ALIGN | making it explicit may break callers; check usage first (step 4) |
| `to_string()` | `to_string() const` | OK | aligned: `Bitset::to_string()` is now `const` and has no trailing newline (it used the default `print` end of line) |
| `print(o, show_pc, endl)` | same | OK | |

## Summary

- **OK:** about 25 entries already match (constructors, copy/move, `msb`/`lsb`, `count`, `set_bit`/`erase_bit` families, `&=` / `|=` / `^=`, `==`, `print`, `extract`).
- **ADD (no behaviour change, step 3):** `test`, `set`, `clear`, `clear_all`, `erase_all_bits`, `and_assign`, `or_assign`, `andnot_eq`, `assign_bit`, `is_singleton`, `find_first_common`, `bit_capacity`, `init(nPop, bits)`, `set_bit(bit_indices)`, the `bit_indices` constructor, `make_bitset_sp*`, `OR_block`, `extract_set`, `extract_stack`, `extract_array`.
- **ALIGN (step 4, touches signatures):** `erase_bit()` return type, `noexcept` on query functions, `num_blocks()` type, the implicit cast, the deleted range overloads.
- **DIFF (documented, not unified):** the block accessors, sparse-only storage functions, `reset_bit`, `clear_bit`.

## Questions to settle

1. ~~Should the implicit `operator bitpos_list()` become explicit?~~ Checked: making it explicit compiles with no errors across the whole project (library, tests, graph code), so it is safe to align in step 4.
2. ~~Can `vBB_` hold zero blocks?~~ Yes: `erase_bit(bit)` and `&=` clear bits or zero `bb_` without removing the block. `is_empty()` already scans for this. So `size()` can exceed the number of non-zero blocks, and `is_singleton` must count bits over blocks (skipping zero ones), not test `B==1`.

## Progress

**Step 3 done (2026-10-13):** added to `BitsetSp`: `test`, `set`, `clear`, `clear_all`, `erase_all_bits`, `and_assign`, `or_assign`, `andnot_eq`, `bit_capacity`, `assign_bit`, `set_bit(bitpos_list)`, `is_singleton`, `find_first_common`, `extract_set`, `extract_stack`, `extract_array`, `init(nPop, bits)`, the `BitsetSp(bitpos_list)` constructor and `make_bitset_sp`. Tests: `BitsetSp_api.*` in `test_bitset_sparse.cpp`. Not done: free `OR_block` (the dense one is a templated range operation; needs its own design), galloping search in `find_first_common`.

**Step 4 done (2026-10-13):** `erase_bit()` now returns `BitsetSp&` (and is `noexcept`); `next_bit`, `prev_bit`, `is_bit`, `is_empty`, `is_disjoint`, `is_singleton`, `find_first_common`, `test` are `noexcept`; `operator bitpos_list()` is `explicit`. Left as they are: `num_blocks()` type (`size_t` vs `int`, would ripple through callers), `set_bit`/`erase_bit(bit)` stay without `noexcept` (they may allocate), the deleted range overloads (`set_bit(first,last,rhs)`, `AND(first,last,...)`, `AND_block(first,last,rhs)`), and the free `erase_bit(lhs, rhs)` shape.
