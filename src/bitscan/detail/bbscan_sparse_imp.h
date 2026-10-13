/**
 * @file bbscan_sparse_imp.h
 * @brief Implementation of the BBScanSp class for sparse bitsets.
 *
 * Provides efficient bit-scanning operations on sparse bitsets,
 * including forward, reverse, destructive, and non-destructive scanning.
 *
 * @author pss
 * @date Created 2012
 * @date Last updated 25/02/2025
 *
 * @todo Refactor and extend unit tests (25/02/2025).
 */

#ifndef BITGRAPH_BITSCAN_BBSCAN_SPARSE_IMP_H
#define BITGRAPH_BITSCAN_BBSCAN_SPARSE_IMP_H


namespace bitgraph {

	inline int BBScanSp::next_bit()
	{

		if (!has_valid_cursor()) {
			return BBObject::noBit;
		}

		Ul posInBB;

		//search for next bit in the last block
		if (_BitScanForward64(&posInBB, vBB_[scan_.bbi_].bb_ & Tables::mask_high[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;

			return (posInBB + static_cast<int>(WMUL(vBB_[scan_.bbi_].idx_)));

		}
		else {

			//Searches for next bit in the remaining blocks
			for (auto i = scan_.bbi_ + 1; i < (int)vBB_.size(); ++i) {
				if (_BitScanForward64(&posInBB, vBB_[i].bb_)) {
					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
				}
			}
		}

		return BBObject::noBit;

	}

	inline int BBScanSp::next_bit(BBScanSp& bitset)
	{
		if (!has_valid_cursor()) {
			return BBObject::noBit;
		}

		Ul posInBB;

		//search for next bit in the last block
		if (_BitScanForward64(&posInBB, vBB_[scan_.bbi_].bb_ & Tables::mask_high[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;

			//delete the bit from the input bitset
			bitset.erase_bit(posInBB);

			return (posInBB + static_cast<int>(WMUL(vBB_[scan_.bbi_].idx_)));

		}
		else {

			//Searches for next bit in the remaining blocks
			for (auto i = scan_.bbi_ + 1; i < (int)	vBB_.size(); ++i) {
				if (_BitScanForward64(&posInBB, vBB_[i].bb_)) {
					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					//delete the bit from the input bitset
					bitset.erase_bit(posInBB);

					return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
				}
			}
		}

		return BBObject::noBit;
	}

	inline int BBScanSp::prev_bit() 
	{

		if (!has_valid_cursor()) {
			return BBObject::noBit;
		}

		Ul posInBB;

		//searches for previous bit in the last scanned block
		if (_BitScanReverse64(&posInBB, vBB_[scan_.bbi_].bb_ & Tables::mask_low[scan_.pos_])) {
			//stores the current bit for next call
			scan_.pos_ = posInBB;

			return (posInBB + static_cast<int>(WMUL(vBB_[scan_.bbi_].idx_)));
		}
		else {
			//Searches for previous bit in the remaining blocks
			for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, vBB_[i].bb_)) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
				}
			}
		}

		return BBObject::noBit;

	}

	inline int BBScanSp::prev_bit(BBScanSp& bitset)
	{

		if (!has_valid_cursor()) {
			return BBObject::noBit;
		}

		Ul posInBB;

		//searches for previous bit in the last scanned block
		if (_BitScanReverse64(&posInBB, vBB_[scan_.bbi_].bb_ & Tables::mask_low[scan_.pos_])) {
			//stores the current bit for next call
			scan_.pos_ = posInBB;

			//delete the bit from the input bitset
			bitset.erase_bit(posInBB);

			return (posInBB + static_cast<int>(WMUL(vBB_[scan_.bbi_].idx_)));
		}
		else {
			//Searches for previous bit in the remaining blocks
			for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, vBB_[i].bb_)) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					//delete the bit from the input bitset
					bitset.erase_bit(posInBB);

