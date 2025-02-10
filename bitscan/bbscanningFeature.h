/**
 * @file bbscanning.h
 * @brief implements bitscanning as feature for the bitstring hierarchy
 * @details created 29/01/2025
 * author pss
 **/

#ifndef __BBSCANNING_FEATURE_H__
#define __BBSCANNING_FEATURE_H__

#include "bbintrinsic.h"

class bscan {
public:
	static const int noBit = -1;

	template< class BitSet_t = BBIntrin>
	struct ScanRev {
		using basic_type = BitSet_t;
		using type = ScanRev<BitSet_t>;

	public:

		ScanRev(BitSet_t& bb) :
			bb_(bb)
		{
			init_scan();
		}

		void init_scan() {
			scan_.set_block(bb_.nBB_ - 1);
			scan_.set_bit(WORD_SIZE);
		}

		int next_bit() {
			
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

			return noBit;

		}

	private:
		BBObject::scan_t scan_;
		BitSet_t& bb_;
	};

	////////////////////////////////

	template< class BitSet_t = BBIntrin>
	struct Scan {
		using basic_type = BitSet_t;
		using type = Scan<BitSet_t>;

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

			return noBit;

		}

	private:
		BBObject::scan_t scan_;
		BitSet_t& bb_;
	};


	////////////////////////////////


	template< class BitSet_t = BBIntrin>
	struct ScanDest {
		using basic_type = BitSet_t;
		using type = ScanDest<BitSet_t>;

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
			
			U32 posInBB;

			//Searches for next bit from the last scanned block
			for (auto i = scan_.bbi; i < bb_.nBB_; ++i) {

				if (_BitScanForward64(&posInBB, bb_.vBB_[i])) {
					//stores the current block
					scan_.bbi = i;

					//deletes the current bit before returning
					bb_.vBB_[i] &= ~Tables::mask[posInBB];

					return (posInBB + WMUL(i));
				}

			}

			return noBit;

		}

	private:
		BBObject::scan_t scan_;
		BitSet_t& bb_;
	};


	////////////////////////////////

	template< class BitSet_t = BBIntrin>
	struct ScanRevDest {
		using basic_type = BitSet_t;
		using type = ScanRevDest<BitSet_t>;

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

			//Searches for previous bit from the last scanned block
			for (auto i = scan_.bbi; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, bb_.vBB_[i])) {

					//stores the current block for the next call
					scan_.bbi = i;

					//deletes the current bit from the bitset before returning
					bb_.vBB_[i] &= ~Tables::mask[posInBB];

					return (posInBB + WMUL(i));
				}
			}
			return noBit;

		}

	private:
		BBObject::scan_t scan_;
		BitSet_t& bb_;
	};

	///////////////////////////////////
	//adapters
	template<class BitSet_t = BBIntrin>
	using scan = Scan<BitSet_t>;

	template<class BitSet_t = BBIntrin>
	using scanRev = ScanRev<BitSet_t>;

	template<class BitSet_t = BBIntrin>
	using scanDest = ScanDest<BitSet_t>;

	template<class BitSet_t = BBIntrin>
	using scanRevDest = ScanRevDest<BitSet_t>;


	///////////////////////////////////
	//convenient make functions
	template<class BitSet_t = BBIntrin>
	static
		scan<BitSet_t> make_scan(BitSet_t& bb) { return scan<BitSet_t>(bb); }

	template<class BitSet_t = BBIntrin>
	static
	scanRev<BitSet_t> make_scan_rev(BitSet_t& bb) { return scanRev<BitSet_t>(bb); }

	template<class BitSet_t = BBIntrin>
	static
	scanDest<BitSet_t> make_scan_dest(BitSet_t& bb) { return scanDest<BitSet_t>(bb); }

	template<class BitSet_t = BBIntrin>
	static
	scanRevDest<BitSet_t> make_scan_rev_dest(BitSet_t& bb) { return scanRevDest<BitSet_t>(bb); }

}; //end class BitScan	



#endif