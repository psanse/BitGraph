 /**
  * @file bbscan_view_imp.h file
  * @brief header file which implements the BBScanView class from the BITSCAN library (see bbscan_view.h).
  *		   Manages efficient bitscanning as a separate view. For hot paths use BBScan.
  * @created 11/09/2026
  * @author pss
  * 
  **/

#ifndef _BITGRAPH_BITSCAN_BBSCAN_VIEW_IMP_H_
#define _BITGRAPH_BITSCAN_BBSCAN_VIEW_IMP_H_


namespace bitgraph {

	inline
	int BBScanView::next_bit_destructive_impl() {

		if (!has_valid_cursor(MASK_LIM)) {
			return BBObject::noBit;
		}

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

	inline
	int BBScanView::next_bit_impl() {

		if (!has_valid_cursor(MASK_LIM)) {
			return BBObject::noBit;
		}

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

	inline
	int BBScanView::prev_bit_impl() {

		if (!has_valid_cursor(WORD_SIZE)) {
			return BBObject::noBit;
		}

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

	inline
	int BBScanView::prev_bit_destructive_impl() {

		if (!has_valid_cursor(WORD_SIZE)) {
			return BBObject::noBit;
		}

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

	inline
	int BBScanView::next_bit() {
		switch (scan_type_) {
		case BBObject::NON_DESTRUCTIVE:
		case BBObject::NON_DESTRUCTIVE_REVERSE:
			return next_bit_impl();
		case BBObject::DESTRUCTIVE:
		case BBObject::DESTRUCTIVE_REVERSE:
			return next_bit_destructive_impl();
		default:
			return BBObject::noBit;
		}
	}

	inline
	int BBScanView::prev_bit() {
		switch (scan_type_) {
		case BBObject::NON_DESTRUCTIVE:
		case BBObject::NON_DESTRUCTIVE_REVERSE:
			return prev_bit_impl();
		case BBObject::DESTRUCTIVE:
		case BBObject::DESTRUCTIVE_REVERSE:
			return prev_bit_destructive_impl();
		default:
			return BBObject::noBit;
		}
	}

	inline
	int BBScanView::init_scan(BBObject::scan_types sct) noexcept {
		if (sct < BBObject::NON_DESTRUCTIVE || sct > BBObject::DESTRUCTIVE_REVERSE) {
			return -1;
		}
		scan_type_ = sct;

		if (bitset_.num_blocks() <= 0) {
			set_scan_block(BBObject::noBit);
			set_scan_bit(MASK_LIM);
			return 0;
		}

		switch (sct) {
		case BBObject::NON_DESTRUCTIVE:
			set_scan_block(0);
			set_scan_bit(MASK_LIM);
			break;
		case BBObject::NON_DESTRUCTIVE_REVERSE:
			set_scan_block(bitset_.num_blocks() - 1);
			set_scan_bit(WORD_SIZE);
			break;
		case BBObject::DESTRUCTIVE:
			set_scan_block(0);
			set_scan_bit(MASK_LIM);
			break;
		case BBObject::DESTRUCTIVE_REVERSE:
			set_scan_block(bitset_.num_blocks() - 1);
			set_scan_bit(WORD_SIZE);
			break;
		default:
			return -1;
		}

		return 0;
	}

	inline
	int BBScanView::init_scan(int firstBit, BBObject::scan_types sct) noexcept {
		if (sct < BBObject::NON_DESTRUCTIVE || sct > BBObject::DESTRUCTIVE_REVERSE) {
			return -1;
		}
		scan_type_ = sct;

		if (firstBit == BBObject::noBit) {
			return init_scan(sct);
		}
		if (firstBit < 0 || firstBit >= bitset_.num_blocks() * WORD_SIZE) {
			set_scan_block(BBObject::noBit);
			set_scan_bit(MASK_LIM);
			return -1;
		}
				
		set_scan_block(WDIV(firstBit));
		set_scan_bit(WMOD(firstBit));

		return 0;
	}

	template <BBObject::scan_types ScanType>
	inline
	int BBScanViewT<ScanType>::next_bit_destructive_impl() {

		if (!has_valid_cursor(MASK_LIM)) {
			return BBObject::noBit;
		}

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

	template <BBObject::scan_types ScanType>
	inline
	int BBScanViewT<ScanType>::next_bit_impl() {

		if (!has_valid_cursor(MASK_LIM)) {
			return BBObject::noBit;
		}

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

	template <BBObject::scan_types ScanType>
	inline
	int BBScanViewT<ScanType>::prev_bit_impl() {

		if (!has_valid_cursor(WORD_SIZE)) {
			return BBObject::noBit;
		}

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

	template <BBObject::scan_types ScanType>
	inline
	int BBScanViewT<ScanType>::prev_bit_destructive_impl() {

		if (!has_valid_cursor(WORD_SIZE)) {
			return BBObject::noBit;
		}

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

	template <BBObject::scan_types ScanType>
	inline
	int BBScanViewT<ScanType>::next_bit() {
		return this->next_bit_dispatch(std::integral_constant<bool, is_destructive_scan()>{});
	}

	template <BBObject::scan_types ScanType>
	inline
	int BBScanViewT<ScanType>::prev_bit() {
		return this->prev_bit_dispatch(std::integral_constant<bool, is_destructive_scan()>{});
	}

	template <BBObject::scan_types ScanType>
	inline
	int BBScanViewT<ScanType>::init_scan() noexcept {

		if (bitset_.num_blocks() <= 0) {
			set_scan_block(BBObject::noBit);
			set_scan_bit(MASK_LIM);
			return 0;
		}

		return this->init_scan_dispatch(std::integral_constant<bool, is_reverse_scan()>{});
	}

	template <BBObject::scan_types ScanType>
	inline
	int BBScanViewT<ScanType>::init_scan(int firstBit) noexcept {

		if (firstBit == BBObject::noBit) {
			return init_scan();
		}
		if (firstBit < 0 || firstBit >= bitset_.num_blocks() * WORD_SIZE) {
			set_scan_block(BBObject::noBit);
			set_scan_bit(MASK_LIM);
			return -1;
		}

		set_scan_block(WDIV(firstBit));
		set_scan_bit(WMOD(firstBit));

		return 0;
	}

	
}//namespace bitgraph





#endif // _BITGRAPH_BITSCAN_BBSCAN_VIEW_IMP_H_






