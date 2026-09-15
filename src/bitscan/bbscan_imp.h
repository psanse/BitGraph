 /**
  * @file bbscan_imp.h file
  * @brief header file which implements the BBScan class from the BITSCAN library (see bbscan.h).
  * @details created 15/09/2026
  * @dev pss
  **/

#ifndef _BITGRAPH_BBSCAN_IMP_H_
#define _BITGRAPH_BBSCAN_IMP_H_


namespace bitgraph {
		

	inline 
	int BBScan::next_bit_del() {

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
	int BBScan::next_bit_del(BBScan& bbN_del) {

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
	int BBScan::next_bit() {

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
	int BBScan::next_bit(BBScan& bitset) {

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
	int BBScan::prev_bit() {

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
	int BBScan::prev_bit_del() {

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
	int BBScan::prev_bit_del(BBScan& bitset) {

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
	int BBScan::init_scan(scan_types sct) noexcept  {

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

		return 0;
	}


	inline
	int BBScan::init_scan(int firstBit, scan_types sct)  noexcept {

		//special case - first bitscan
		if (firstBit == BBObject::noBit) {
			return init_scan(sct);
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

		return 0;
	}

}//namespace bitgraph



#endif // _BITGRAPH_BBSCAN_IMP_H_






