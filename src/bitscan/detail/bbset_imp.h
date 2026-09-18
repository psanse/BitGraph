
#ifndef BITGRAPH_BITSCAN_BBSET_IMP_H
#define BITGRAPH_BITSCAN_BBSET_IMP_H

#include "bbset.h"


namespace bitgraph {
		

	///////////////////
	// Friend functions of Bitset
	///////////////////

	template<bool Erase>
	inline
	Bitset& AND(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res)
	{

		//////////////////////////////////////////////
		assert(firstBit >= 0 && firstBit <= lastBit);
		/////////////////////////////////////////////

		Bitset::block_index_t blockL = WDIV(firstBit);
		Bitset::block_index_t blockH = WDIV(lastBit);
		int offsetH = WMOD(lastBit);			//lastBit - WMUL(blockH));
		int offsetL = WMOD(firstBit);			//firstBit - WMUL(blockL);

		//////////////////////////////////
		//Blocks within the range

		//special case - single block
		if (blockL == blockH)
		{
			if (Erase) {
				//overwrites
				res.vBB_[blockH] = lhs.vBB_[blockH] & rhs.vBB_[blockH] & bblock::MASK_1(offsetL, offsetH);
			}
			else {
				//overwrites partially in the closed range
				bblock::copy(offsetL, offsetH, lhs.vBB_[blockH] & rhs.vBB_[blockH], res.vBB_[blockH]);
			}
		}
		else
		{
			//AND intermediate blocks
			for (auto i = blockL + 1; i < blockH; ++i) {
				//overwrites
				res.vBB_[i] = lhs.vBB_[i] & rhs.vBB_[i];
			}

			//updates the last block blockH in the range
			if (Erase) {
				//overwrites
				res.vBB_[blockH] = lhs.vBB_[blockH] & rhs.vBB_[blockH] & bblock::MASK_1_LOW(offsetH);
			}
			else {
				//overwrites blockH partially inside the range (including offsetH)
				bblock::copy_low(offsetH, lhs.vBB_[blockH] & rhs.vBB_[blockH], res.vBB_[blockH]);
			}

			//updates the first block bbl in the range
			if (Erase) {
				//overwrites
				res.vBB_[blockL] = lhs.vBB_[blockL] & rhs.vBB_[blockL] & bblock::MASK_1_HIGH(offsetL);
			}
			else {
				//overwrites bbl partially inside the range (including offsetL)
				bblock::copy_high(offsetL, lhs.vBB_[blockL] & rhs.vBB_[blockL], res.vBB_[blockL]);
			}
		}

		//////////////////////////////////
		//Blocks outside the range

		//set to 0 all bits outside the bitblock range if required
		if (Erase) {
			for (auto i = blockH + 1; i < static_cast<Bitset::block_index_t>(res.nBB_); ++i) {
				res.vBB_[i] = ZERO;
			}

			for (Bitset::block_index_t i = 0; i < blockL; ++i) {
				res.vBB_[i] = ZERO;
			}
		}

		return res;
	}

	template<bool Erase>
	inline
		Bitset& AND_block(Bitset::block_index_t firstBlock, Bitset::block_index_t lastBlock, const Bitset& lhs, const Bitset& rhs, Bitset& res) {

		const auto last_block = (lastBlock == Bitset::npos) ? lhs.nBB_ - 1 : lastBlock;

		//////////////////////////////////////////////////////////////////
		assert((firstBlock >= 0) && (last_block < lhs.num_blocks()) &&
			(firstBlock <= last_block) && (rhs.num_blocks() == lhs.num_blocks()));
		//////////////////////////////////////////////////////////////////


		//AND mask in the range
		for (auto i = firstBlock; i <= last_block; ++i) {
			res.vBB_[i] = rhs.vBB_[i] & lhs.vBB_[i];
		}

		//set bits to 0 outside the range if required
		if (Erase) {
			for (auto i = lastBlock + 1; i < static_cast<Bitset::block_index_t>(lhs.nBB_); ++i) {
				res.vBB_[i] = ZERO;
			}
			for (Bitset::block_index_t i = 0; i < firstBlock; ++i) {
				res.vBB_[i] = ZERO;
			}
		}

		return res;
	}


