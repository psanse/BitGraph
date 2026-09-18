/**
 * @file bbscan_impl.h
 * @brief Inline and template implementations for the BBScan class.
 *
 * This file implements the operations declared in bbscan.h. BBScan provides
 * persistent-cursor scanning optimized for performance-critical paths in
 * bit-parallel algorithms.
 *
 * This implementation header is included at the end of bbscan.h and should
 * not be included directly by user code.
 *
 * @date Created: 15/09/2026
 * @author P. San Segundo
 */

#ifndef _BITGRAPH_BBSCAN_IMP_H_
#define _BITGRAPH_BBSCAN_IMP_H_


namespace bitgraph {
		

	inline 
	int BBScan::next_bit_del()
	{

		assert(has_valid_cursor());

		Ul posInBB;

		for (auto i = scan_.bbi_; i < nBB_; ++i) {

			if (_BitScanForward64(&posInBB, vBB_[i])) {
				//stores the current block
				scan_.bbi_ = i;

				//deletes the current bit before returning
				vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}

		}

		return BBObject::noBit;
	}


	inline 
	int BBScan::next_bit_del(BBScan& bbN_del)
	{

		assert(has_valid_cursor());

		Ul posInBB;

		for (auto i = scan_.bbi_; i < nBB_; ++i) {

			if (_BitScanForward64(&posInBB, vBB_[i])) {
				//stores the current block and copies to output
				scan_.bbi_ = i;

				//deletes the current bit before returning
				vBB_[i] &= ~Tables::mask[posInBB];

				//erases from the bitset passed the scanned bit
				bbN_del.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}


	inline 
	int BBScan::next_bit()
	{

		assert(has_valid_cursor());

		Ul posInBB;

		//Search for next bit in the last scanned block
		if (_BitScanForward64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;									//current block has not changed, so not stored

			return (posInBB + WMUL(scan_.bbi_));

		}
		else {

			//Searches for next bit in the remaining blocks
			for (auto i = scan_.bbi_ + 1; i < nBB_; ++i) {
				if (_BitScanForward64(&posInBB, vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::noBit;
	}



	inline
	int BBScan::next_bit(BBScan& bitset)
	{

		assert(has_valid_cursor());

		Ul posInBB;

		//Search for next bit in the last scanned block
		if (_BitScanForward64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;									//current block has not changed, so not stored	

			//outputs the current block
			//block = scan_.bbi_;

			//deletes the bit from the input bitset
			bitset.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

			return (posInBB + WMUL(scan_.bbi_));
		}
		else {
			//Searches for next bit in the remaining blocks
			for (auto i = scan_.bbi_ + 1; i < nBB_; i++) {
				if (_BitScanForward64(&posInBB, vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					//outputs the current block
					//block = i;

					//deletes the bit from the input bitset
					bitset.vBB_[i] &= ~Tables::mask[posInBB];

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::noBit;
	}


	inline 
	int BBScan::prev_bit()
	{
		assert(has_valid_cursor());

		Ul posInBB;

		//Searches for previous bit in the last scanned block
		if (_BitScanReverse64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;									//current block has not changed, so not stored			

			return (posInBB + WMUL(scan_.bbi_));

		}
		else {

			//Searches for previous bit in the remaining blocks
			for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::noBit;
	}


	inline
	int BBScan::prev_bit(BBScan& bitset)
	{
		assert(has_valid_cursor());

		Ul posInBB;

		//Searches for previous bit in the last scanned block
		if (_BitScanReverse64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;									//current block has not changed, so not stored			

			//deletes the bit from the input bitset
			bitset.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

			return (posInBB + WMUL(scan_.bbi_));

		}
		else {

			//Searches for previous bit in the remaining blocks
			for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					//deletes the bit from the input bitset
					bitset.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::noBit;
	}



	inline 
	int BBScan::prev_bit_del()
	{
		assert(has_valid_cursor());

		Ul posInBB;

		for (auto i = scan_.bbi_; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i])) {

				//stores the current block for the next call
				scan_.bbi_ = i;

				//deletes the current bit from the bitset before returning
				vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}
		return BBObject::noBit;
	}



	inline 
	int BBScan::prev_bit_del(BBScan& bitset)
	{
		assert(has_valid_cursor());

		Ul posInBB;

		for (auto i = scan_.bbi_; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i])) {

				//stores the current block for the next call
				scan_.bbi_ = i;

				//deletes the current bit from the bitset before returning
				vBB_[i] &= ~Tables::mask[posInBB];


				//outputs the current block
				//block = i;

				//erases the bit from the input bitset
				bitset.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}


	inline
	void BBScan::init_scan(scan_types sct) noexcept  {

		switch (sct) {
		case NON_DESTRUCTIVE:
			scan_block(0);
			scan_bit(MASK_LIM);
			break;
		case NON_DESTRUCTIVE_REVERSE:
			scan_block(nBB_ - 1);
			scan_bit(WORD_SIZE);		//mask_low[WORD_SIZE] = ONE
			break;
		case DESTRUCTIVE:
			scan_block(0);
			break;
		case DESTRUCTIVE_REVERSE:
			scan_block(nBB_ - 1);
			break;
		default:
			invalid_scan_type();					
		}

		//return 0;
	}


	inline
	void BBScan::init_scan(int firstBit, scan_types sct)  noexcept {

		//special case - first bitscan
		if (firstBit == BBObject::noBit) {
			init_scan(sct);
			return;
		}


		int bbh = WDIV(firstBit);
		switch (sct) {
		case NON_DESTRUCTIVE:
		case NON_DESTRUCTIVE_REVERSE:
			scan_block(bbh);
			scan_bit(firstBit - WMUL(bbh) /* WMOD(firstBit) */);
			break;
		case DESTRUCTIVE:
		case DESTRUCTIVE_REVERSE:
			scan_block(bbh);
			break;
		default:
			invalid_scan_type();
		}

		//return 0;
	}

}//namespace bitgraph



#endif // _BITGRAPH_BBSCAN_IMP_H_






