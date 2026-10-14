# BITSCAN API overview

The main types and functions of BITSCAN, grouped by task. This page is a map, not a full reference: for every signature and parameter, build the Doxygen reference (see [docs/README.md](../README.md)) or read the headers in [src/bitscan](../../src/bitscan). A guide with examples is in the [BITSCAN README](../../src/bitscan/README.md).

Include everything with `#include "BitGraph/bitscan/bitscan.h"`. All names are in namespace `bitgraph`.

**Conventions.** Bit positions are zero-based. Ranges are **closed**: both ends are included. Functions that modify a bitset return `*this`, so calls chain. `BBObject::noBit` (also `bitarray::noBit`) is the value returned when there is no bit.

## Types

| Alias | Class | Description |
|---|---|---|
| `bitarray` | `BBScan` | dense bitset with persistent scanning (recommended) |
| `simple_bitarray` | `Bitset` | dense bitset, without the scanning state |
| `sparse_bitarray` | `BBScanSp` | sparse bitset with persistent scanning (recommended for large, mostly empty sets) |
| `simple_sparse_bitarray` | `BitsetSp` | sparse bitset, without the scanning state |
| `watched_bitarray` | `experimental::BBSentinel` | **experimental**: operations restricted to the range of non-zero blocks |

`BBObject` is the common base class: it defines the shared type aliases (`bit_index_t`, `block_index_t`, `bit_indices`, ...), the constants `noBit` and `npos`, and the scan type constants `NON_DESTRUCTIVE`, `NON_DESTRUCTIVE_REVERSE`, `DESTRUCTIVE` and `DESTRUCTIVE_REVERSE`. `BBObject` is also available under the alias `bbo`.

## Creating and sizing

| Function | Purpose |
|---|---|
| `Bitset(nPop)`, `BitsetSp(nPop)` | construct a bitset with room for `nPop` bits, all clear |
| `reset(...)`, `init(...)` | reinitialize a bitset, discarding its contents |
| `shrink_to_fit()` | release unused memory |
| `num_blocks()` | number of 64-bit blocks |
| `bit_capacity()` | number of bits the allocated blocks can hold |
| `Bitset::empty(nPop)`, `Bitset::full(nPop)` | factories for an empty and a full dense bitset |
| `Bitset::from_positions(...)`, `Bitset::from_set_bits(...)` | factories from a list of bit positions |
| `make_bitset`, `make_bitset_full`, `make_BBScan`, `make_bitset_sp` | free factory functions |

## Bits and ranges

| Function | Purpose |
|---|---|
| `set_bit(i)`, `set(i)` | set bit `i` |
| `set_range(first, last)` | set the closed range `[first, last]` |
| `erase_bit(i)`, `clear(i)` | clear bit `i` |
| `clear_range(first, last)` | clear the closed range `[first, last]` |
| `clear_all()`, `erase_all_bits()` | clear every bit |
| `is_bit(i)`, `test(i)` | test bit `i` |
| `flip()` | flip every bit |
| `assign_bit(other)` | overwrite this bitset with `other` (equivalent to assignment) |

## Set operations

| Function | Purpose |
|---|---|
| `operator&=`, `and_assign` | intersection |
| `operator\|=`, `or_assign` | union |
| `operator^=` | symmetric difference |
| `andnot_eq(rhs)` | difference: remove the bits of `rhs` |
| `AND(lhs, rhs)`, `OR(lhs, rhs)` | free functions that return the intersection or union of two bitsets; overloads write the result into an output bitset and cover block ranges |
| `erase_bit(lhs, rhs, res)` | free function: `res` receives the bits of `lhs` that are not in `rhs` |
| `AND_block`, `OR_block` | the same operations on a block range |

## Queries

| Function | Purpose |
|---|---|
| `count()` | number of set bits |
| `is_empty()` | no bit is set |
| `is_singleton()` | exactly one bit is set |
| `intersects(other)` | the two bitsets share at least one bit |
| `is_disjoint(other)` | the two bitsets share no bit |
| `find_first_common(other)` | first bit in the intersection, or `noBit` |
| `find_singleton(...)` | detect a single set bit in a range |
| `lsb()`, `msb()` | lowest and highest set bit |

## Scanning

| Name | Purpose |
|---|---|
| `bitarray::scan`, `scanR`, `scanD`, `scanDR` | scanner objects: forward, reverse, destructive forward, destructive reverse. Same on `sparse_bitarray`. |
| `next_bit()`, `prev_bit()` | next bit of the scan in the forward and reverse direction (on the scanner object) |
| `init_scan(scan_type)` | configure the scan stored in a `BBScan`/`BBScanSp`, using one of the `BBObject` scan type constants |
| `next_bit(bit)`, `prev_bit(bit)` | stateless successor and predecessor of `bit` (pass `noBit` to get the first one) |
| `view::scan`, `view::scanR`, `view::scanD`, `view::scanDR` | independent views with their own cursor (`view::sparse::*` for sparse bitsets) |
| `view::ForwardView<BitsetT>` and friends | the generic form of the views, with the bitset type as a template argument |

Only one persistent scan should be active on a given bitset at a time; use views when you need several.

## Extracting and printing

| Function | Purpose |
|---|---|
| `extract(vector)`, `extract_set(set)` | copy the set bits into a container |
| `to_vector(bitset)` | free function returning a `std::vector<int>` of the set bits |
| `extract_first_k(k, bitset, vec)` | copy the first `k` set bits into `vec` and return how many were extracted |
| `to_string()`, `print()` | readable text with the population count |
| `operator<<` | stream a bitset |

## Block access

| Function | Purpose |
|---|---|
| `block(i)` | the 64-bit block `i` (dense bitsets; on sparse bitsets see below) |
| `set_block(...)`, `erase_block(...)` | modify blocks or block ranges |
| `is_empty_block`, `is_disjoint_block`, `find_first_common_block` | block-range versions of the queries |

## Sparse bitsets (`BitsetSp`, `BBScanSp`)

The set operations and queries above are available on the sparse types. In addition:

| Function | Purpose |
|---|---|
| `find_block(index)` | find a stored block; returns 0 if it is not stored |
| `block(k)`, `pBlock(k)` | the `k`-th **stored** block (its position in the storage, not its block index); `pBlock` also gives the block index |
| `begin()`, `end()`, `cbegin()`, `cend()` | iterate over the stored blocks |
| `clear_bit(first, last)` | clear the closed range `[first, last]` and remove the blocks that become empty |
| `BitsetSp::from_num_blocks(n)` | create an empty bitset from a number of blocks |
| `sort()` | sort the stored blocks by block index |

## Operations on a single 64-bit word: namespace `bblock`

| Function | Purpose |
|---|---|
| `lsb(w)`, `msb(w)` | index of the lowest and highest set bit |
| `popc64(w)`, `count(w)` | population count |
| `is_bit(w, i)` | test bit `i` of the word |
| `MASK_BIT(i)` | a word with only bit `i` set |
| `MASK_1(low, high)`, `MASK_0(low, high)` | masks of ones or zeros over a closed range |
| `MASK_1_LOW`, `MASK_1_HIGH`, `MASK_0_LOW`, `MASK_0_HIGH` | masks below or above an index |
| `trim_low(w, i)`, `trim_high(w, i)` | clear the bits to one side of an index |
| `copy(first, last, source, dest)` | copy the closed bit range `[first, last]` of `source` into `dest` |
| `print(w)` | stream a word and its population count |

## Utilities

| Function | Purpose |
|---|---|
| `random_bitblock(p)` | a random 64-bit word with density `p` of ones |
