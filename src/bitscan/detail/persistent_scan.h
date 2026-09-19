/**
 * @file persistent_scan.h
 * @brief Lightweight scanners using scan state stored in the source bitset.
 *
 * The scanner classes in this file provide forward, reverse, destructive,
 * and destructive-reverse traversal over set bits.
 *
 * Unlike BBScanViewT, these scanners do not own an independent cursor. They
 * use the persistent scan cursor stored in the referenced bitset to minimize
 * overhead in performance-critical bit-parallel graph algorithms.
 *
 * @warning Only one persistent scan may be active on a given bitset at a
 *          time. Constructing or reinitializing another persistent scanner
 *          on the same bitset invalidates any scan already in progress.
 */

#ifndef BITGRAPH_BITSCAN_DETAIL_PERSISTENT_SCAN_H
#define BITGRAPH_BITSCAN_DETAIL_PERSISTENT_SCAN_H

#include "bitscan/bbobject.h"

namespace bitgraph {
    namespace detail {

		/**
		 * @brief Forward scanner using the persistent cursor of a bitset.
		 *
		 * Set bits are returned in increasing bit-position order. The scanner uses
		 * the cursor stored in the referenced bitset and therefore does not maintain
		 * independent scan state.
		 *
		 * @tparam BitsetT Bitset type supporting the persistent scanning interface.
		 *
		 * @warning Only one persistent scanner may be active on the referenced bitset.
		 * @note The referenced bitset must remain valid throughout the scanner's
		 *       lifetime.
		 */
        template<class BitsetT>
        class PersistentScan {
			using bitset_type = BitsetT;		// basic type (a type of bitset)
			using bitset_t = bitset_type;		// alias less verbose

		public:

			/**
			 * @brief Constructs and initializes a forward scanner.
			 *
			 * @param bb Bitset to scan.
			 * @param firstBit Initial bit position, or BBObject::noBit to scan from the
			 *                 beginning of the bitset.
			 */
			explicit PersistentScan(bitset_t& bb, int firstBit = BBObject::noBit) : bb_(bb) { init_scan(firstBit); }

			int get_block() const noexcept { return bb_.scan_.bbi_; }

			/**
			 * @brief Initializes a forward scan.
			 *
			 * The scan proceeds in increasing bit-position order, beginning at
			 * @p firstBit. Passing BBObject::noBit selects the beginning of the bitset.
			 *
			 * @param firstBit Initial bit position, or BBObject::noBit to use the
			 *                 default forward-scan position.
			 *
			 * @warning Reinitializing the scan invalidates any other persistent scanner
			 *          currently operating on the same bitset.
			 */
			void init_scan(int firstBit = BBObject::noBit) noexcept { bb_.init_scan(firstBit, BBObject::NON_DESTRUCTIVE); }

			/**
			 * @brief Returns the next set bit in forward order.
			 *
			 * @return Position of the next set bit, or BBObject::noBit when the scan
			 *         reaches its end.
			 *
			 * @pre The scan has been initialized and has not already returned
			 *      BBObject::noBit.
			 */
			int next_bit() { return bb_.next_bit(); }

			/**
			 * @brief Returns the next set bit and clears its position in another bitset.
			 *
			 * The scan cursor belongs to the source bitset supplied to the constructor.
			 * The corresponding returned position is cleared from @p bitset.
			 *
			 * @param bitset Bitset from which the returned position is cleared.
			 *
			 * @return Position of the next set bit, or BBObject::noBit when the scan
			 *         reaches its end.
			 *
			 * @pre The scan has been initialized and has not already returned
			 *      BBObject::noBit.
			 */
			int next_bit(bitset_t& bitSet) { return bb_.next_bit(bitSet); }

		private:
			bitset_t& bb_;
        };


		/**
		 * @brief Reverse scanner using the persistent cursor of a bitset.
		 *
		 * Set bits are returned in decreasing bit-position order. The scanner uses
		 * the cursor stored in the referenced bitset and therefore does not maintain
		 * independent scan state.
		 *
		 * @tparam BitsetT Bitset type supporting the persistent scanning interface.
		 *
		 * @warning Only one persistent scanner may be active on the referenced bitset.
		 * @note The referenced bitset must remain valid throughout the scanner's
		 *       lifetime.
		 */
        template<class BitsetT>
        class PersistentScanReverse {
			using bitset_type = BitsetT;		// basic type (a type of bitset)
			using bitset_t = bitset_type;		// alias less verbose

		public:

			/**
			 * @brief Constructs and initializes a reverse scanner.
			 *
			 * @param bb Bitset to scan.
			 * @param firstBit Initial bit position, or BBObject::noBit to scan from the
			 *                 end of the bitset.
			 */
			explicit PersistentScanReverse(bitset_t& bb, int firstBit = BBObject::noBit) : bb_(bb) { init_scan(firstBit); }

			int get_block() const noexcept { return bb_.scan_.bbi_; }

