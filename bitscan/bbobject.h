/**  
 * @file bbobject.h
 * @brief Interface for the BITSCAN library hierarchy of classes
 * @deatails created ?, last_update 14/02/2025
 * @author: pss
 *
 **/

#ifndef  __BB_OBJECT_H__
#define  __BB_OBJECT_H__

#include <iostream>
#include "bitscan/bbtypes.h"
#include "bitscan/bbconfig.h"

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

	//to stream
	friend std::ostream& operator<< (std::ostream& o , const BBObject& bb)		{ bb.print(o); return o;}

//////////////////
// Efficient nested data structures for bitscanning 
	
	//cache for bitscanning
	struct scan_t {																			
		
		int bbi_;								//bitblock index 	
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
	
	/////////////////////////////
	//
	// BitScanning Friendly classes
	// (similar to iterator models)
	// 
	// Currently, BitSet_t can only be an object derived from BBScan class (14/02/25)
	//
	////////////////////////////
	template< class BitSet_t >
	struct ScanRev {
		using basic_type = BitSet_t;
		using type = ScanRev<BitSet_t>;

	public:

		//constructor
		ScanRev(BitSet_t& bb, int firstBit = -1) : bb_(bb)	{ init_scan(firstBit);	}

		int get_block() { return  bb_.scan_.bbi_;}
		
		/**
		* @brief Scans the bitset from [firstBit , end of the bitset)
		*		 If firstBit = -1 scans the whole bitset
		* @param firstBit: starting position of the scan
		**/
		int init_scan(int firstBit = -1) { return bb_.init_scan(firstBit, BBObject::NON_DESTRUCTIVE_REVERSE ); }

		/**
		* @brief returns the next bit in the bitset during a reverse bitscanning operation
		**/
		int next_bit() { return bb_.prev_bit(); }
		
		/**
		* @brief scans the next bit in the bitset and deletes it from the
		*		bitstring bitSet
		* @param bbdel: bitset to delete the bit from
		**/
		int next_bit(BitSet_t& bitSet) { return bb_.prev_bit(bitSet); }

	private:		
		BitSet_t& bb_;		
	};

	
	template< class BitSet_t >
	struct Scan {
		using basic_type = BitSet_t;
		using type = Scan<BitSet_t>;

	public:

		//constructor
		Scan(BitSet_t& bb, int firstBit = -1) : bb_(bb) { init_scan(firstBit); }

		int get_block() { return  bb_.scan_.bbi_; }

		/**
		* @brief Scans the bitset from [firstBit , end of the bitset)
		*		 If firstBit = -1 scans the whole bitset
		* @param firstBit: starting position of the scan
		**/
		int init_scan(int firstBit = -1) { return bb_.init_scan(firstBit, BBObject::NON_DESTRUCTIVE); }

		/**
		* @brief returns the next bit in the bitset during a reverse bitscanning operation
		**/
		int next_bit() { return bb_.next_bit(); }

		/**
		* @brief scans the next bit in the bitset and deletes it from the
		*		 bitstring bitSet
		* @param bbdel: bitset to delete the bit from
		**/
		int next_bit(BitSet_t& bitSet) { return bb_.next_bit(bitSet); }

	private:
		BitSet_t& bb_;
	};

	
	template< class BitSet_t >
	struct ScanDest {
		using basic_type = BitSet_t;
		using type = ScanDest<BitSet_t>;

	public:

		//constructor
		ScanDest(BitSet_t& bb) : bb_(bb) { init_scan(); }

		int get_block() { return bb_.scan_.bbi_;}

		/**
		* @brief Scans the bitset in the range [0 , end of the bitset)
		*		 Removes bits as they are scanned
		**/
		int init_scan() { return bb_.init_scan(BBObject::DESTRUCTIVE); }

		/**
		* @brief returns the next bit in the bitset during a reverse bitscanning operation
		**/
		int next_bit() { return bb_.next_bit_del(); }

		/**
		* @brief scans the next bit in the bitset and deletes it from the
		*		 bitstring bitSet
		* @param bbdel: bitset to delete the bit from
		**/
		int next_bit(BitSet_t& bitSet) { return bb_.next_bit_del(bitSet); }

	private:		
		BitSet_t& bb_;
	};

	
	template< class BitSet_t >
	struct ScanDestRev {
		using basic_type = BitSet_t;
		using type = ScanDest<BitSet_t>;

	public:

		//constructor
		ScanDestRev(BitSet_t& bb) : bb_(bb) { init_scan(); }

		int get_block() { return bb_.scan_.bbi_;}

		/**
		* @brief Scans the bitset in the range (end_of_bitset, 0]
		*		 Removes bits as they are scanned
		**/
		int init_scan() { return bb_.init_scan(BBObject::DESTRUCTIVE_REVERSE); }

		/**
		* @brief returns the next bit in the bitset during a reverse bitscanning operation
		**/
		int next_bit() { return bb_.prev_bit_del(); }

		/**
		* @brief scans the next bit in the bitset during a reverse bitscanning operation
		*		 and deletes it from the bitstring  bitSet
		* @param bbdel: bitset to delete the bit from
		**/
		int next_bit(BitSet_t& bitSet) { return bb_.prev_bit_del(bitSet); }

	private:		
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



#endif