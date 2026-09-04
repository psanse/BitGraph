 /**
  * @file bbscan.h file
  * @brief header file of the BBScan class from the BITSCAN library.
  *		   Manages efficient bitscanning. An alternative is using the nested bitscanning classes in BBObject
  * @details Use external scanning feature for commodity. Use this class for specific tuning of bitscans
  * @details last_updated 13/02/2025
  * @author pss
  * 
  * TODO - Compare efficiency with nested bitscanning classes in BBObject (13/02/2025)
  **/

#ifndef __BBSCAN_H__
#define __BBSCAN_H__

#include "bbset.h"	
#include <cassert>

namespace bitgraph{

		/////////////////////////////////
		//
		// Class BBScanView
		//
		// (Efficient bit scanning by composition over Bitset)
		//
		/////////////////////////////////

		class BBScanView {
		public:
			explicit BBScanView(Bitset& bitset) noexcept : bitset_(bitset) {}
			~BBScanView() = default;

			void scan_block(int bbindex) noexcept { scan_.bbi_ = bbindex; }
			void scan_bit(int posbit) noexcept { scan_.pos_ = posbit; }

			int  scan_block() const noexcept { return scan_.bbi_; }
			int  scan_bit() const noexcept { return scan_.pos_; }

			int init_scan(BBObject::scan_types sct) noexcept;
			int init_scan(int firstBit, BBObject::scan_types sct) noexcept;

			int next_bit_del();
			int next_bit_del(Bitset& bitset);
			int next_bit();
			int next_bit(Bitset& bitset);
			int prev_bit();
			int prev_bit(Bitset& bitset);
			int prev_bit_del();
			int prev_bit_del(Bitset& bitset);

		protected:
			bool has_valid_cursor(int maxPosition) const noexcept {
				static_cast<void>(maxPosition);
				if (scan_.bbi_ == BBObject::noBit) {
					return false;
				}
				assert(scan_.bbi_ >= 0 && scan_.bbi_ < bitset_.num_blocks());
				assert(scan_.pos_ >= 0 && scan_.pos_ <= maxPosition);
				return true;
			}

			bool has_compatible_layout(const Bitset& bitset) const noexcept {
				static_cast<void>(bitset);
				assert(bitset.num_blocks() == bitset_.num_blocks());
				return true;
			}

			Bitset& bitset_;
			BBObject::scan_t scan_;
		};

	
		/////////////////////////////////
		//
		// Class BBScan
		// 
		// (Efficient bit scanning)
		// 
		/////////////////////////////////

		class BBScan : public Bitset {
		public :

			template <class U>
			friend struct BBObject::Scan;
			template <class U>
			friend struct BBObject::ScanDest;
			template <class U>
			friend struct BBObject::ScanRev;
			template <class U>
			friend struct BBObject::ScanDestRev;

		public:

			//aliases for bitscanning 
			using scan = typename BBObject::Scan<BBScan>;
			using scanR = typename BBObject::ScanRev<BBScan>;
			using scanD = typename BBObject::ScanDest<BBScan>;
			using scanDR = typename BBObject::ScanDestRev<BBScan>;

			// for basic bitscanning operatins - they are hidden by next_bit()
			using Bitset::next_bit;				

			//////////////////////////////
			//construction / destruction

			//inherit constructors	
			using Bitset::Bitset;

			//TODO...check copy and move assignments 

			~BBScan() = default;

			///////////////////////////////
			//setters and getters

			void scan_block(int bbindex) { scan_.bbi_ = bbindex; }
			void scan_bit(int posbit) { scan_.pos_ = posbit; }

			int  scan_block()	 const { return scan_.bbi_; }
			int  scan_bit()	 const { return scan_.pos_; }
			
			//////////////////////////////
			// Bitscanning (with cached info)

			/**
			* @brief Configures the initial block and bit position for bitscanning
			*		 according to one of the 4 scan types passed as argument
			* @param sct: type of scan
			* @returns 0 if successful, -1 otherwise  (substituted by fail-safe policy in (08/07/2025))
			* @details: fail-safe policy, program terminates with -1 code  if error
			**/
			virtual int init_scan (scan_types sct) noexcept ;

			/**
			* @brief Configures the initial block and bit position for bitscanning
			*		 starting from the bit 'firstBit' onwards, excluding 'firstBit'
			*		 according to one of the 4 scan types passed as argument.
			*		 If firstBit is -1 (BBObject::noBit), the scan starts from the beginning.
			* @param firstBit: starting bit
			* @param sct: type of scan
			* @returns 0 if successful, -1 otherwise (substituted by fail-safe policy in (08/07/2025))
			* @details: fail-safe policy, program terminates with -1 code  if error
			*
			* TODO - no firstBit information is configured for DESTRUCTIVE scan types (08/02/2025)
			**/
			int init_scan(int firstBit, scan_types sct) noexcept ;


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
			virtual	 int next_bit();

		

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
			virtual  int prev_bit();
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