					return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
				}
			}
		}

		return BBObject::noBit;
	}


	inline int BBScanSp::next_bit_del() {

		if (!has_valid_cursor()) {
			return BBObject::noBit;
		}

		Ul posInBB;

		for (auto i = scan_.bbi_; i < (int)vBB_.size(); ++i) {

			if (_BitScanForward64(&posInBB, vBB_[i].bb_)) {

				//stores the current block
				scan_.bbi_ = i;

				//deletes the current bit before returning
				vBB_[i].bb_ &= ~Tables::mask[posInBB];

				return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
			}

		}

		return BBObject::noBit;

	}

	inline int BBScanSp::next_bit_del(BBScanSp& bitset)
	{
		if (!has_valid_cursor()) {
			return BBObject::noBit;
		}

		Ul posInBB;

		for (auto i = scan_.bbi_; i < (int)vBB_.size(); ++i) {

			if (_BitScanForward64(&posInBB, vBB_[i].bb_)) {

				//stores the current block
				scan_.bbi_ = i;

				//deletes the current bit before returning
				vBB_[i].bb_ &= ~Tables::mask[posInBB];

				//delete the bit from the input bitset
				bitset.erase_bit(posInBB);

				return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
			}

		}

		return BBObject::noBit;
	}

	inline int BBScanSp::prev_bit_del() {

		if (!has_valid_cursor()) {
			return BBObject::noBit;
		}

		Ul posInBB;

		for (int i = scan_.bbi_; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i].bb_)) {

				//stores the current block for the next call
				scan_.bbi_ = i;

				//deletes the current bit from the bitset before returning
				vBB_[i].bb_ &= ~Tables::mask[posInBB];

				return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
			}
		}

		return BBObject::noBit;
	}

	inline int BBScanSp::prev_bit_del(BBScanSp& bitset)
	{
		if (!has_valid_cursor()) {
			return BBObject::noBit;
		}

		Ul posInBB;

		for (int i = scan_.bbi_; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i].bb_)) {

				//stores the current block for the next call
				scan_.bbi_ = i;

				//deletes the current bit from the bitset before returning
				vBB_[i].bb_ &= ~Tables::mask[posInBB];

				//delete the bit from the input bitset
				bitset.erase_bit(posInBB);

				return (posInBB + static_cast<int>(WMUL(vBB_[i].idx_)));
			}
		}

		return BBObject::noBit;
	}

	inline
		void BBScanSp::init_scan(scan_types sct)  noexcept {

		//necessary check since sparse bitstrings have empty semantics (i.e. sparse graphs)
		if (vBB_.empty()) {
			invalidate_scan();
			return;			
		}			

		switch (sct) {
		case NON_DESTRUCTIVE:
			scan_block(0);
			scan_bit(MASK_LIM);
			break;
		case NON_DESTRUCTIVE_REVERSE:
			scan_block(static_cast<int>(vBB_.size()) - 1);
			scan_bit(WORD_SIZE);
			break;
		case DESTRUCTIVE:
			scan_block(0);
			break;
		case DESTRUCTIVE_REVERSE:
			scan_block(static_cast<int>(vBB_.size()) - 1);
			break;
		default:
			invalid_scan_type();	
		}
		
	}

	inline
		void BBScanSp::init_scan(int firstBit, scan_types sct) noexcept {

		//necessary check 
		if (vBB_.empty()) {
			invalidate_scan();
			return;
		}

		//special case - first bitscan
		if (firstBit == BBObject::noBit) {
			init_scan(sct);
			return;
		}

		//determine the index of the starting block (not its ID)
		auto bbL = WDIV(firstBit);

		////////////////////////////////////////////////////
		pair<bool, int> p = find_block_pos(bbL);
		///////////////////////////////////////////////////

		//no blocks with index greater or equal to bbL, nothing to scan
		if (p.second == BBObject::noBit) {
			invalidate_scan();
			return;
		}

		switch (sct) {
		case NON_DESTRUCTIVE:
		case NON_DESTRUCTIVE_REVERSE:
			scan_block(p.second);
			(p.first) ? scan_bit(firstBit - WMUL(bbL)) : scan_bit(MASK_LIM);
			break;
		case DESTRUCTIVE:
		case DESTRUCTIVE_REVERSE:
			//scan_block(p.second);
			LOG_ERROR("destructive scan type currently not available in BBScanSp::init_scan...exiting");
			std::exit(EXIT_FAILURE);
			//throw BitScanError("incorrect destructive scan type in BBScanSp::init_scan");		
			break;
		default:
			invalid_scan_type();
		}

	}


	////////////////////////////////
	//
	// DEPRECATED
	//
	//////////////////////////////////
	//inline 
	//int BBScanSp::next_bit_del(int& block_index) {
	//	////////////////////////////
	//	//
	//	// date: 23/3/12
	//	// Destructive bitscan for sparse bitstrings using intrinsics
	//	//
	//	// COMMENTS
	//	// 1-Requires previous assignment scan_.bbi=0 
	//
	//	unsigned long posbb;
	//
	//	for (int i = scan_.bbi_; i < vBB_.size(); i++) {
	//		if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
	//			scan_.bbi_ = i;
	//			block_index = vBB_[i].idx_;
	//			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
	//			return (posbb + WMUL(vBB_[i].idx_));
	//		}
	//	}
	//
	//	return EMPTY_ELEM;
	//}
	//
	//
	//inline int BBScanSp::next_bit_del(int& block_index, BBScanSp& bbN_del) {
	//	////////////////////////////
	//	//
	//	// date: 06/07/2019 (EXPERIMENTAL)
	//	// Destructive bitscan for sparse bitstrings using intrinsics
	//	//
	//	// COMMENTS
	//	// 1-added for compatibility with BBIntrin
	//	// 1-Requires previous assignment scan_.bbi=0 
	//
	//	unsigned long posbb;
	//
	//	for (int i = scan_.bbi_; i < vBB_.size(); i++) {
	//		if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
	//			scan_.bbi_ = i;
	//			block_index = vBB_[i].idx_;
	//			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
	//			bbN_del.vBB_[i].bb_ &= ~Tables::mask[posbb];
	//			return (posbb + WMUL(vBB_[i].idx_));
	//		}
	//	}
	//
	//	return EMPTY_ELEM;
	//}
	//
	//inline int BBScanSp::next_bit_del_pos(int& posBB) {
	//	////////////////////////////
	//	//
	//	// date: 29/10/14
	//	// Destructive bitscan which returns the position of the bitblock scanned in the collection
	//	// (not the index attribute)
	//	//
	//	// COMMENTS
	//	// 1-Requires previous assignment scan_.bbi=0 
	//
	//	unsigned long posbb;
	//
	//	for (int i = scan_.bbi_; i < vBB_.size(); i++) {
	//		if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
	//			posBB = scan_.bbi_ = i;
	//			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
	//			return (posbb + WMUL(vBB_[i].idx_));
	//		}
	//	}
	//
	//	return EMPTY_ELEM;
	//}

	//inline int BBScanSp::next_bit(int& block_index) {
	//	////////////////////////////
	//	// date:5/9/2014
	//	// non destructive bitscan for sparse bitstrings using intrinsics
	//	// caches index in the collection and pos inside the bitblock
	//	//
	//	// comments
	//	// 1-require previous assignment scan_.bbi=0 and scan_.pos_=mask_lim
	//
	//	unsigned long posbb;
	//
	//	//search for next bit in the last block
	//	if (_BitScanForward64(&posbb, vBB_[scan_.bbi_].bb_ & Tables::mask_high[scan_.pos_])) {
	//		scan_.pos_ = posbb;
	//		block_index = vBB_[scan_.bbi_].idx_;
	//		return (posbb + WMUL(vBB_[scan_.bbi_].idx_));
	//	}
	//	else {											//search in the remaining blocks
	//		for (int i = scan_.bbi_ + 1; i < vBB_.size(); i++) {
	//			if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
	//				scan_.bbi_ = i;
	//				scan_.pos_ = posbb;
	//				block_index = vBB_[i].idx_;
	//				return (posbb + WMUL(vBB_[i].idx_));
	//			}
	//		}
	//	}
	//
	//	return EMPTY_ELEM;
	//}

	//inline int BBScanSp::prev_bit_del(int& bb_index) {
	//	////////////////////////////
	//	//
	//	// date: 23/3/12
	//	// Destructive bitscan for sparse bitstrings using intrinsics
	//	//
	//	// COMMENTS
	//	// 1-Requires previous assignment scan_.bbi=number of bitblocks-1
	//
	//	unsigned long posbb;
	//
	//	for (int i = scan_.bbi_; i >= 0; i--) {
	//		if (_BitScanReverse64(&posbb, vBB_[i].bb_)) {
	//			scan_.bbi_ = i;
	//			bb_index = vBB_[i].idx_;
	//			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
	//			return (posbb + WMUL(vBB_[i].idx_));
	//		}
	//	}
	//
	//	return BBObject::noBit;
	//}

}//end namespace bitgraph



#endif // BITGRAPH_BITSCAN_BBSCAN_SPARSE_IMP_H