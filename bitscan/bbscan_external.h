/**
 * @file bbscan_external.h
 * @brief implements efficient bitscanning as an external feature for the bitstring hierarchy
 * @details created 29/01/2025, last_updated 12/02/2025
 * author pss
 **/

#ifndef __BBSCAN_EXTERNAL_H__
#define __BBSCAN_EXTERNAL_H__

#include "bbset.h"

class bscan {
public:
	static const int noBit = -1;

	/**
	* @brief Converts the bitstring bb to a C-array of integers
	* @param bb: input bitstring
	* @param lv: output array which must be initialized previously to at least the number of 1-bits 
	*			   of the bitstring bb.
	* @param size: output size of the array (number of bits in the bitstring)			   
	* @param rev: if true, the bits are stored in reverse order
	* @returns the C-array of integers
	* @details created 11/02/2025  
	* @details preserved for backward compatibility of the opt algorithm code - TO REMOVE 09/02/2025
	**/
	template<class BitSet_t>
	static int* to_C_array (BitSet_t& bb, int* lv, std::size_t& size, bool rev = false) ;


	template< class BitSet_t = BitSet>
	struct ScanRev {
		using basic_type = BitSet_t;
		using type = ScanRev<BitSet_t>;

	public:

		ScanRev(BitSet_t& bb, int firstBit=-1) :
			bb_(bb)
		{
			init_scan(firstBit);
		}

		/**
		* @brief Scans the bitset from [firstBit , end of the bitset)
		*		 If firstBit = -1 scans the whole bitset
		* @param firstBit: starting position of the scan
		**/
		void init_scan(int firstBit = -1) {

			if (firstBit == -1) {
				scan_.set_block(bb_.nBB_ - 1);
				scan_.set_pos(WORD_SIZE);
			}
			else {
				scan_.set_bit(firstBit);	
			}
		}

