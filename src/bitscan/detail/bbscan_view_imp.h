/**
 * @file bbscan_view_imp.h
 * @brief Inline template implementation of BITSCAN scan views.
 *
 * @details
 * This implementation fragment defines the operations declared by
 * @c bitgraph::BBScanViewT. It performs forward and reverse searches over
 * dense bitblock storage and dispatches at compile time between
 * non-destructive and destructive scan paths. Destructive paths clear each
 * returned bit from the referenced bitset; non-destructive paths update only
 * the view's independent cursor.
 *
 * Empty bitsets are represented by an invalid cursor and return
 * @c BBObject::noBit when scanned. Valid scans retain the current block and
 * bit position so that subsequent calls continue from the previously found
 * bit in the configured direction.
 *
 * This file is included at the end of @c bbscan_view.h because the definitions
 * are templates. It must not be included directly by application code.
 * For a scan cursor stored inside the bitset itself, use @c BBScan instead.
 *
 * @author P. San Segundo
 * @date Created: 11 September 2026
 *
 * @see ../bbscan_view.h
 * @see ../bbscan.h
 * @warning Destructive implementations modify the referenced bitset and are
 *          incompatible with overlapping scans on the same storage.
 */

#ifndef BITGRAPH_BITSCAN_BBSCAN_VIEW_IMP_H
#define BITGRAPH_BITSCAN_BBSCAN_VIEW_IMP_H


namespace bitgraph {
		
	template <class BitsetT, BBObject::scan_types ScanType>
	inline
	int BBScanViewT<BitsetT, ScanType>::next_bit_destructive_impl() {
	
		auto& vBB = bitset_.bitset();
		Ul posInBB;

		for (auto i = scan_.bbi_; i < bitset_.num_blocks(); ++i) {
			const BITBOARD candidates = (i == scan_.bbi_)
				? vBB[i] & Tables::mask_high[scan_.pos_]
				: vBB[i];
			if (_BitScanForward64(&posInBB, candidates)) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				vBB[i] &= ~(BITBOARD(1) << posInBB);
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	template <class BitsetT, BBObject::scan_types ScanType>
	inline
	int BBScanViewT<BitsetT, ScanType>::next_bit_impl() {
		
		auto& vBB = bitset_.bitset();
		Ul posInBB;

		if (_BitScanForward64(&posInBB, vBB[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {
			scan_.pos_ = posInBB;
			return (posInBB + WMUL(scan_.bbi_));
		}

		for (auto i = scan_.bbi_ + 1; i < bitset_.num_blocks(); ++i) {
			if (_BitScanForward64(&posInBB, vBB[i])) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	template <class BitsetT, BBObject::scan_types ScanType>
	inline
	int BBScanViewT<BitsetT, ScanType>::prev_bit_impl() {
		
		auto& vBB = bitset_.bitset();
		Ul posInBB;

		if (_BitScanReverse64(&posInBB, vBB[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {
			scan_.pos_ = posInBB;
			return (posInBB + WMUL(scan_.bbi_));
		}

		for (auto i = scan_.bbi_ - 1; i >= 0; --i) {
			if (_BitScanReverse64(&posInBB, vBB[i])) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	template <class BitsetT, BBObject::scan_types ScanType>
	inline
	int BBScanViewT<BitsetT, ScanType>::prev_bit_destructive_impl() {
		
		auto& vBB = bitset_.bitset();
		Ul posInBB;

		for (auto i = scan_.bbi_; i >= 0; --i) {
			const BITBOARD candidates = (i == scan_.bbi_)
				? vBB[i] & Tables::mask_low[scan_.pos_]
				: vBB[i];
			if (_BitScanReverse64(&posInBB, candidates)) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				vBB[i] &= ~(BITBOARD(1) << posInBB);
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	template <class BitsetT, BBObject::scan_types ScanType>
	inline
	int BBScanViewT<BitsetT, ScanType>::next_bit() {

		assert(has_valid_cursor());

		return next_bit_dispatch(
			std::integral_constant<bool, is_reverse_scan()>{},
			std::integral_constant<bool, is_destructive_scan()>{}
		);		
	}

	template <class BitsetT, BBObject::scan_types ScanType>
	inline
	void BBScanViewT<BitsetT, ScanType>::init_scan() noexcept {

		if (bitset_.num_blocks() <= 0) {
			set_scan_block(BBObject::noBit);
			set_scan_bit(MASK_LIM);
			return;
		}

		init_scan_dispatch(
			std::integral_constant<bool, is_reverse_scan()>{}
		);
	}

	template <class BitsetT, BBObject::scan_types ScanType>
	inline
	void BBScanViewT<BitsetT, ScanType>::init_scan(int firstBit) noexcept {

		if (firstBit == BBObject::noBit) {
			init_scan();
			return;
		}
						
		if (firstBit < 0 || 
			static_cast<std::size_t>(firstBit) >= bitset_.bit_capacity()) {
			scan_initialization_error();
		}

		set_scan_block(WDIV(firstBit));
		set_scan_bit(WMOD(firstBit));
	}

	
}//namespace bitgraph






#endif // BITGRAPH_BITSCAN_BBSCAN_VIEW_IMP_H_	