	template<bool Erase>
	inline
		Bitset& OR(int firstBit, int lastBit, const Bitset& lhs, const Bitset& rhs, Bitset& res)
	{
		//////////////////////////////
		assert(firstBit <= lastBit && firstBit >= 0);
		//////////////////////////////

		int blockL = WDIV(firstBit);
		int blockH = WDIV(lastBit);
		int offsetH = WMOD(lastBit);						//lastBit - WMUL(blockH);
		int offsetL = WMOD(firstBit);						//firstBit - WMUL(blockL);

		//////////////////////////////////
		//Blocks within the range

		//special case - single block
		if (blockL == blockH)
		{
			if (Erase) {
				//overwrites
				res.vBB_[blockH] = (lhs.vBB_[blockH] | rhs.vBB_[blockH]) & bblock::MASK_1(offsetL, offsetH);
			}
			else {
				//overwrites partially in the closed range
				bblock::copy(offsetL, offsetH, lhs.vBB_[blockH] | rhs.vBB_[blockH], res.vBB_[blockH]);
			}
		}
		else
		{
			//AND intermediate blocks
			for (int i = blockL + 1; i < blockH; ++i) {
				//overwrites
				res.vBB_[i] = lhs.vBB_[i] | rhs.vBB_[i];
			}

			//updates the last block blockH in the range
			if (Erase) {
				//overwrites
				res.vBB_[blockH] = (lhs.vBB_[blockH] | rhs.vBB_[blockH]) & bblock::MASK_1_LOW(offsetH);
			}
			else {
				//overwrites blockH partially inside the range (including offsetH)
				bblock::copy_low(offsetH, lhs.vBB_[blockH] | rhs.vBB_[blockH], res.vBB_[blockH]);
			}

			//updates the first block blockL in the range
			if (Erase) {
				//overwrites
				res.vBB_[blockL] = (lhs.vBB_[blockL] | rhs.vBB_[blockL]) & bblock::MASK_1_HIGH(offsetL);
			}
			else {
				//overwrites blockL partially inside the range (including offsetL)
				bblock::copy_high(offsetL, lhs.vBB_[blockL] | rhs.vBB_[blockL], res.vBB_[blockL]);
			}
		}

		//////////////////////////////////
		//Blocks outside the range

		//set to 0 all bits outside the bitblock range if required
		if (Erase) {
			for (auto i = blockH + 1; i < res.nBB_; ++i) {
				res.vBB_[i] = ZERO;
			}

			for (int i = 0; i < blockL; ++i) {
				res.vBB_[i] = ZERO;
			}
		}

		return res;
	}

	template<bool Erase>
	inline
		Bitset& OR_block(Bitset::block_index_t firstBlock, Bitset::block_index_t lastBlock, const Bitset& lhs, const Bitset& rhs, Bitset& res)
	{

		const auto last_block = (lastBlock == Bitset::npos) ? lhs.nBB_ - 1 : lastBlock;

		//////////////////////////////////////////////////////////////////
		assert((firstBlock >= 0) && (lastBlock < lhs.nBB_) &&
			(firstBlock <= lastBlock) && (rhs.num_blocks() == lhs.num_blocks()));
		//////////////////////////////////////////////////////////////////


		//AND mask in the range
		for (Bitset::block_index_t i = firstBlock; i <= last_block; ++i) {
			res.vBB_[i] = rhs.vBB_[i] | lhs.vBB_[i];
		}

		//set bits to 0 outside the range if required
		if (Erase) {
			for (Bitset::block_index_t i = lastBlock + 1; i < lhs.nBB_; ++i) {
				res.vBB_[i] = ZERO;
			}
			for (Bitset::block_index_t i = 0; i < firstBlock; ++i) {
				res.vBB_[i] = ZERO;
			}
		}

		return res;
	}




	///////////////////
	// Template Bitset methods
	///////////////////

