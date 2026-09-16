#ifndef BITGRAPH_DETAIL_PERSISTENT_SCAN_HPP
#define BITGRAPH_DETAIL_PERSISTENT_SCAN_HPP

#include "bitscan/bbobject.h"

namespace bitgraph {
    namespace detail {

        /**
         * @brief Forward scanner using the persistent cursor of a bitset.
         *
         * @tparam BitsetT Bitset type supporting the persistent scanning interface.
         */
        template<class BitsetT>
        class PersistentScan {
			using bitset_type = BitsetT;		// basic type (a type of bitset)
			using bitset_t = bitset_type;		// alias less verbose

		public:

			/**
			* @brief: constructor for bitscanning - may throw for sparse bitsets if empty
			**/
			explicit PersistentScan(bitset_t& bb, int firstBit = BBObject::noBit) : bb_(bb) { init_scan(firstBit); }

			int get_block() const noexcept { return bb_.scan_.bbi_; }

			/**
			* @brief Scans the bitset from [firstBit , end of the bitset)
			*		 If firstBit = -1 scans the whole bitset
			* @param firstBit: starting position of the scan
			* @details: may throw for sparse bitsets if empty
			**/
			void init_scan(int firstBit = BBObject::noBit) noexcept { bb_.init_scan(firstBit, BBObject::NON_DESTRUCTIVE); }

			/**
			* @brief returns the next bit in the bitset during a reverse bitscanning operation
			**/
			int next_bit() { return bb_.next_bit(); }

			/**
			* @brief scans the next bit in the bitset and deletes it from the
			*		 bitstring bitSet
			* @param bbdel: bitset to delete the bit from
			**/
			int next_bit(bitset_t& bitSet) { return bb_.next_bit(bitSet); }

		private:
			bitset_t& bb_;
        };

        template<class BitsetT>
        class PersistentScanReverse {
			using bitset_type = BitsetT;		// basic type (a type of bitset)
			using bitset_t = bitset_type;		// alias less verbose

		public:

			/**
			* @brief: constructor for reverse bitscanning - may throw for sparse bitsets if empty
			**/
			explicit PersistentScanReverse(bitset_t& bb, int firstBit = BBObject::noBit) : bb_(bb) { init_scan(firstBit); }

			int get_block() const noexcept { return bb_.scan_.bbi_; }

			/**
			* @brief Scans the bitset from [firstBit , end of the bitset)
			*		 If firstBit = -1 scans the whole bitset
			* @param firstBit: starting position of the scan
			* @details: may throw for sparse bitsets if empty
			**/
			void init_scan(int firstBit = BBObject::noBit) noexcept { bb_.init_scan(firstBit, BBObject::NON_DESTRUCTIVE_REVERSE); }

			/**
			* @brief returns the next bit in the bitset during a reverse bitscanning operation
			**/
			int next_bit() { return bb_.prev_bit(); }

			/**
			* @brief scans the next bit in the bitset and deletes it from the
			*		bitstring bitSet
			* @param bbdel: bitset to delete the bit from
			**/
			int next_bit(bitset_t& bitSet) { return bb_.prev_bit(bitSet); }

		private:
			bitset_t& bb_;
        };

        template<class BitsetT>
        class PersistentScanDestructive {

			using bitset_type = BitsetT;		// basic type (a type of bitset)
			using bitset_t = bitset_type;		// alias less verbose
		public:

			/**
			* @brief: constructor for destructive bitscanning - may throw for sparse bitsets if empty
			**/
			explicit PersistentScanDestructive(bitset_t& bb) : bb_(bb) { init_scan(); }

			int get_block() const noexcept { return bb_.scan_.bbi_; }

			/**
			* @brief Scans the bitset in the range [0 , end of the bitset)
			*		 Removes bits as they are scanned
			* @details: may throw for sparse bitsets if empty
			**/
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

#endif // BITGRAPH_DETAIL_PERSISTENT_SCAN_HPP