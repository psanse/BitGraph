/**
 * @file bbscanFeature.cpp
 * @brief implements bitscanning as feature for the bitstring hierarchy (bbscanFeature.h header file)
 * @details created 11/02/2025
 * author pss
 **/

#include "bbscanFeature.h"

template<class BitSet_t>
int bscan::ScanRev<BitSet_t>::next_bit(BitSet_t& bb_del)
{

	U32 posInBB;

	//Searches for previous bit in the last scanned block
	if (_BitScanReverse64(&posInBB, bb_.vBB_[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {

		//stores the current bit for next call
		scan_.pos_ = posInBB;									//current block has not changed, so not stored			

		//deletes the bit from the input bitset
		bb_del.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

		return (posInBB + WMUL(scan_.bbi_));

	}
	else {

		//Searches for previous bit in the remaining blocks
		for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, bb_.vBB_[i])) {

				//stores the current block and bit for next call
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;

				//deletes the bit from the input bitset
				bb_del.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}
	}

	return noBit;
}

template<class BitSet_t>
int bscan::Scan<BitSet_t>::next_bit(BitSet_t& bb_del)
{

	U32 posInBB;

	//Search for next bit in the last scanned block
	if (_BitScanForward64(&posInBB, bb_.vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {

		//stores the current bit for next call
		scan_.pos_ = posInBB;									//current block has not changed, so not stored

		//deletes the bit from the input bitset
		bb_del.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

		return (posInBB + WMUL(scan_.bbi_));

	}
	else {

		//Searches for next bit in the remaining blocks
		for (auto i = scan_.bbi_ + 1; i < bb_.nBB_; ++i) {
			if (_BitScanForward64(&posInBB, bb_.vBB_[i])) {

				//stores the current block and bit for next call
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;

				//deletes the bit from the input bitset
				bb_del.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}
	}

	return noBit;
}

template<class BitSet_t>
int bscan::ScanDest<BitSet_t>::next_bit(BitSet_t& bb_del)
{

	U32 posInBB;

	//Searches for next bit from the last scanned block
	for (auto i = scan_.bbi_; i < bb_.nBB_; ++i) {

		if (_BitScanForward64(&posInBB, bb_.vBB_[i])) {
			//stores the current block
			scan_.bbi_ = i;

			//deletes the current bit before returning
			bb_.vBB_[i] &= ~Tables::mask[posInBB];


			//deletes the bit from the input bitset
			bb_del.vBB_[i] &= ~Tables::mask[posInBB];

			return (posInBB + WMUL(i));
		}

	}

	return noBit;
}

template<class BitSet_t>
int bscan::ScanRevDest<BitSet_t>::next_bit(BitSet_t& bb_del)
{

	U32 posInBB;

	//Searches for previous bit from the last scanned block
	for (auto i = scan_.bbi_; i >= 0; --i) {

		if (_BitScanReverse64(&posInBB, bb_.vBB_[i])) {

			//stores the current block for the next call
			scan_.bbi_ = i;

			//deletes the current bit from the bitset before returning
			bb_.vBB_[i] &= ~Tables::mask[posInBB];

			//deletes the bit from the input bitset
			bb_del.vBB_[i] &= ~Tables::mask[posInBB];

			return (posInBB + WMUL(i));
		}
	}
	return noBit;

}

template<class BitSet_t>
int* bscan::to_C_array(BitSet_t& bb, int* lv, std::size_t& nPop, bool rev)
{
	int bit = noBit;
	nPop = 0;

	if (rev) {

		auto sc = make_scan_rev(bb);
		while ((bit = sc.next_bit()) != EMPTY_ELEM) {
			lv[nPop++] = bit;
		}

	}
	else {

		auto sc = make_scan(bb);
		while ((bit = sc.next_bit()) != EMPTY_ELEM) {
			lv[nPop++] = bit;
		}

	}

	return lv;
}


/////////////////////////////////////////////////////////////////////////////
// Explicit type instantiation enumeration required to avoid linker errors
// (alternatively, place the code in the header file)

template int bscan::Scan<BitSet>::next_bit(BitSet& bb_del);
template int bscan::ScanRev<BitSet>::next_bit(BitSet& bb_del);
template int bscan::ScanDest<BitSet>::next_bit(BitSet& bb_del);
template int bscan::ScanRevDest<BitSet>::next_bit(BitSet& bb_del);

template int* bscan::to_C_array(BitSet& bb, int* lv, std::size_t& nPop, bool rev);