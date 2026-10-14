/**
 * @file bbscan_view_sparse.h
 * @brief Independent compile-time scan views for sparse bitsets.
 *
 * @details
 * This header defines @c bitgraph::BBScanViewSpT, the sparse counterpart of
 * @c BBScanViewT. Each view references a @c BitsetSp-compatible object and
 * owns its own scan cursor, allowing multiple non-destructive views to scan
 * the same sparse bitset independently. Forward, reverse, destructive, and
 * non-destructive modes are selected at compile time through
 * @c BBObject::scan_types.
 *
 * The cursor's block component is an index into the sparse block collection,
 * not the logical block number in the represented bitstring. Views perform no
 * allocation and the implementation is inline. Convenience aliases for
 * generic sparse bitsets and @c BBScanSp are available in @c bitgraph::view.
 *
 * An empty sparse bitset produces an exhausted scan. Passing a starting bit
 * outside the valid logical range terminates the program, consistent with the
 * class's fail-fast initialization policy. Destructive views clear returned
 * bits, but the sparse block collection must not be structurally modified
 * while any view is active.
 *
 * @author pss
 * @date Created: 14 October 2026
 *
 * @see bbscan_sparse.h
 * @see bbscan_view.h
 * @warning The referenced bitset must outlive the view. Do not insert or
 *          remove sparse blocks while a view is using it.
 */

#ifndef BITGRAPH_BITSCAN_BBSCAN_VIEW_SPARSE_H
#define BITGRAPH_BITSCAN_BBSCAN_VIEW_SPARSE_H

#include "bbscan_sparse.h"
#include <cstdio>
#include <exception>

namespace bitgraph {

	/**
	* @brief Compile-time-configurable view for scanning the set bits of a sparse bitset.
	* @tparam BitsetT sparse bitset type exposing bitset() (the sorted blocks), find_block_pos and bit_capacity
	* @tparam ScanType traversal direction and whether the scan is destructive
	* @details Semantics follow BBScanViewT: init_scan(firstBit) starts the scan after firstBit
	*          (before it, in reverse scans), firstBit = noBit starts from the beginning (end, in reverse scans).
	*          next_bit() returns BBObject::noBit when the scan is exhausted, and keeps returning it.
	*          An empty sparse bitset produces an exhausted scan.
	* @warning The program terminates if firstBit is outside [0, bit_capacity()).
	*/
	template <class BitsetT, BBObject::scan_types ScanType = BBObject::NON_DESTRUCTIVE>
	class BBScanViewSpT {

		static_assert(
			ScanType == BBObject::NON_DESTRUCTIVE ||
			ScanType == BBObject::NON_DESTRUCTIVE_REVERSE ||
			ScanType == BBObject::DESTRUCTIVE ||
			ScanType == BBObject::DESTRUCTIVE_REVERSE,
			"Invalid BBScanViewSpT scanning mode");

	public:
		explicit BBScanViewSpT(BitsetT& bitset, int firstBit = BBObject::noBit) noexcept : bitset_(bitset) { init_scan(firstBit); }
		~BBScanViewSpT() = default;

		/** @brief Initializes a scan over the complete bitset. An empty bitset produces an exhausted scan. */
		void init_scan() noexcept {
			const int n = static_cast<int>(bitset_.bitset().size());
			if (n == 0) {
				scan_.bbi_ = BBObject::noBit;
				scan_.pos_ = MASK_LIM;
			}
			else if (is_reverse_scan()) {
				scan_.bbi_ = n - 1;
				scan_.pos_ = WORD_SIZE;
			}
			else {
				scan_.bbi_ = 0;
				scan_.pos_ = MASK_LIM;
			}
		}

		/** @brief Initializes a scan at firstBit (excluded). BBObject::noBit is equivalent to init_scan(). */
		void init_scan(int firstBit) noexcept {

			if (firstBit == BBObject::noBit) {
				init_scan();
				return;
			}
			if (firstBit < 0 || static_cast<std::size_t>(firstBit) >= bitset_.bit_capacity()) {
				scan_initialization_error();
			}

			const int bbL = WDIV(firstBit);
			const std::pair<bool, int> p = bitset_.find_block_pos(bbL);	//position of the first block with index >= bbL (noBit if none)

			if (!is_reverse_scan()) {
				if (p.second == BBObject::noBit) {
					scan_.bbi_ = BBObject::noBit;						//nothing after firstBit
					return;
				}
				scan_.bbi_ = p.second;
				scan_.pos_ = p.first ? static_cast<int>(firstBit - WMUL(bbL)) : MASK_LIM;
			}
			else if (p.first) {
				scan_.bbi_ = p.second;									//bits of the block below firstBit
				scan_.pos_ = static_cast<int>(firstBit - WMUL(bbL));
			}
			else {
				//block of firstBit absent: last block with a lower index, all of its bits
				const int pos = (p.second == BBObject::noBit) ? static_cast<int>(bitset_.bitset().size()) : p.second;
				if (pos == 0) {
					scan_.bbi_ = BBObject::noBit;						//nothing before firstBit
					return;
				}
				scan_.bbi_ = pos - 1;
				scan_.pos_ = WORD_SIZE;
			}
		}

