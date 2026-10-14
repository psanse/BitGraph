/**
 * @file bbscan.h
 * @brief Persistent-cursor bit scanning for dense BITSCAN bitsets.
 *
 * @details
 * This header defines @c bitgraph::BBScan, a @c Bitset-derived type that
 * stores a mutable scan cursor alongside the bitset data. The persistent
 * cursor is designed for hot paths that repeatedly scan a bitset, avoiding
 * the setup and state transfer required by a standalone scanner.
 *
 * BBScan supports forward and reverse scans, both non-destructive and
 * destructive, as well as scans that apply their result to another bitset.
 * It also exposes lightweight scanner adapters backed by the same cursor.
 * Copying or moving a BBScan transfers the bitset contents but does not copy
 * an active scan position.
 *
 * Only one scan may be active on a given BBScan object at a time. Initializing
 * another scan overwrites the stored cursor; independent BBScan objects may
 * be scanned concurrently. For general-purpose scanning, prefer the external
 * scanning views; use BBScan when persistent cursor state is beneficial.
 *
 * @author pss
 * @date Last updated: 14 October 2026
 *
 * @see bbset.h
 * @see bbobject.h
 * @see bbscan_view.h
 * @see detail/persistent_scan.h
 * @warning Destructive scan operations clear the bits they return.
 */

#ifndef BITGRAPH_BITSCAN_BBSCAN_H
#define BITGRAPH_BITSCAN_BBSCAN_H

#include "detail/persistent_scan.h"
#include "bbset.h"	
#include <cassert>
#include <type_traits>
#include <utility>			// std::move
#include <exception>		// std::terminate
#include <cstdio>			// stderr
#include <initializer_list>

namespace bitgraph{
		
		/**
		* @brief Bitset implementation optimized for high-performance bit scanning.
		*
		* BBScan stores a persistent scan cursor (`scan_t`) inside the bitset to
		* minimize initialization and state-management overhead in performance-critical
		* code paths. This design is particularly suited to bit-parallel graph
		* algorithms, where bitsets represent vertex sets, candidate sets, or graph
		* neighborhoods and are scanned repeatedly.
		*
		* @warning A BBScan object supports only one active scan at a time. Starting or
		*          initializing another scan on the same bitset overwrites its
		*          persistent scan cursor and invalidates the scan already in progress.
		*          Different BBScan objects may be scanned independently.
		*
		* @note The scanner classes associated with BBScan are lightweight interfaces
		*       over the persistent cursor owned by the bitset; they do not maintain
		*       independent scan state.
		*/

		class BBScan : public Bitset {

			template<class>
			friend class detail::PersistentScan;

			template<class>
			friend class detail::PersistentScanReverse;

			template<class>
			friend class detail::PersistentScanDestructive;

			template<class>
			friend class detail::PersistentScanDestructiveReverse;
	

		public :
	
			/**
			* @brief Punblic Hot-path scanners using the persistent cursor owned by BBScan.
			* @details Only one scanner may be active on a given BBScan object at a time.
			*/

			using scan = detail::PersistentScan<BBScan>;
			using scanR = detail::PersistentScanReverse<BBScan>;
			using scanD = detail::PersistentScanDestructive<BBScan>;
			using scanDR = detail::PersistentScanDestructiveReverse<BBScan>;

			// for basic bitscanning operatins - they are hidden by next_bit()
			using Bitset::next_bit;
			using Bitset::prev_bit;			// stateless predecessor, also hidden by prev_bit()

			//////////////////////////////
			//construction / destruction

			//inherit constructors	
			using Bitset::Bitset;

			BBScan() = default;		// not implicitly declared once copy/move constructors are user-provided

			/**
			* @brief Copy constructor: copies the bitstring only.
			* @details A newly constructed BBScan never inherits an active scan from
			*          another object: its scan cursor is default-initialized.
			**/
			BBScan(const BBScan& other) : Bitset(other), scan_() {}