		/**
		* @brief returns the next bit in the bitset during a reverse bitscanning operation
		**/
		int next_bit() {
			
			U32 posInBB;

			//Searches for previous bit in the last scanned block
			if (_BitScanReverse64(&posInBB, bb_.vBB_[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {

				//stores the current bit for next call
				scan_.pos_ = posInBB;									//current block has not changed, so not stored			

				return (posInBB + WMUL(scan_.bbi_));

			}
			else {

				//Searches for previous bit in the remaining blocks
				for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

					if (_BitScanReverse64(&posInBB, bb_.vBB_[i])) {

						//stores the current block and bit for next call
						scan_.bbi_ = i;
						scan_.pos_ = posInBB;

						return (posInBB + WMUL(i));
					}
				}
			}

			return noBit;

		}

		/**
		* @brief scans the next bit in the bitset and deletes it from the
		*		 bitset bb_del
		* @param bbdel: bitset to delete the bit from
		**/
		int next_bit(BitSet_t& bb_del);


		/**
		* @brief returns the index of the current bitblock being scanned
		**/
		int get_block() { return  scan_.bbi_; }

	private:
		BBObject::scan_t scan_;
		BitSet_t& bb_;
	};

	////////////////////////////////

	template< class BitSet_t = BitSet>
	struct Scan {
		using basic_type = BitSet_t;
		using type = Scan<BitSet_t>;

	public:

		Scan(BitSet_t& bb, int firstBit = -1) :
			bb_(bb)
		{
			init_scan(firstBit);
		}

		/**
		* @brief Scans the bitset from [firstBit , end of the bitset)
		*		 If firstBit = -1 scans the whole bitset
		* @param firstBit: starting position of the scan
		**/
		void init_scan(int firstBit = -1) {


			if (firstBit == -1) {
				scan_.set_block(0);
				scan_.set_pos(MASK_LIM);
			}
			else {
				scan_.set_bit(firstBit);
			}
			
		}


		/**
		* @brief returns the next bit in the bitset during a direct bitscanning operation
		**/
		int next_bit() {
			
			U32 posInBB;

			//Search for next bit in the last scanned block
			if (_BitScanForward64(&posInBB, bb_.vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {

				//stores the current bit for next call
				scan_.pos_ = posInBB;									//current block has not changed, so not stored

				return (posInBB + WMUL(scan_.bbi_));

			}
			else {

				//Searches for next bit in the remaining blocks
				for (auto i = scan_.bbi_ + 1; i < bb_.nBB_; ++i) {
					if (_BitScanForward64(&posInBB, bb_.vBB_[i])) {

						//stores the current block and bit for next call
						scan_.bbi_ = i;
						scan_.pos_ = posInBB;

						return (posInBB + WMUL(i));
					}
				}
			}

			return noBit;

		}

		/**
		* @brief scans the next bit in the bitset and deletes it from the
		*		 bitset bb_del
		* @param bbdel: bitset to delete the bit from
		**/
		int next_bit(BitSet_t&  bb_del);


		/**
		* @brief returns the index of the current bitblock being scanned
		**/
		int get_block() { return  scan_.bbi_; }

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

		/**
		* @brief returns the next bit in the bitset during a direct bitscanning operation
		**/
		int next_bit() {
			
			U32 posInBB;

			//Searches for next bit from the last scanned block
			for (auto i = scan_.bbi_; i < bb_.nBB_; ++i) {

				if (_BitScanForward64(&posInBB, bb_.vBB_[i])) {
					//stores the current block
					scan_.bbi_ = i;

					//deletes the current bit before returning
					bb_.vBB_[i] &= ~Tables::mask[posInBB];

					return (posInBB + WMUL(i));
				}

			}

			return noBit;

		}

		/**
		* @brief scans the next bit in the bitset and deletes it from the
		*		 bitset bb_del
		* @param bbdel: bitset to delete the bit from
		**/
		int next_bit(BitSet_t& bb_del);

		/**
		* @brief returns the index of the current bitblock being scanned
		**/
		int get_block() { return scan_.bbi_; }

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

		/**
		* @brief returns the next bit in the bitset during a reverse bitscanning operation
		**/
		int next_bit() {

			U32 posInBB;

			//Searches for previous bit from the last scanned block
			for (auto i = scan_.bbi_; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, bb_.vBB_[i])) {

					//stores the current block for the next call
					scan_.bbi_ = i;

					//deletes the current bit from the bitset before returning
					bb_.vBB_[i] &= ~Tables::mask[posInBB];

					return (posInBB + WMUL(i));
				}
			}
			return noBit;

		}


		/**
		* @brief scans the next bit in the bitset and deletes it from the
		*		 bitset bb_del
		* @param bbdel: bitset to delete the bit from
		**/
		int next_bit(BitSet_t& bb_del);


		/**
		* @brief returns the index of the current bitblock being scanned
		**/
		int get_block() { return scan_.bbi_; }

	private:
		BBObject::scan_t scan_;
		BitSet_t& bb_;
	};

	///////////////////////////////////
	//adapters
	template<class BitSet_t = BBIntrin>
	using scan = Scan<BitSet_t>;

	template<class BitSet_t = BBIntrin>
	using scanReverse = ScanRev<BitSet_t>;

	template<class BitSet_t = BBIntrin>
	using scanDestructive = ScanDest<BitSet_t>;

	template<class BitSet_t = BBIntrin>
	using scanReverseDestructive = ScanRevDest<BitSet_t>;


	///////////////////////////////////
	//convenient make functions
	template<class BitSet_t = BBIntrin>
	static
		scan<BitSet_t> make_scan(BitSet_t& bb, int firstBit=-1) { return scan<BitSet_t>(bb, firstBit); }

	template<class BitSet_t = BBIntrin>
	static
	scanReverse<BitSet_t> make_scan_rev(BitSet_t& bb, int firstBit = -1) { return scanReverse<BitSet_t>(bb, firstBit); }

	template<class BitSet_t = BBIntrin>
	static
	scanDestructive<BitSet_t> make_scan_dest(BitSet_t& bb) { return scanDestructive<BitSet_t>(bb); }

	template<class BitSet_t = BBIntrin>
	static
	scanReverseDestructive<BitSet_t> make_scan_rev_dest(BitSet_t& bb) { return scanReverseDestructive<BitSet_t>(bb); }

}; //end class BitScan	



#endif

