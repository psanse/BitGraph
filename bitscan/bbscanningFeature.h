/**
 * @file bbscanning.h
 * @brief Attempt to refactor bitscanning operations using generic programming
 * @details created 29/01/2025
 * author pss
 **/

#ifndef __BBSCANNING_FEATURE_H__
#define __BBSCANNING_FEATURE_H__

#include "bbintrinsic.h"


template< class BitSet_t = BBIntrin>
struct ScanRev {
	using basic_type = BitSet_t;

public:

	ScanRev(BitSet_t& bb):
		bb_(bb)
	{
		init_scan();
	}

	void init_scan() {
		scan_.set_block	(bb_.nBB_ - 1);
		scan_.set_bit	(WORD_SIZE);
	}

	int next_bit() {
		//DirectRevScan
		U32 posInBB;

		//Searches for previous bit in the last scanned block
		if (_BitScanReverse64(&posInBB, bb_.vBB_[scan_.bbi] & Tables::mask_low[scan_.pos])) {

			//stores the current bit for next call
			scan_.pos = posInBB;									//current block has not changed, so not stored			

			return (posInBB + WMUL(scan_.bbi));

		}
		else {

			//Searches for previous bit in the remaining blocks
			for (auto i = scan_.bbi - 1; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, bb_.vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi = i;
					scan_.pos = posInBB;

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::NOBIT;

	}

private:
	BBObject::scan_t scan_;	
	BitSet_t& bb_;
};	

////////////////////////////////

template< class BitSet_t = BBIntrin>
struct Scan {
	using basic_type = BitSet_t;

public:

	Scan(BitSet_t& bb) :
		bb_(bb)
	{
		init_scan();
	}

	void init_scan() {
		scan_.set_block(0);
		scan_.set_bit(MASK_LIM);
	}

	int next_bit() {
		//DirectScan
		U32 posInBB;

		//Search for next bit in the last scanned block
		if (_BitScanForward64(&posInBB, bb_.vBB_[scan_.bbi] & Tables::mask_high[scan_.pos])) {

			//stores the current bit for next call
			scan_.pos = posInBB;									//current block has not changed, so not stored

			return (posInBB + WMUL(scan_.bbi));

		}
		else {

			//Searches for next bit in the remaining blocks
			for (auto i = scan_.bbi + 1; i < bb_.nBB_; ++i) {
				if (_BitScanForward64(&posInBB, bb_.vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi = i;
					scan_.pos = posInBB;

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::NOBIT;

	}

private:
	BBObject::scan_t scan_;
	BitSet_t& bb_;
};


////////////////////////////////


template< class BitSet_t = BBIntrin>
struct ScanDest {
	using basic_type = BitSet_t;

public:

	ScanDest(BitSet_t& bb) :
		bb_(bb)
	{
		init_scan();
	}

	void init_scan() {
		scan_.set_block(0);
	}

	int next_bit() {
		//DestructiveScan
		U32 posInBB;

		for (auto i = scan_.bbi; i < bb_.nBB_; ++i) {

			if (_BitScanForward64(&posInBB, bb_.vBB_[i])) {
				//stores the current block
				scan_.bbi = i;

				//deletes the current bit before returning
				bb_.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}

		}

		return BBObject::NOBIT;

	}

private:
	BBObject::scan_t scan_;
	BitSet_t& bb_;
};


////////////////////////////////

template< class BitSet_t = BBIntrin>
struct ScanRevDest {
	using basic_type = BitSet_t;

public:

	ScanRevDest(BitSet_t& bb) :
		bb_(bb)
	{
		init_scan();
	}

	void init_scan() {
		scan_.set_block(bb_.nBB_ - 1);
	}

	int next_bit() {
		U32 posInBB;

		for (auto i = scan_.bbi; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, bb_.vBB_[i])) {

				//stores the current block for the next call
				scan_.bbi = i;

				//deletes the current bit from the bitset before returning
				bb_.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}
		return BBObject::NOBIT;

	}

private:
	BBObject::scan_t scan_;
	BitSet_t& bb_;
};

//adapters
template<class BitSet_t = BBIntrin>
using scan		= Scan<BitSet_t>;

template<class BitSet_t = BBIntrin>
using scanRev	= ScanRev<BitSet_t>;

template<class BitSet_t = BBIntrin>
using scanDest	= ScanDest<BitSet_t>;

template<class BitSet_t = BBIntrin>
using scanRevDest	= ScanRevDest<BitSet_t>;


#endif