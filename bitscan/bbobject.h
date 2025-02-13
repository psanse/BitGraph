/**  
 * @file bbobject.h
 * @brief Interface for the BITSCAN library hierarchy of classes
 * @deatails created ?, last_update 01/02/2025
 * @author: pss
 *
 **/

#ifndef  __BB_OBJECT_H__
#define  __BB_OBJECT_H__

#include <iostream>
#include "bbtypes.h"
#include "tables.h"

class BBObject{
public:
	static const int noBit = -1;

	//types of bit scans
	enum scan_types	{NON_DESTRUCTIVE, NON_DESTRUCTIVE_REVERSE, DESTRUCTIVE, DESTRUCTIVE_REVERSE};				

	friend std::ostream& operator<< (std::ostream& o , const BBObject& bb)		{ bb.print(o); return o;}


//////////////////
// Efficient nested data structures for bitscanning 
	
	//cache for bitscanning
	struct scan_t {																			
		
		int bbi_;								//bitboard index 	
		int pos_;								//bit index [0...63] 

		scan_t() :bbi_(noBit), pos_(MASK_LIM)		{}
				
		void set_block(int block)					{ bbi_ = block; }
		void set_pos(int bit)						{ pos_ = bit; }
		void set_bit(int bit) {
			int bbh = WDIV(bit);
			bbi_ = bbh;
			pos_ = bit - WMUL(bbh);					/* equiv. to WMOD(bit)*/
		}
	};

	//scanning classes - similar to iterators
	template< class BitSet_t >
	struct ScanRev {
		using basic_type = BitSet_t;
		using type = ScanRev<BitSet_t>;

	public:

		//constructor
		ScanRev(BitSet_t& bb, int firstBit = -1) :
			bb_(bb)
		{
			init_scan(firstBit);
		}
		
		/**
		* @brief Scans the bitset from [firstBit , end of the bitset)
		*		 If firstBit = -1 scans the whole bitset
		* @param firstBit: starting position of the scan
		**/
		void init_scan(int firstBit = -1);

		/**
		* @brief returns the next bit in the bitset during a reverse bitscanning operation
		**/
		int next_bit();

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
		scan_t scan_;
		BitSet_t& bb_;		
	};



//////////////
//construction / destruction
	BBObject() = default;

	virtual ~BBObject() = default;
	
//////////////////	
//bit scanning

	virtual int init_scan			(scan_types)								 { return 0; }

///////////////////
//I/O

	virtual std::ostream& print(	std::ostream& o = std::cout, 
									bool show_pc = true,
									bool endl = true					) const = 0;
};

/////////////////////////////////////
// Necessary template implementation in header file


template<class BitSet_t>
inline
int BBObject::ScanRev< BitSet_t>::next_bit() {
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

template<class BitSet_t>
inline
int BBObject::ScanRev< BitSet_t>::next_bit(BitSet_t& bb_del) {
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
inline
void BBObject::ScanRev< BitSet_t>::init_scan(int firstBit = -1) {

	if (firstBit == -1) {
		scan_.set_block(bb_.nBB_ - 1);
		scan_.set_pos(WORD_SIZE);
	}
	else {
		scan_.set_bit(firstBit);
	}
}

#endif