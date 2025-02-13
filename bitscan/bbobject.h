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


//////////////////
//
// BBObject class
// (abstract class - base of the bitstring hierarchy)
//
//////////////////

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

	//scanning classes
	template< class BitSet_t >
	struct ScanRev {
		using basic_type = BitSet_t;
		using type = ScanRev<BitSet_t>;

	public:

		//constructor
		ScanRev(BitSet_t& bb, int firstBit = -1) : bb_(bb)	{ init_scan(firstBit);	}

		int get_block() { return  scan_.bbi_; }
		
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

	private:
		scan_t scan_;
		BitSet_t& bb_;		
	};

	
	template< class BitSet_t >
	struct Scan {
		using basic_type = BitSet_t;
		using type = Scan<BitSet_t>;

	public:

		//constructor
		Scan(BitSet_t& bb, int firstBit = -1) : bb_(bb) { init_scan(firstBit); }

		int get_block() { return  scan_.bbi_; }

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

	private:
		scan_t scan_;
		BitSet_t& bb_;
	};

	
	template< class BitSet_t >
	struct ScanDest {
		using basic_type = BitSet_t;
		using type = ScanDest<BitSet_t>;

	public:

		//constructor
		ScanDest(BitSet_t& bb) : bb_(bb) { init_scan(); }

		int get_block() { return  scan_.bbi_; }

		/**
		* @brief Scans the bitset in the range [0 , end of the bitset)
		*		 Removes bits as they are scanned
		**/
		void init_scan();

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

	private:
		scan_t scan_;
		BitSet_t& bb_;
	};

	
	template< class BitSet_t >
	struct ScanDestRev {
		using basic_type = BitSet_t;
		using type = ScanDest<BitSet_t>;

	public:

		//constructor
		ScanDestRev(BitSet_t& bb) : bb_(bb) { init_scan(); }

		int get_block() { return  scan_.bbi_; }

		/**
		* @brief Scans the bitset in the range (end_of_bitset, 0]
		*		 Removes bits as they are scanned
		**/
		void init_scan();

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

	private:
		scan_t scan_;
		BitSet_t& bb_;
	};


//////////////
//construction / destruction
	BBObject() = default;

	virtual ~BBObject() = default;

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



template<class BitSet_t>
inline
int BBObject::Scan< BitSet_t>::next_bit() {

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

template<class BitSet_t>
inline
int BBObject::Scan< BitSet_t>::next_bit(BitSet_t& bb_del) {

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
inline
void BBObject::Scan< BitSet_t>::init_scan(int firstBit = -1) {

	if (firstBit == -1) {
		scan_.set_block(0);
		scan_.set_pos(MASK_LIM);
	}
	else {
		scan_.set_bit(firstBit);
	}
}



template<class BitSet_t>
inline
int BBObject::ScanDest< BitSet_t>::next_bit() {

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

template<class BitSet_t>
inline
int BBObject::ScanDest< BitSet_t>::next_bit(BitSet_t& bb_del) {

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
inline
void BBObject::ScanDest< BitSet_t>::init_scan() {

	scan_.set_block(0);
}


template<class BitSet_t>
inline
int BBObject::ScanDestRev< BitSet_t>::next_bit() {

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

template<class BitSet_t>
inline
int BBObject::ScanDestRev< BitSet_t>::next_bit(BitSet_t& bb_del) {

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
inline
void BBObject::ScanDestRev< BitSet_t>::init_scan() {

	scan_.set_block(bb_.nBB_ - 1);
}

#endif