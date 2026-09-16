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

#include "bbset.h"	
#include <cassert>
#include <type_traits>

namespace bitgraph{
		
		/**
		 * @brief Compile-time-configurable view for scanning the set bits of a Bitset.
		 *
		 * The @p ScanType template parameter determines the traversal direction
		 * and whether the scan is destructive. Fixing the scanning mode at compile
		 * time avoids run-time dispatch.
		 *
		 * In destructive modes, visited bits are cleared from the referenced Bitset.
		 *
		 * @tparam ScanType Scanning mode, including direction and destructive behavior.
		 *
		 * @note The referenced Bitset must remain valid throughout the lifetime
		 *       of this object.
		 */
		template <BBObject::scan_types ScanType = BBObject::NON_DESTRUCTIVE>
		class BBScanViewT {

			static_assert(
				ScanType == BBObject::NON_DESTRUCTIVE ||
				ScanType == BBObject::NON_DESTRUCTIVE_REVERSE ||
				ScanType == BBObject::DESTRUCTIVE ||
				ScanType == BBObject::DESTRUCTIVE_REVERSE,
				"Invalid BBScanViewT scanning mode"
				);

		public:
			explicit BBScanViewT(Bitset& bitset) noexcept : bitset_(bitset) {}
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

			/**
			 * @brief Returns the next set bit while scanning in reverse order.
			 *
			 * Searches for the next set bit in decreasing bit-position order. For a
			 * destructive scan, the returned bit is also cleared from the referenced
			 * bitset.
			 *
			 * @pre The scan has been initialized and has not already been exhausted.
			 *
			 * @return The position of the next set bit, or BBObject::noBit if this call
			 *         reaches the end of the scan.
			 *
			 * @warning After this function returns BBObject::noBit, it must not be called
			 *          again until the scan has been reinitialized.
			 */
			int prev_bit();

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

			int next_bit_dispatch(std::true_type) { return next_bit_destructive_impl(); }
			int next_bit_dispatch(std::false_type) { return next_bit_impl(); }
			int prev_bit_dispatch(std::true_type) { return prev_bit_destructive_impl(); }
			int prev_bit_dispatch(std::false_type) { return prev_bit_impl(); }

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

			Bitset& bitset_;
			BBObject::scan_t scan_;
		};	


		// Convenient aliases for the supported scanning modes

		using BBScanForward =
			BBScanViewT<BBObject::NON_DESTRUCTIVE>;

		using BBScanReverse =
			BBScanViewT<BBObject::NON_DESTRUCTIVE_REVERSE>;

		using BBScanDestructive =
			BBScanViewT<BBObject::DESTRUCTIVE>;

		using BBScanDestructiveReverse =
			BBScanViewT<BBObject::DESTRUCTIVE_REVERSE>;


} //namespace bitgraph


///////////////////////
// INLINE Implementation for generic code; must be in header file
#include "bbscan_view_imp.h"


#endif // _BITGRAPH_BITSCAN_BBSCAN_VIEW_H_