			/**
			 * @brief Initializes a reverse scan.
			 *
			 * The scan proceeds in decreasing bit-position order, beginning at
			 * @p firstBit. Passing BBObject::noBit selects the end of the bitset.
			 *
			 * @param firstBit Initial bit position, or BBObject::noBit to use the
			 *                 default reverse-scan position.
			 *
			 * @warning Reinitializing the scan invalidates any other persistent scanner
			 *          currently operating on the same bitset.
			 */
			void init_scan(int firstBit = BBObject::noBit) noexcept { bb_.init_scan(firstBit, BBObject::NON_DESTRUCTIVE_REVERSE); }

			/**
			 * @brief Returns the next set bit in reverse order.
			 *
			 * @return Position of the next set bit, or BBObject::noBit when the scan
			 *         reaches its end.
			 *
			 * @pre The scan has been initialized and has not already returned
			 *      BBObject::noBit.
			 */
			int next_bit() { return bb_.prev_bit(); }

			/**
			 * @brief Returns the next set bit in reverse order and clears its position
			 *        in another bitset.
			 *
			 * @param bitset Bitset from which the returned position is cleared.
			 *
			 * @return Position of the next set bit, or BBObject::noBit when the scan
			 *         reaches its end.
			 *
			 * @pre The scan has been initialized and has not already returned
			 *      BBObject::noBit.
			 */
			int next_bit(bitset_t& bitSet) { return bb_.prev_bit(bitSet); }

		private:
			bitset_t& bb_;
        };

		/**
		 * @brief Destructive forward scanner using the persistent bitset cursor.
		 *
		 * Set bits are returned in increasing bit-position order and removed from
		 * the scanned bitset as they are returned.
		 *
		 * @tparam BitsetT Bitset type supporting the persistent destructive-scanning
		 *                 interface.
		 *
		 * @warning Scanning modifies the referenced bitset.
		 * @warning Only one persistent scanner may be active on the referenced bitset.
		 * @note The referenced bitset must remain valid throughout the scanner's
		 *       lifetime.
		 */
        template<class BitsetT>
        class PersistentScanDestructive {

			using bitset_type = BitsetT;		// basic type (a type of bitset)
			using bitset_t = bitset_type;		// alias less verbose
		public:

			/**
			 * @brief Constructs and initializes a destructive forward scanner.
			 *
			 * @param bb Bitset to scan and modify.
			 */
			explicit PersistentScanDestructive(bitset_t& bb) : bb_(bb) { init_scan(); }

			int get_block() const noexcept { return bb_.scan_.bbi_; }

			/**
			 * @brief Initializes a destructive forward scan.
			 *
			 * The scan begins at the start of the bitset. Each returned bit is removed
			 * from the scanned bitset.
			 *
			 * @warning Reinitializing the scan invalidates any other persistent scanner
			 *          currently operating on the same bitset.
			 */
			void init_scan() noexcept { bb_.init_scan(BBObject::DESTRUCTIVE); }

			/**
			* @brief returns the next bit in the bitset during a reverse bitscanning operation
			**/
			int next_bit() { return bb_.next_bit_del(); }

			/**
			* @brief scans the next bit in the bitset and deletes it from the
			*		 bitstring bitSet
			* @param bbdel: bitset to delete the bit from
			**/
			int next_bit(bitset_t& bitSet) { return bb_.next_bit_del(bitSet); }

		private:
			bitset_t& bb_;
        };

        template<class BitsetT>
        class PersistentScanDestructiveReverse {
			using bitset_type = BitsetT;		// basic type (a type of bitset)
			using bitset_t = bitset_type;		// alias less verbose

		public:

			/**
			* @brief: constructor for destructive reverse bitscanning - may throw for sparse bitsets if empty
			**/
			explicit PersistentScanDestructiveReverse(bitset_t& bb) : bb_(bb) { init_scan(); }

			int get_block() const noexcept { return bb_.scan_.bbi_; }

			/**
			* @brief Scans the bitset in the range (end_of_bitset, 0]
			*		 Removes bits as they are scanned
			* @details: may throw for sparse bitsets if empty
			**/
			void init_scan() noexcept { bb_.init_scan(BBObject::DESTRUCTIVE_REVERSE); }

			/**
			* @brief returns the next bit in the bitset during a reverse bitscanning operation
			**/
			int next_bit() { return bb_.prev_bit_del(); }

			/**
			* @brief scans the next bit in the bitset during a reverse bitscanning operation
			*		 and deletes it from the bitstring  bitSet
			* @param bbdel: bitset to delete the bit from
			**/
			int next_bit(bitset_t& bitSet) { return bb_.prev_bit_del(bitSet); }

		private:
			bitset_t& bb_;
        };

    } // namespace detail
} // namespace bitgraph

#endif // BITGRAPH_BITSCAN_DETAIL_PERSISTENT_SCAN_H	