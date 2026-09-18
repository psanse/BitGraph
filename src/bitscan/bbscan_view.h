 /**
  * @file bbscan_view.h file
  * @brief header file of the BBScanView class from the BITSCAN library.
  *		   Manages efficient bitscanning as a separate view. For hot paths use BBScan.
  * @created 11/09/2026
  * @author pss
  * 
  **/

#ifndef _BITGRAPH_BITSCAN_BBSCAN_VIEW_H_
#define _BITGRAPH_BITSCAN_BBSCAN_VIEW_H_

#include "bbscan.h"	
#include <cassert>
#include <type_traits>

namespace bitgraph{
		
		/**
		* @brief Compile-time-configurable view for scanning set bits.
		*
		* ScanType determines the traversal direction and whether scanning is
		* destructive. Fixing the scanning mode at compile time eliminates per-bit
		* run-time mode dispatch.
		*
		* Each view owns an independent cursor. Multiple non-destructive views may
		* therefore scan the same bitset independently.
		*
		* In destructive modes, returned bits are cleared from the referenced bitset.
		*
		* @tparam BitsetT Bitset type supporting the scan-view interface.
		* @tparam ScanType Scanning mode, including traversal direction and
		*                   destructive behavior.
		*
		* @note The referenced bitset must remain valid throughout the lifetime of
		*       the view.
		*
		* @warning Destructive views must not overlap with other scans accessing the
		*          same bitset.
		*/
		template <class BitsetT, BBObject::scan_types ScanType = BBObject::NON_DESTRUCTIVE>
		class BBScanViewT {

			static_assert(
				ScanType == BBObject::NON_DESTRUCTIVE ||
				ScanType == BBObject::NON_DESTRUCTIVE_REVERSE ||
				ScanType == BBObject::DESTRUCTIVE ||
				ScanType == BBObject::DESTRUCTIVE_REVERSE,
				"Invalid BBScanViewT scanning mode"
				);

		public:
			explicit BBScanViewT(BitsetT& bitset, int firstBit = BBObject::noBit) noexcept : bitset_(bitset) { init_scan(firstBit); }
			~BBScanViewT() = default;

			/**
			* @brief Initializes a scan over the complete bitset.
			*
			* An empty bitset produces an exhausted scan.
			*/
			void init_scan() noexcept;

			/**
			 * @brief Initializes a scan at a specified bit position.
			 *
			 * Passing BBObject::noBit is equivalent to calling init_scan().
			 *
			 * @param firstBit Initial bit position, or BBObject::noBit to use the
			 *                 default position for the configured scanning direction.
			 *
			 * @warning The program terminates if @p firstBit is outside the valid
			 *          range of the bitset.
			 */
			void init_scan(int firstBit) noexcept;

			/**
			 * @brief Returns the next set bit according to the configured scan mode.
			 *
			 * @pre The scan has been initialized and has not already reached its
			 *      exhausted state.
			 *
			 * @return Position of the next set bit, or BBObject::noBit if the current
			 *         call reaches the end of the scan.
			 *
			 * @warning Calling this function again after it has returned
			 *          BBObject::noBit violates the precondition.
			 */
			int next_bit();
			
		private:
			
			static constexpr bool is_reverse_scan() noexcept {
				return (ScanType == BBObject::NON_DESTRUCTIVE_REVERSE || ScanType == BBObject::DESTRUCTIVE_REVERSE);
			}

			static constexpr bool is_destructive_scan() noexcept {
				return (ScanType == BBObject::DESTRUCTIVE || ScanType == BBObject::DESTRUCTIVE_REVERSE);
			}

			void set_scan_block(int bbindex) noexcept { scan_.bbi_ = bbindex; }
			void set_scan_bit(int posbit) noexcept { scan_.pos_ = posbit; }

			int next_bit_impl();
			int next_bit_destructive_impl();
			int prev_bit_impl();
			int prev_bit_destructive_impl();

			// Forward, non-destructive
			int next_bit_dispatch(std::false_type, std::false_type)	{	return next_bit_impl();			}

			// Forward, destructive
			int next_bit_dispatch(std::false_type, std::true_type) 	{	return next_bit_destructive_impl(); }

			// Reverse, non-destructive
			int next_bit_dispatch(std::true_type, std::false_type) 	{	return prev_bit_impl();		}

			// Reverse, destructive
			int next_bit_dispatch(std::true_type, std::true_type)	{	return prev_bit_destructive_impl();	}

			
			void init_scan_dispatch(std::true_type) noexcept {
				set_scan_block(bitset_.num_blocks() - 1);
				set_scan_bit(WORD_SIZE);
			}
			void init_scan_dispatch(std::false_type) noexcept {
				set_scan_block(0);
				set_scan_bit(MASK_LIM);
			}

			bool has_valid_cursor() const noexcept
			{
				if (scan_.bbi_ == BBObject::noBit) {
					return false;
				}

				assert(scan_.bbi_ >= 0);
				assert(scan_.bbi_ < bitset_.num_blocks());
				assert(scan_.pos_ >= 0);
				assert(scan_.pos_ <= (is_reverse_scan() ? WORD_SIZE : MASK_LIM));

				return true;
			}

			// handlers for error conditions; terminate the program if an error occurs
			[[noreturn]]
			void scan_initialization_error() noexcept
			{
				std::fputs("BBScanViewT scan initialization failed\n", stderr);
				std::terminate();
			}

			BitsetT& bitset_;
			BBObject::scan_t scan_;
		};	


		// Convenient aliases for the supported scanning modes
		namespace view {


			// Generic scan-view aliases.

			template<class BitsetT>
			using ForwardView =
				BBScanViewT<BitsetT, BBObject::NON_DESTRUCTIVE>;

			template<class BitsetT>
			using ReverseView =
				BBScanViewT<BitsetT, BBObject::NON_DESTRUCTIVE_REVERSE>;

			template<class BitsetT>
			using DestructiveForwardView =
				BBScanViewT<BitsetT, BBObject::DESTRUCTIVE>;

			template<class BitsetT>
			using DestructiveReverseView =
				BBScanViewT<BitsetT, BBObject::DESTRUCTIVE_REVERSE>;

			// Convenience aliases for the public bitarray implementation.

			using Forward =	ForwardView<BBScan>;

			using Reverse = ReverseView<BBScan>;

			using DestructiveForward = DestructiveForwardView<BBScan>;			

			using DestructiveReverse = DestructiveReverseView<BBScan>;
	
		}


} //namespace bitgraph


///////////////////////
// INLINE Implementation for generic code; must be in header file
#include "detail/bbscan_view_imp.h"


#endif // _BITGRAPH_BITSCAN_BBSCAN_VIEW_H_






