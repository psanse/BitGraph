/**
 * @file bbscan_sparse.h
 * @brief header for sparse class equivalent to the BBScan class
 *		  manages sparse bit strings with efficient bitscanning capabilities
 * @author pss
 * @details: created 2012, last_update 25/02/2025
 *
 * TODO refactoring and testing 25/02/2025
 **/

#ifndef BITGRAPH_BITSCAN_BBSCAN_SPARSE_H
#define BITGRAPH_BITSCAN_BBSCAN_SPARSE_H

#include "detail/persistent_scan.h"
#include "bbset_sparse.h"
#include <cassert>

using namespace std;

namespace bitgraph {

	/////////////////////////////////
	//
	// Class BBScanSp
	// (Uses a number of optimizations for sparse bit scanning)
	//
	///////////////////////////////////

	class BBScanSp  : public BitsetSp {

	public:

		template<class>
		friend class detail::PersistentScan;

		template<class>
		friend class detail::PersistentScanReverse;

		template<class>
		friend class detail::PersistentScanDestructive;

		template<class>
		friend class detail::PersistentScanDestructiveReverse;

	public:

		//aliases for bitscanning 
		using scan = detail::PersistentScan<BBScanSp>;
		using scanR = detail::PersistentScanReverse<BBScanSp>;
		using scanD = detail::PersistentScanDestructive<BBScanSp>;
		using scanDR = detail::PersistentScanDestructiveReverse<BBScanSp>;

		//////////////////////////////
		//construction / destruction
				
		using BitsetSp::BitsetSp;		//inherit constructors

		//TODO...check copy and move assignments - should be forbidden

		~BBScanSp() = default;

		///////////////
		// setters and getters
		void scan_block(int blockID) { scan_.bbi_ = blockID; }			//refers to the position in the collection (not in the bitstring)
		void scan_bit(int bit) { scan_.pos_ = bit; }

		int  scan_block()	 const { return scan_.bbi_; }
		int  scan_bit()	 const { return scan_.pos_; }

		//////////////////////////////
		// Bitscanning (with cached info)

		/**
		* @brief Configures the initial block and bit position for bitscanning
		*		 according to one of the 4 scan types passed as argument
		* @param sct: type of scan
		* @details : sparse bitsets may have no blocks, in which case the scan is not possible and
		*		     the function returns -1 -
		**/
		void init_scan(scan_types sct) noexcept;

		/**
		* @brief Configures the initial block and bit position for bitscanning
		*		 starting from the bit 'firstBit' onwards, excluding 'firstBit'
		*		 according to one of the 4 scan types passed as argument
		*		 (currently ONLY for the NON-DESTRUCTIVE cases).
		*		 If firstBit is -1 (BBObject::noBit), the scan starts from the beginning.
		* @param firstBit: starting bit
		* @param sct: type of scan
		* @details : sparse bitsets may have no blocks, in which case the scan is not possible and
		*		     the function returns -1
		* @details : (08/07/2025) throws  BitScanError for empty sparse bitset
		*	
		**/
		void init_scan(int firstBit, scan_types sct) noexcept;

		////////////////
		// bitscan forward

		/**
		* @brief next bit in the bitstring, starting from the block
		*		 in the last call to next_bit.
		*		 Scan type: destructive
		*
		*		 I. caches the current block for the next call
		*		II. erases the current scanned bit
		*		III. First call requires initialization with init_scan(DESTRUCTIVE)
		*
		* @returns the next bit in the bitstring, BBObject::noBit if there are no more bits
		* @details created 2015, last update 25/02/2025
		**/
		inline int next_bit_del();
		inline int next_bit_del(BBScanSp& bitset);

		/**
		* @brief next bit in the bitstring, starting from the bit retrieved
		*		 in the last call to next_bit.
		*		 Scan type: non-destructive
		*
		*		 I. caches the current block for the next call
		*		II. DOES NOT erase the current scanned bit
		*		III. caches the scanned bit for the next call
		* 		IV. First call requires initialization with init_scan(NON-DESTRUCTIVE)
		*
		* @returns the next bit in the bitstring, BBObject::noBit if there are no more bits
		* @details Created 5/9/2014, last update 09/02/2025
		* @details Since the scan does not delete the scanned bit from the bitstring,
		*		   it has to cache the last scanned bit for the next call
		**/
		inline int next_bit();
		inline int next_bit(BBScanSp& bitset);

		/**
		* @brief for basic bitscanning - they are hidden by next_bit()
		**/
		using BitsetSp::next_bit;

		////////////////
		// bitscan backwards

		/**
		* @brief previous bit (next less significant bit) in the bitstring, starting from the bit retrieved
		*		 in the last call to next_bit.
		*		 Scan type: destructive, reverse
		*
		*		 I. caches the current block for the next call
		*		II. erases the current scanned bit
		* 		III. First call requires initialization with init_scan(DESTRUCTIVE, REVERSE)
		*
		* @returns the next lsb bit in the bitstring, BBObject::noBit if there are no more bits
		* @details Created 23/3/12, last update 09/02/2025
		**/
		inline int prev_bit_del();
		inline int prev_bit_del(BBScanSp& bitset);
		/**
		* @brief previous bit (next less significant bit)) in the bitstring, starting from the bit retrieved
		*		 in the last call to next_bit.
		*		 Scan type: non-destructive, reverse
		*
		*		 I. caches the current block for the next call
		*		II. caches the scanned bit for the next call
		* 		III. First call requires initialization with init_scan(NON-DESTRUCTIVE, REVERSE)
		*
		* @returns the previous bit (next less significant bit) in the bitstring, BBObject::noBit if there are no more bits
		* @details Created 5/9/2014, last update 09/02/2025
		* @details Since the scan does not delete the scanned bit from the bitstring,
		*		   it has to cache the last scanned bit for the next call
		**/
		inline int prev_bit();
		inline int prev_bit(BBScanSp& bitset);

		/////////////////
		//	DEPRECATED

	//inline int next_bit_del				(int& nBB);								//nBB: index of bitblock in the bitstring	(not in the collection)	
	//inline int next_bit_del				(int& nBB, BBScanSp& );					//EXPERIMENTAL! 
	//inline int next_bit_del_pos			(int& posBB);							//posBB: position of bitblock in the collection (not the index of the element)		
	//inline int next_bit					(int& nBB);								//nBB: index of bitblock in the bitstring	(not in the collection)				
	//inline int prev_bit_del				(int& nBB);


	protected:
		
		// terminating handlers
		[[noreturn]]
		inline void invalid_scan_type() noexcept {
			LOG_ERROR("Unknown scan type in BBScanSp::init_scan");
			std::terminate();
		}

		void invalidate_scan() noexcept {
			scan_block(BBObject::noBit);
			scan_bit(BBObject::noBit);
		}

		bool has_valid_cursor() const noexcept {
			return scan_.bbi_ != BBObject::noBit;
		}

	//////////////////
	// data members

		scan_t scan_;
	};

}//end namespace bitgraph

///////////////////////
// Necessary implementations, necessary in header file for generic code

#include "detail/bbscan_sparse_imp.h"



#endif // BITGRAPH_BITSCAN_BBSCAN_SPARSE_H