			/**
			* @brief Move constructor: moves the bitstring only.
			* @details A newly constructed BBScan never inherits an active scan from
			*          another object: its scan cursor is default-initialized.
			**/
			BBScan(BBScan&& other) noexcept : Bitset(std::move(other)), scan_() {}

			/**
			* @brief Copy assignment: copies the bitstring only.
			* @details The scan cursor of this object is left untouched; the cursor
			*          of @p other is never transferred.
			**/
			BBScan& operator=(const BBScan& other) {
				Bitset::operator=(other);
				return *this;
			}

			/**
			* @brief Move assignment: moves the bitstring only.
			* @details The scan cursor of this object is left untouched; the cursor
			*          of @p other is never transferred.
			**/
			BBScan& operator=(BBScan&& other) noexcept {
				Bitset::operator=(std::move(other));
				return *this;
			}

			~BBScan() = default;				
		
		
			//////////////////////////////
			// Bitscanning (with cached info)

			/**
			* @brief Configures the initial block and bit position for bitscanning
			*		 according to one of the 4 scan types passed as argument
			* @param sct: type of scan
			* @details: fail-safe policy, program terminates if error
			**/
			virtual void init_scan (scan_types sct) noexcept;

			/**
			* @brief Configures the initial block and bit position for bitscanning
			*		 starting from the bit 'firstBit' onwards, excluding 'firstBit'
			*		 according to one of the 4 scan types passed as argument.
			*		 If firstBit is -1 (BBObject::noBit), the scan starts from the beginning.
			* @param firstBit: starting bit
			* @param sct: type of scan
			* @details: fail-safe policy, program terminates if error
			**/
			void init_scan(int firstBit, scan_types sct) noexcept ;


			////////////////
			// bitscan forwards
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
			* @details Created   23/3/12, last update 09/02/2025
			**/
			virtual  int next_bit_del();


			/**
			* @brief next bit in the bitstring, starting from the block
			*		 in the last call to next_bit.
			*		 Returns in block the current bitblock of the returned bit and deletes from
			*		 the input bitset the current bit scanned (returned).
			*		 Erases the current scanned bit from the bitblock bitset passed as argument
			*		 Scan type: destructive
			*
			*		 I. caches the current block for the next call
			*		II. erases the current scanned bit
			* 		III. First call requires initialization with init_scan(DESTRUCTIVE)
			*
			* @param bitset: a bitset from which the scanned bit is erased
			* @param block: output parameter with the current bitblock
			* @returns the next bit in the bitstring, BBObject::noBit if there are no more bits
			**/
			virtual int next_bit_del(BBScan& bitset);

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
			* @details Created   23/3/12, last update 09/02/2025
			* @details Since the scan does not delete the scanned bit from the bitstring,
			*		   it has to cache the last scanned bit for the next call
			**/
			virtual	 int next_bit() const;
				

			/**
			* @brief next bit in the bitstring, starting from the bit retrieved
			*		 in the last call to next_bit.
			*		 Deletes from the input bitset the current bit scanned (returned).
			*		 Deletes the current scanned bit from the bitblock bitset passed as argument
			*		 Scan type: non-destructive
			*
			*		 I. caches the current block for the next call
			*		II. DOES NOT erase the current scanned bit
			*		III.caches the scanned bit for the next call
			* 		IV. First call requires initialization with init_scan(NON-DESTRUCTIVE)
			*
			* @param bitset: bitblock where the scanned bit is erased
			* @returns the next bit in the bitstring, BBObject::noBit if there are no more bits
			* @details Created   23/3/12, last update 09/02/2025
			* @details Since the scan does not delete the scanned bit from the bitstring,
			*		   it has to cache the last scanned bit for the next call
			**/
			virtual int next_bit(BBScan& bitset);

			////////////////
			// bitscan backwards