			//////////////////
			/// data members

		protected:
			scan_t scan_;
		};

} //namespace bitgraph

///////////////////////
//
// INLINE Implementation for generic code, must be in header file

namespace bitgraph {

	inline
	int BBScanView::next_bit_del() {

		if (!has_valid_cursor(MASK_LIM)) {
			return BBObject::noBit;
		}

		auto& vBB = bitset_.bitset();
		Ul posInBB;

		for (auto i = scan_.bbi_; i < bitset_.num_blocks(); ++i) {
			const BITBOARD candidates = (i == scan_.bbi_)
				? vBB[i] & Tables::mask_high[scan_.pos_]
				: vBB[i];
			if (_BitScanForward64(&posInBB, candidates)) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				vBB[i] &= ~Tables::mask[posInBB];
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	inline
	int BBScanView::next_bit_del(Bitset& bitset) {

		if (!has_valid_cursor(MASK_LIM) || !has_compatible_layout(bitset)) {
			return BBObject::noBit;
		}

		auto& vBB = bitset_.bitset();
		auto& vBB_del = bitset.bitset();
		assert(has_compatible_layout(bitset));

		Ul posInBB;
		for (auto i = scan_.bbi_; i < bitset_.num_blocks(); ++i) {
			const BITBOARD candidates = (i == scan_.bbi_)
				? vBB[i] & Tables::mask_high[scan_.pos_]
				: vBB[i];
			if (_BitScanForward64(&posInBB, candidates)) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				vBB[i] &= ~Tables::mask[posInBB];
				vBB_del[i] &= ~Tables::mask[posInBB];
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	inline
	int BBScanView::next_bit() {

		if (!has_valid_cursor(MASK_LIM)) {
			return BBObject::noBit;
		}

		auto& vBB = bitset_.bitset();
		Ul posInBB;

		if (_BitScanForward64(&posInBB, vBB[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {
			scan_.pos_ = posInBB;
			return (posInBB + WMUL(scan_.bbi_));
		}

		for (auto i = scan_.bbi_ + 1; i < bitset_.num_blocks(); ++i) {
			if (_BitScanForward64(&posInBB, vBB[i])) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	inline
	int BBScanView::next_bit(Bitset& bitset) {

		if (!has_valid_cursor(MASK_LIM) || !has_compatible_layout(bitset)) {
			return BBObject::noBit;
		}

		auto& vBB = bitset_.bitset();
		auto& vBB_del = bitset.bitset();
		assert(has_compatible_layout(bitset));

		Ul posInBB;
		if (_BitScanForward64(&posInBB, vBB[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {
			scan_.pos_ = posInBB;
			vBB_del[scan_.bbi_] &= ~Tables::mask[posInBB];
			return (posInBB + WMUL(scan_.bbi_));
		}

		for (auto i = scan_.bbi_ + 1; i < bitset_.num_blocks(); ++i) {
			if (_BitScanForward64(&posInBB, vBB[i])) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				vBB_del[i] &= ~Tables::mask[posInBB];
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	inline
	int BBScanView::prev_bit() {

		if (!has_valid_cursor(WORD_SIZE)) {
			return BBObject::noBit;
		}

		auto& vBB = bitset_.bitset();
		Ul posInBB;

		if (_BitScanReverse64(&posInBB, vBB[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {
			scan_.pos_ = posInBB;
			return (posInBB + WMUL(scan_.bbi_));
		}

		for (auto i = scan_.bbi_ - 1; i >= 0; --i) {
			if (_BitScanReverse64(&posInBB, vBB[i])) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	inline
	int BBScanView::prev_bit(Bitset& bitset) {

		if (!has_valid_cursor(WORD_SIZE) || !has_compatible_layout(bitset)) {
			return BBObject::noBit;
		}

		auto& vBB = bitset_.bitset();
		auto& vBB_del = bitset.bitset();
		assert(has_compatible_layout(bitset));

		Ul posInBB;
		if (_BitScanReverse64(&posInBB, vBB[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {
			scan_.pos_ = posInBB;
			vBB_del[scan_.bbi_] &= ~Tables::mask[posInBB];
			return (posInBB + WMUL(scan_.bbi_));
		}

		for (auto i = scan_.bbi_ - 1; i >= 0; --i) {
			if (_BitScanReverse64(&posInBB, vBB[i])) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				vBB_del[scan_.bbi_] &= ~Tables::mask[posInBB];
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	inline
	int BBScanView::prev_bit_del() {

		if (!has_valid_cursor(WORD_SIZE)) {
			return BBObject::noBit;
		}

		auto& vBB = bitset_.bitset();
		Ul posInBB;

		for (auto i = scan_.bbi_; i >= 0; --i) {
			const BITBOARD candidates = (i == scan_.bbi_)
				? vBB[i] & Tables::mask_low[scan_.pos_]
				: vBB[i];
			if (_BitScanReverse64(&posInBB, candidates)) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				vBB[i] &= ~Tables::mask[posInBB];
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	inline
	int BBScanView::prev_bit_del(Bitset& bitset) {

		if (!has_valid_cursor(WORD_SIZE) || !has_compatible_layout(bitset)) {
			return BBObject::noBit;
		}

		auto& vBB = bitset_.bitset();
		auto& vBB_del = bitset.bitset();
		assert(has_compatible_layout(bitset));

		Ul posInBB;
		for (auto i = scan_.bbi_; i >= 0; --i) {
			const BITBOARD candidates = (i == scan_.bbi_)
				? vBB[i] & Tables::mask_low[scan_.pos_]
				: vBB[i];
			if (_BitScanReverse64(&posInBB, candidates)) {
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;
				vBB[i] &= ~Tables::mask[posInBB];
				vBB_del[i] &= ~Tables::mask[posInBB];
				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}

	inline
	int BBScanView::init_scan(BBObject::scan_types sct) noexcept {
		if (sct < BBObject::NON_DESTRUCTIVE || sct > BBObject::DESTRUCTIVE_REVERSE) {
			return -1;
		}

		if (bitset_.num_blocks() <= 0) {
			scan_block(BBObject::noBit);
			scan_bit(MASK_LIM);
			return 0;
		}

		switch (sct) {
		case BBObject::NON_DESTRUCTIVE:
			scan_block(0);
			scan_bit(MASK_LIM);
			break;
		case BBObject::NON_DESTRUCTIVE_REVERSE:
			scan_block(bitset_.num_blocks() - 1);
			scan_bit(WORD_SIZE);
			break;
		case BBObject::DESTRUCTIVE:
			scan_block(0);
			scan_bit(MASK_LIM);
			break;
		case BBObject::DESTRUCTIVE_REVERSE:
			scan_block(bitset_.num_blocks() - 1);
			scan_bit(WORD_SIZE);
			break;
		default:
			return -1;
		}

		return 0;
	}

	inline
	int BBScanView::init_scan(int firstBit, BBObject::scan_types sct) noexcept {

		if (firstBit == BBObject::noBit) {
			return init_scan(sct);
		}
		if (firstBit < 0 || firstBit >= bitset_.num_blocks() * WORD_SIZE) {
			scan_block(BBObject::noBit);
			scan_bit(MASK_LIM);
			return -1;
		}

		const int bbh = WDIV(firstBit);
		switch (sct) {
		case BBObject::NON_DESTRUCTIVE:
		case BBObject::NON_DESTRUCTIVE_REVERSE:
		case BBObject::DESTRUCTIVE:
		case BBObject::DESTRUCTIVE_REVERSE:
			scan_block(bbh);
			scan_bit(WMOD(firstBit));
			break;
		default:
			return -1;
		}

		return 0;
	}

	inline 
	int BBScan::next_bit_del() {

		Ul posInBB;

		for (auto i = scan_.bbi_; i < nBB_; ++i) {

			if (_BitScanForward64(&posInBB, vBB_[i])) {
				//stores the current block
				scan_.bbi_ = i;

				//deletes the current bit before returning
				vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}

		}

		return BBObject::noBit;
	}


	inline 
	int BBScan::next_bit_del(BBScan& bbN_del) {

		Ul posInBB;

		for (auto i = scan_.bbi_; i < nBB_; ++i) {

			if (_BitScanForward64(&posInBB, vBB_[i])) {
				//stores the current block and copies to output
				scan_.bbi_ = i;

				//deletes the current bit before returning
				vBB_[i] &= ~Tables::mask[posInBB];

				//erases from the bitset passed the scanned bit
				bbN_del.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}


	inline 
	int BBScan::next_bit() {

		Ul posInBB;

		//Search for next bit in the last scanned block
		if (_BitScanForward64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;									//current block has not changed, so not stored

			return (posInBB + WMUL(scan_.bbi_));

		}
		else {

			//Searches for next bit in the remaining blocks
			for (auto i = scan_.bbi_ + 1; i < nBB_; ++i) {
				if (_BitScanForward64(&posInBB, vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::noBit;
	}



	inline
	int BBScan::next_bit(BBScan& bitset) {

		Ul posInBB;

		//Search for next bit in the last scanned block
		if (_BitScanForward64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;									//current block has not changed, so not stored	

			//outputs the current block
			//block = scan_.bbi_;

			//deletes the bit from the input bitset
			bitset.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

			return (posInBB + WMUL(scan_.bbi_));
		}
		else {
			//Searches for next bit in the remaining blocks
			for (auto i = scan_.bbi_ + 1; i < nBB_; i++) {
				if (_BitScanForward64(&posInBB, vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					//outputs the current block
					//block = i;

					//deletes the bit from the input bitset
					bitset.vBB_[i] &= ~Tables::mask[posInBB];

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::noBit;
	}


	inline 
	int BBScan::prev_bit() {

		Ul posInBB;

		//Searches for previous bit in the last scanned block
		if (_BitScanReverse64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;									//current block has not changed, so not stored			

			return (posInBB + WMUL(scan_.bbi_));

		}
		else {

			//Searches for previous bit in the remaining blocks
			for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::noBit;
	}


	inline
	int BBScan::prev_bit(BBScan& bitset)
	{

		Ul posInBB;

		//Searches for previous bit in the last scanned block
		if (_BitScanReverse64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {

			//stores the current bit for next call
			scan_.pos_ = posInBB;									//current block has not changed, so not stored			

			//deletes the bit from the input bitset
			bitset.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

			return (posInBB + WMUL(scan_.bbi_));

		}
		else {

			//Searches for previous bit in the remaining blocks
			for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, vBB_[i])) {

					//stores the current block and bit for next call
					scan_.bbi_ = i;
					scan_.pos_ = posInBB;

					//deletes the bit from the input bitset
					bitset.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

					return (posInBB + WMUL(i));
				}
			}
		}

		return BBObject::noBit;
	}



	inline 
	int BBScan::prev_bit_del() {

		Ul posInBB;

		for (auto i = scan_.bbi_; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i])) {

				//stores the current block for the next call
				scan_.bbi_ = i;

				//deletes the current bit from the bitset before returning
				vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}
		return BBObject::noBit;
	}



	inline 
	int BBScan::prev_bit_del(BBScan& bitset) {

		Ul posInBB;

		for (auto i = scan_.bbi_; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i])) {

				//stores the current block for the next call
				scan_.bbi_ = i;

				//deletes the current bit from the bitset before returning
				vBB_[i] &= ~Tables::mask[posInBB];


				//outputs the current block
				//block = i;

				//erases the bit from the input bitset
				bitset.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}

		return BBObject::noBit;
	}


	inline
	int BBScan::init_scan(scan_types sct) noexcept  {

		switch (sct) {
		case NON_DESTRUCTIVE:
			scan_block(0);
			scan_bit(MASK_LIM);
			break;
		case NON_DESTRUCTIVE_REVERSE:
			scan_block(nBB_ - 1);
			scan_bit(WORD_SIZE);		//mask_low[WORD_SIZE] = ONE
			break;
		case DESTRUCTIVE:
			scan_block(0);
			break;
		case DESTRUCTIVE_REVERSE:
			scan_block(nBB_ - 1);
			break;
		default:			
			assert(false && "unknown scan type - BBScan::init_scan");			
		}

		return 0;
	}


	inline
	int BBScan::init_scan(int firstBit, scan_types sct)  noexcept {

		//special case - first bitscan
		if (firstBit == BBObject::noBit) {
			return init_scan(sct);
		}


		int bbh = WDIV(firstBit);
		switch (sct) {
		case NON_DESTRUCTIVE:
		case NON_DESTRUCTIVE_REVERSE:
			scan_block(bbh);
			scan_bit(firstBit - WMUL(bbh) /* WMOD(firstBit) */);
			break;
		case DESTRUCTIVE:
		case DESTRUCTIVE_REVERSE:
			scan_block(bbh);
			break;
		default:
			LOG_ERROR("unknown scan type - BBScan::init_scan");
			assert(false);
			//throw BitScanError("unknown scan type in BBScan::init_scan");		
		}

		return 0;
	}

}//namespace bitgraph


///////////////////
// helpers for BBScan construction

namespace bitgraph {

	/**
	 * @brief Creates a BBScan object given a maximum number of bits  @nPop (all bits set to 0).
	 **/
	inline
	BBScan make_BBScan(int nPop) { return BBScan(nPop, false); }

	/**
	 * @brief Creates a BBScan object given a maximum number of bits  @nPop (all bits set to 1).
	 **/
	inline 
	BBScan make_BBScan_full(int nPop) { return BBScan(nPop, true); }

	/**
	* @brief Creates a BBScan object given a maximum @nPop and a list of values in brackets
	* @details: - negative values are ignored (asserted in debug mode).
	*			- values >= nPop are ignored.
	**/
	inline 
	BBScan make_BBScan(int nPop, std::initializer_list<int> lv) { return BBScan(nPop, lv); }
		

} // namespace bitgraph



#endif






