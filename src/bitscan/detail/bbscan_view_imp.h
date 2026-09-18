/**
 * @file bbscan_view_impl.h
 * @brief Inline and template implementations for BITSCAN scan views.
 *
 * This file implements the scan-view classes declared in bbscan_view.h.
 * Scan views own an independent cursor, allowing multiple non-destructive
 * views to traverse the same bitset independently.
 *
 * For performance-critical paths that can use a cursor stored directly in the
 * bitset, prefer the persistent scanning interface provided by BBScan.
 *
 * This implementation header is included at the end of bbscan_view.h and
 * should not be included directly by user code.
 *
 * @date Created: 11/09/2026
 * @author P. San Segundo
 */

#ifndef _BITGRAPH_BITSCAN_BBSCAN_VIEW_IMP_H_
#define _BITGRAPH_BITSCAN_BBSCAN_VIEW_IMP_H_


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
				vBB[i] &= ~Tables::mask[posInBB];
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
				vBB[i] &= ~Tables::mask[posInBB];
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





#endif // _BITGRAPH_BITSCAN_BBSCAN_VIEW_IMP_H_






