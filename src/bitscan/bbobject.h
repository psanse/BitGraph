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

namespace bitgraph {
	namespace _impl {
		//////////////////
		//
		// BBObject class
		// (abstract class - base of the bitset hierarchy)
		//
		//////////////////

		class BBObject {
		public:
			static const int noBit = -1;
			static const BITBOARD noBlock = std::numeric_limits<BITBOARD>::max();		

			//types of bit scans
			enum scan_types { NON_DESTRUCTIVE, NON_DESTRUCTIVE_REVERSE, DESTRUCTIVE, DESTRUCTIVE_REVERSE };

			//to stream
			friend std::ostream& operator<< (std::ostream& o, const BBObject& bb) { bb.print(o, true/*show_pc*/, false/*endl*/); return o; }

			//////////////////
			// Efficient nested data structures for bitscanning 

				//cache for bitscanning
			struct scan_t {

				int bbi_;								//bitblock index 	
				int pos_;								//bit index [0...63] 

				scan_t() :bbi_(noBit), pos_(MASK_LIM) {}

				void set_block(int block) { bbi_ = block; }
				void set_pos(int bit) { pos_ = bit; }
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
			// Currently, BitSetT can only be an object derived from BBScan class (14/02/25)
			//
			////////////////////////////

			template< class BitSetT >
			struct ScanRev {
				using basic_type = BitSetT;
				using type = ScanRev<BitSetT>;

			public:
								
				/**
				* @brief: constructor for reverse bitscanning - may throw for sparse bitsets if empty
				**/
				ScanRev(BitSetT& bb, int firstBit = -1) : bb_(bb) { init_scan(firstBit); }

				int get_block() { return  bb_.scan_.bbi_; }

				/**
				* @brief Scans the bitset from [firstBit , end of the bitset)
				*		 If firstBit = -1 scans the whole bitset
				* @param firstBit: starting position of the scan
				* @details: may throw for sparse bitsets if empty
				**/
				int init_scan(int firstBit = -1) { return bb_.init_scan(firstBit, BBObject::NON_DESTRUCTIVE_REVERSE); }

				/**
				* @brief returns the next bit in the bitset during a reverse bitscanning operation
				**/
				int next_bit() { return bb_.prev_bit(); }

				/**
				* @brief scans the next bit in the bitset and deletes it from the
				*		bitstring bitSet
				* @param bbdel: bitset to delete the bit from
				**/
				int next_bit(BitSetT& bitSet) { return bb_.prev_bit(bitSet); }

			private:
				BitSetT& bb_;
			};


			template< class BitSetT >
			struct Scan {
				using basic_type = BitSetT;
				using type = Scan<BitSetT>;

			public:

				/**
				* @brief: constructor for bitscanning - may throw for sparse bitsets if empty
				**/
				Scan(BitSetT& bb, int firstBit = -1) : bb_(bb) { init_scan(firstBit); }

				int get_block() { return  bb_.scan_.bbi_; }

				/**
				* @brief Scans the bitset from [firstBit , end of the bitset)
				*		 If firstBit = -1 scans the whole bitset
				* @param firstBit: starting position of the scan
				* @details: may throw for sparse bitsets if empty
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
				int next_bit(BitSetT& bitSet) { return bb_.next_bit(bitSet); }

			private:
				BitSetT& bb_;
			};


			template< class BitSetT >
			struct ScanDest {
				using basic_type = BitSetT;
				using type = ScanDest<BitSetT>;

			public:

				/**
				* @brief: constructor for destructive bitscanning - may throw for sparse bitsets if empty
				**/
				ScanDest(BitSetT& bb) : bb_(bb) { init_scan(); }

				int get_block() { return bb_.scan_.bbi_; }

				/**
				* @brief Scans the bitset in the range [0 , end of the bitset)
				*		 Removes bits as they are scanned
				* @details: may throw for sparse bitsets if empty
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
				int next_bit(BitSetT& bitSet) { return bb_.next_bit_del(bitSet); }

			private:
				BitSetT& bb_;
			};


			template< class BitSetT >
			struct ScanDestRev {
				using basic_type = BitSetT;
				using type = ScanDest<BitSetT>;

			public:

				/**
				* @brief: constructor for destructive reverse bitscanning - may throw for sparse bitsets if empty
				**/
				ScanDestRev(BitSetT& bb) : bb_(bb) { init_scan(); }

				int get_block() { return bb_.scan_.bbi_; }

				/**
				* @brief Scans the bitset in the range (end_of_bitset, 0]
				*		 Removes bits as they are scanned
				* @details: may throw for sparse bitsets if empty
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
				int next_bit(BitSetT& bitSet) { return bb_.prev_bit_del(bitSet); }

			private:
				BitSetT& bb_;
			};


			//////////////
			//construction / destruction
			BBObject() = default;

			virtual ~BBObject() = default;

			///////////////////
			//I/O

			virtual std::ostream& print(std::ostream& o = std::cout,
				bool show_pc = true,
				bool endl = true) const = 0;
		};

	}// namespace _impl

	using _impl::BBObject;

	namespace _impl {
		std::ostream& operator<< (std::ostream& o, const BBObject& bb);
	}	
	using _impl::operator<<;

}// namespace bitgraph

#endif