			/**
			* @brief next least-significant bit in the bitstring, starting from the bit retrieved
			*		 in the last call to next_bit.
			*		 Scan type: non-destructive, reverse
			*
			*		 I. caches the current block for the next call
			*		II. caches the scanned bit for the next call
			* 		III. First call requires initialization with init_scan(NON-DESTRUCTIVE, REVERSE)
			*
			* @returns the next lsb bit in the bitstring, BBObject::noBit if there are no more bits
			* @details Created   23/3/12, last update 09/02/2025
			* @details Since the scan does not delete the scanned bit from the bitstring,
			*		   it has to cache the last scanned bit for the next call
			**/
			virtual  int prev_bit() const;
			virtual	 int prev_bit(BBScan& bitset);


			/**
			* @brief next least-significant bit in the bitstring, starting from the bit retrieved
			*		 in the last call to next_bit.
			*		 Scan type: destructive, reverse
			*
			*		 I. caches the current block for the next call
			*		II. erases the current scanned bit
			* 		III. First call requires initialization with init_scan(DESTRUCTIVE, REVERSE)
			*
			* @returns the next lsb bit in the bitstring, BBObject::noBit if there are no more bits
			* @details Created   23/3/12, last update 09/02/2025
			**/
			virtual int prev_bit_del();


			/**
			* @brief next least-significant bit in the bitstring, starting from the bit retrieved
			*		 in the last call to next_bit.
			*		 Returns in block the current bitblock of the returned bit and deletes from
			*		 the input bitset the current bit scanned (returned).
			*		 Erases the current scanned bit from the bitblock bitset passed as argument
			*		 Scan type: destructive, reverse
			*
			*		 I. caches the current block for the next call
			*		II. erases the current scanned bit
			* 		IV. First call requires initialization with init_scan(DESTRUCTIVE, REVERSE)
			*
			* @param block: output bitblock of the retrieved bit
			* @param bitset: bitset from which the retrieved bit is erased
			* @returns the next lsb bit in the bitstring, BBObject::noBit if there are no more bits
			* @details Created   23/3/12, last update 09/02/2025
			**/
			virtual int prev_bit_del(BBScan& bitset);

		protected:

		///////////////////////////////
		//setters and getters
		
			void scan_block(int bbindex) noexcept { scan_.bbi_ = bbindex; }
			void scan_bit(int posbit) noexcept { scan_.pos_ = posbit; }

			int  scan_block() 	 const noexcept { return scan_.bbi_; }
			int  scan_bit()	  const noexcept { return scan_.pos_; }

		//////////////////
		// terminating and other handlers
		[[noreturn]]
			inline void invalid_scan_type() noexcept {
				std::fputs("Unknown scan type in BBScan::init_scan\n", stderr);
				std::terminate();
			}

			/**
			 * @brief Checks if the current scan cursor is valid.
			 * @return true if the cursor is valid, false otherwise.
			 */
			bool has_valid_cursor() const noexcept {
				return scan_.bbi_ >= 0 &&
					scan_.bbi_ < nBB_ &&
					scan_.pos_ >= 0 &&
					scan_.pos_ <= MASK_LIM;
			}

		//////////////////
		/// data members
	
			mutable scan_t scan_;
		};

} //namespace bitgraph


///////////////////
// convenient helpers for BBScan construction
// (part of the public API)

namespace bitgraph {

	/**
	 * @brief Creates a BBScan object given a maximum number of bits  @p nPop (all bits set to 0).
	 **/
	inline
		BBScan make_BBScan(int nPop) { return BBScan(nPop, false); }

	/**
	 * @brief Creates a BBScan object given a maximum number of bits  @p nPop (all bits set to 1).
	 **/
	inline
		BBScan make_BBScan_full(int nPop) { return BBScan(nPop, true); }

	/**
	* @brief Creates a BBScan object given a maximum @p nPop and a list of values in brackets
	* @details: - negative values are ignored (asserted in debug mode).
	*			- values >= nPop are ignored.
	**/
	inline
		BBScan make_BBScan(int nPop, std::initializer_list<int> lv) { return BBScan(nPop, lv); }


} // namespace bitgraph


//////////////////////////
// INLINE Implementation for generic code, should be in header file
#include "detail/bbscan_imp.h"



#endif // BITGRAPH_BITSCAN_BBSCAN_H_