	template<class ColT>
	inline
	bitgraph::Bitset::Bitset(std::size_t nPop, const ColT& bits)
	try
	: nBB_(static_cast<int>(INDEX_1TO1(nPop))),
		vBB_(static_cast<std::size_t>(nBB_), BITBOARD{ 0 })
	{
		for (const auto& value : bits) {
			const auto bit = static_cast<bit_index_t>(value);

			assert(bit >= 0);
			assert(static_cast<std::size_t>(bit) < nPop);

			if (bit < 0 || static_cast<std::size_t>(bit) >= nPop) {
				LOGG_ERROR("Invalid bit position: ", bit, "- Bitset::Bitset(nPop, ColT)");
				std::terminate();
			}

			set_bit(static_cast<bit_t>(bit));
		}
	}
	catch (const std::bad_alloc& e) {
		LOG_ERROR("Bitset construction failed: out of memory - Bitset::Bitset(nPop, ColT)");
		LOG_ERROR(e.what());
		std::terminate();
	}
	catch (const std::exception& e) {
		LOG_ERROR("Bitset construction failed: std::exception - Bitset::Bitset(nPop, ColT)");
		LOG_ERROR(e.what());
		std::terminate();
	}
	catch (...) {
		LOG_ERROR("Bitset construction failed: unknown error - Bitset::Bitset(nPop, ColT)");
		std::terminate();
	}



	template<bool Erase>
	inline
	Bitset& Bitset::AND_EQUAL_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& rhs) {

		const auto last_block = (lastBlock == Bitset::npos) ? nBB_ - 1 : lastBlock;

		///////////////////////////////////////////////////////////////////////////////////
		assert((firstBlock >= 0) && (firstBlock <= last_block) && (last_block < rhs.num_blocks()));
		/////////////////////////////////////////////////////////////////////////////////


		for (auto i = firstBlock; i <= last_block; ++i) {
			this->vBB_[i] &= rhs.vBB_[i];
		}

		//set bits to 0 outside the range if required
		if (Erase) {
			for (auto i = last_block + 1; i < nBB_; ++i) {
				vBB_[i] = ZERO;
			}
			for (block_index_t i = 0; i < firstBlock; ++i) {
				vBB_[i] = ZERO;
			}
		}

		return *this;
	}

	template<bool Erase>
	inline
	Bitset& Bitset::OR_EQUAL_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& rhs) {

		auto last_block = (lastBlock == Bitset::npos) ? static_cast<block_index_t>(nBB_ - 1) : lastBlock;

		///////////////////////////////////////////////////////////////////////////////////
		assert(firstBlock <= last_block && last_block < rhs.num_blocks());
		/////////////////////////////////////////////////////////////////////////////////

		for (block_index_t i = firstBlock; i <= last_block; ++i) {
			vBB_[i] |= rhs.vBB_[i];
		}

		//set bits to 0 outside the range if required
		if (Erase) {
			for (block_index_t i = last_block + 1; i < nBB_; ++i) {
				vBB_[i] = ZERO;
			}
			for (block_index_t i = 0; i < firstBlock; ++i) {
				vBB_[i] = ZERO;
			}
		}

		return *this;
	}


	//////////////////
	// template (free) additional factories
	/////////////////

	/**
	* @brief Creates a Bitset given a maximum @nPop and a range of iterators [first, last).
	* @details: - negative values are ignored (asserted in debug mode).
	*			- values >= nPop are ignored.
	**/
	template<class It>
	inline Bitset make_bitset_from(int nPop, It first, It last) {
		Bitset bs(nPop, false);
		for (auto it = first; it != last; ++it) {
			int v = static_cast<int>(*it);

			//////////////////
			assert(v >= 0);
			//////////////////

			if (v >= 0 && v < nPop) bs.set_bit(v);
		}
		return bs;
	}

	/**
	* @brief Creates a Bitset given a maximum @nPop and collection of values
	* @details: - negative values are ignored (asserted in debug mode).
	*			- values >= nPop are ignored.
	**/
	template<class Col>
	inline Bitset make_bitset(int nPop, const Col& lv) {
		return make_bitset_from(nPop, std::begin(lv), std::end(lv));
	}

} // end namespace bitgraph


#endif // BITGRAPH_BITSCAN_BBSET_IMP_H