		/**
		* @brief Next set bit according to the scan mode, BBObject::noBit if the scan is exhausted.
		* @details Destructive modes clear the returned bit in the referenced bitset.
		**/
		int next_bit() {

			auto& vBB = bitset_.bitset();
			const std::size_t size = vBB.size();

			//also false if the scan is exhausted at init (bbi_ = noBit = -1, a huge unsigned value)
			if (static_cast<std::size_t>(scan_.bbi_) >= size) {
				return BBObject::noBit;
			}

			Ul posInBB;

			if (!is_reverse_scan()) {
				for (std::size_t i = static_cast<std::size_t>(scan_.bbi_); i < size; ++i) {
					const BITBOARD cand = (static_cast<int>(i) == scan_.bbi_) ? (vBB[i].bb_ & Tables::mask_high[scan_.pos_]) : vBB[i].bb_;
					if (_BitScanForward64(&posInBB, cand)) {
						scan_.bbi_ = static_cast<int>(i);
						scan_.pos_ = static_cast<int>(posInBB);
						if (is_destructive_scan()) { vBB[i].bb_ &= ~Tables::mask[posInBB]; }
						return static_cast<int>(posInBB) + static_cast<int>(WMUL(vBB[i].idx_));
					}
				}
			}
			else {
				for (int i = scan_.bbi_; i >= 0; --i) {
					const BITBOARD cand = (i == scan_.bbi_) ? (vBB[i].bb_ & Tables::mask_low[scan_.pos_]) : vBB[i].bb_;
					if (_BitScanReverse64(&posInBB, cand)) {
						scan_.bbi_ = i;
						scan_.pos_ = static_cast<int>(posInBB);
						if (is_destructive_scan()) { vBB[i].bb_ &= ~Tables::mask[posInBB]; }
						return static_cast<int>(posInBB) + static_cast<int>(WMUL(vBB[i].idx_));
					}
				}
			}

			return BBObject::noBit;
		}

	private:
		static constexpr bool is_reverse_scan() noexcept {
			return (ScanType == BBObject::NON_DESTRUCTIVE_REVERSE || ScanType == BBObject::DESTRUCTIVE_REVERSE);
		}
		static constexpr bool is_destructive_scan() noexcept {
			return (ScanType == BBObject::DESTRUCTIVE || ScanType == BBObject::DESTRUCTIVE_REVERSE);
		}

		[[noreturn]]
		void scan_initialization_error() noexcept {
			std::fputs("BBScanViewSpT scan initialization failed\n", stderr);
			std::terminate();
		}

		BitsetT& bitset_;
		BBObject::scan_t scan_;
	};


	namespace view {

		template<class BitsetT> using ForwardViewSp = BBScanViewSpT<BitsetT, BBObject::NON_DESTRUCTIVE>;
		template<class BitsetT> using ReverseViewSp = BBScanViewSpT<BitsetT, BBObject::NON_DESTRUCTIVE_REVERSE>;
		template<class BitsetT> using DestructiveForwardViewSp = BBScanViewSpT<BitsetT, BBObject::DESTRUCTIVE>;
		template<class BitsetT> using DestructiveReverseViewSp = BBScanViewSpT<BitsetT, BBObject::DESTRUCTIVE_REVERSE>;

		// Convenience aliases for the sparse bitset implementations
		using ForwardSp = ForwardViewSp<BBScanSp>;
		using ReverseSp = ReverseViewSp<BBScanSp>;
		using DestructiveForwardSp = DestructiveForwardViewSp<BBScanSp>;
		using DestructiveReverseSp = DestructiveReverseViewSp<BBScanSp>;
	}

} //namespace bitgraph

#endif // BITGRAPH_BITSCAN_BBSCAN_VIEW_SPARSE_H
