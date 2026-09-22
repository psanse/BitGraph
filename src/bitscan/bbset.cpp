/**
 * @file bbset.cpp file
 * @brief implementation of the Bitset class for non-sparse bitarrays
 * @author pss
 **/

#include "bbset.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <utils/logger.h>
 
using namespace std;
using namespace bitgraph;

/////////////////////////////
// BitSet definitions


Bitset::Bitset(std::size_t nPop, bool val)
try
: nBB_(static_cast<int>(INDEX_1TO1(nPop))),
vBB_(nBB_, val ? ONE : ZERO)
{
	// Clear unused high positions in the final block if val = TRUE
	if (val && nPop > 0) {
		const auto usedBits = WMOD(nPop);
		if (usedBits != 0) {
			vBB_.back() &= bblock::MASK_0_HIGH(usedBits);
		}
	}

	////trim last bitblock to ZERO if val = TRUE
	//if (val && nPop > 0) {
	//	vBB_.back() &= bblock::MASK_0_HIGH(nPop - WMUL(nBB_ - 1));			// cannot be /* bblock::MASK_0_HIGH(WMOD(nPop))! */
	//}

}
catch (const std::bad_alloc& e) {
	LOG_ERROR("Bitset construction failed: out of memory - Bitset::Bitset()");
	LOG_ERROR(e.what());
	std::terminate();
}
catch (const std::exception& e) {
	LOG_ERROR("Bitset construction failed: std::exception - Bitset::Bitset()");
	LOG_ERROR(e.what());
	std::terminate();
}
catch (...) {
	LOG_ERROR("Bitset construction failed: unknown error - Bitset::Bitset()");
	std::terminate();
}

Bitset::Bitset(const bit_indices& bits)
try
: nBB_(0)
{
	if (bits.empty()) {
		return;
	}

	const auto maxBit = *std::max_element(bits.begin(), bits.end());
	assert(maxBit >= 0);

	nBB_ = static_cast<int>(INDEX_0TO1(maxBit));
	vBB_.assign(static_cast<std::size_t>(nBB_), ZERO);

	for (const auto bit : bits) {
		assert(bit >= 0);

		if (bit >= 0) {	set_bit(bit);}
	}
}
catch (const std::bad_alloc& e) {
	LOG_ERROR("Bitset construction failed: out of memory - Bitset::Bitset(const bit_indices&)");
	LOG_ERROR(e.what());
	std::terminate();
}
catch (const std::exception& e) {
	LOG_ERROR("Bitset construction failed: std::exception - Bitset::Bitset(const bit_indices&)");
	LOG_ERROR(e.what());
	std::terminate();
}
catch (...) {
	LOG_ERROR("Bitset construction failed: unknown error - Bitset::Bitset(const bit_indices&)");
	std::terminate();
}

Bitset::Bitset(std::size_t nPop, std::initializer_list<int> bits)
try
	: nBB_(static_cast<block_index_t>(INDEX_1TO1(nPop))),
	vBB_(static_cast<std::size_t>(nBB_), ZERO)
{
	for (const int bit : bits) {
		assert(bit >= 0 && bit < static_cast<bit_index_t>(nPop));
				
		if (bit < 0 || bit >= static_cast<bit_index_t>(nPop)) {
			LOG_ERROR("Bitset construction failed: invalid bit index - Bitset::Bitset(nPop, initializer_list)");
			LOG_ERROR("invalid bit is out of [0, nPop)");
			std::terminate();
		}

		set_bit(bit);
	}
}

catch (const std::bad_alloc& e) {
	LOG_ERROR("Bitset construction failed: out of memory - Bitset::Bitset(nPop, initializer_list)");
	LOG_ERROR(e.what());
	std::terminate();
}
catch (const std::exception& e) {
	LOG_ERROR("Bitset construction failed: std::exception - Bitset::Bitset(nPop, initializer_list)");
	LOG_ERROR(e.what());
	std::terminate();
}
catch (...) {
	LOG_ERROR("Bitset construction failed: unknown error - Bitset::Bitset(nPop, initializer_list)");
	std::terminate();
}

void Bitset::reset(std::size_t nPop) noexcept {

	try {
		nBB_ = static_cast<block_index_t>(INDEX_1TO1(nPop));
		vBB_.assign(nBB_, ZERO);
	}
	catch (const std::bad_alloc& e) {
		LOG_ERROR("init failed: out of memory - Bitset::reset(std::size_t nPop)");
		LOG_ERROR(e.what());
		std::terminate();
	}
	catch (const std::exception& e) {
		LOG_ERROR("init failed: std::exception - Bitset::reset(std::size_t nPop)");
		LOG_ERROR(e.what());
		std::terminate();
	}
	catch (...) {
		LOG_ERROR("init failed: unknown error - Bitset::reset(std::size_t nPop)");
		std::terminate();
	}

}

void Bitset::reset(std::size_t nPop, const bit_indices& bits) noexcept {

	try {
		nBB_ = static_cast<block_index_t>(INDEX_1TO1(nPop));
		vBB_.assign(static_cast<std::size_t>(nBB_), BITBOARD{ 0 });

		//sets bit conveniently
		for (const auto bit : bits) {

			assert(bit >= 0 && bit < static_cast<int>(nPop));

			//sets bits - no prior erasing
			set_bit(static_cast<bit_t>(bit));
		}
	}
	catch (...) {
		LOG_ERROR("Error during allocation - Bitset::reset(const std::size_t nPop, const bit_indices& bits)");
		LOG_ERROR("exiting...");
		std::terminate();
	}
}

//////////////////////////
//
// BITSET OPERATORS
// (size is determined by *this)
/////////////////////////

Bitset& Bitset::operator &=	(const Bitset& bbn) noexcept{

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] &= bbn.vBB_[i];
	}
				
	return *this;
}

Bitset& Bitset::operator |=	(const Bitset& bbn) noexcept{
	
	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] |= bbn.vBB_[i];
	}

	return *this;
}

Bitset& Bitset::operator ^=	(const Bitset& bbn) noexcept {
	
	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] ^= bbn.vBB_[i];
	}

	return *this;
}



Bitset& Bitset::flip () noexcept {

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] = ~vBB_[i];
	}

	return *this;
}

Bitset& Bitset::flip_block(block_index_t firstBlock, block_index_t lastBlock) noexcept
{
	
	///////////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < num_blocks()));
	/////////////////////////////////////////////////////////////////////////////////

	for (auto i = firstBlock; i < lastBlock; ++i) {
		vBB_[i] = ~vBB_[i];
	}

	return *this;
}

std::ostream& Bitset::print(std::ostream& o, bool show_pc, bool endl ) const
{
	o << "[";
	
	//scans de bitstring and serializes it to the output stream
	int nBit = BBObject::noBit;
	while( (nBit = next_bit(nBit)) != BBObject::noBit ){
		o << nBit << " ";
	}

	//adds popcount if required
	if(show_pc){
		int pc = popcn64();
		if (pc) {
			o << "(" << popcn64() << ")";
		}
	}
	
	o << "]";

	if (endl) { o << std::endl; }
	return o;
}

string Bitset::to_string ()
{
	ostringstream sstr;

	print(sstr);

	/*sstr << "[";

	this->print();
	int nBit = BBObject::noBit;
	while ((nBit = next_bit(nBit)) != BBObject::noBit) {
		sstr << nBit << " ";
	}

	sstr << "(" << popcn64() << ")";
	sstr << "]";*/

	return sstr.str();
}


void Bitset::extract (bit_indices& lv ) const {

	lv.clear();
	const int pc = this->count();
	if (pc == 0) return;				
	lv.reserve(pc);		


	int v = BBObject::noBit;
	while( (v = next_bit(v)) != BBObject::noBit){
		lv.emplace_back(v);
	}
}

void Bitset::extract_set(bit_index_set& ls) const
{
	ls.clear();
	const int pc = this->count();
	if (pc == 0) return;			//fast exit for empty set
	
	int v = BBObject::noBit;
	while ((v = next_bit(v)) != BBObject::noBit) {
		ls.insert(v);
	}
}

Bitset::operator bit_indices() const {
	bit_indices result;
	extract(result);
	return result;
}

Bitset::operator bit_index_set() const
{
	bit_index_set result;
	extract_set(result);
	return result;
}


void Bitset::extract_array(int* lv, std::size_t& size, bool rev) 	{
	size = 0;
	int v = BBObject::noBit;

	if (rev) {
		while ((v = prev_bit(v)) != BBObject::noBit) {
			lv[size++] = v;
		}
	}
	else {
		while ((v = next_bit(v)) != BBObject::noBit) {
			lv[size++] = v;
		}
	}

}

Bitset& Bitset::set_bit(const bit_indices& lv) {

	//copies elements up to the maximum capacity of the bitstring
	auto maxPopSize = WMUL(nBB_);
	for (auto i = 0u; i < lv.size(); ++i) {

		/////////////////////
		assert(lv[i] >= 0);
		////////////////////

		if (lv[i] < maxPopSize /* 1-based*/) {
			set_bit(lv[i]);
		}
	}

	return *this;

}


///////////////////////
// Friend Bitset functions (non-template) 
//

namespace bitgraph {


	Bitset& AND(const Bitset& lhs, const Bitset& rhs, Bitset& res) {

		for (auto i = 0; i < lhs.nBB_; ++i) {
			res.vBB_[i] = lhs.vBB_[i] & rhs.vBB_[i];
		}

		return res;
	}

	Bitset& OR(const Bitset& lhs, const Bitset& rhs, Bitset& res) {

		for (auto i = 0; i < lhs.nBB_; ++i) {
			res.vBB_[i] = lhs.vBB_[i] | rhs.vBB_[i];
		}

		return res;
	}

	
	bool operator==	(const Bitset& lhs, const Bitset& rhs) {
			return ((lhs.nBB_ == rhs.nBB_) &&	(lhs.vBB_ == rhs.vBB_));
	};

	
	bool operator!=	(const Bitset& lhs, const Bitset& rhs) {
		return !(lhs == rhs);
	}; 

	Bitset& erase_bit(const Bitset& lhs, const Bitset& rhs, Bitset& res) {


		for (auto i = 0; i < lhs.nBB_; ++i) {
			res.vBB_[i] = lhs.vBB_[i] & ~rhs.vBB_[i];
		}

		return res;
	}

	int find_first_common(const Bitset& lhs, const Bitset& rhs) {

		for (auto i = 0; i < lhs.nBB_; ++i) {
			BITBOARD bb = lhs.vBB_[i] & rhs.vBB_[i];
			if (bb) {
				return bblock::lsb(bb) + static_cast<int>(WMUL(i));
			}
		}

		return BBObject::noBit;		//disjoint
	}

	int find_first_common_block(Bitset::block_index_t firstBlock, Bitset::block_index_t lastBlock, const Bitset& lhs, const Bitset& rhs) {

		///////////////////////////////////////////////////////////////////////////////
		assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < lhs.num_blocks()));
		///////////////////////////////////////////////////////////////////////////////

		Bitset::block_index_t last_block = (lastBlock == Bitset::npos) ? static_cast<Bitset::block_index_t>(rhs.nBB_ - 1) : lastBlock;

		for (auto i = firstBlock; i <= last_block; i++) {
			BITBOARD bb = lhs.vBB_[i] & rhs.vBB_[i];
			if (bb) {
				return bblock::lsb(bb) + static_cast<int>(WMUL(i));
			}
		}

		return BBObject::noBit;		//disjoint
	}





	
		int Bitset::find_first_common(const Bitset& rhs) const {

			BITBOARD bb = 0;
			for (auto i = 0; i < nBB_; ++i) {
				if ((bb = (vBB_[i] & rhs.vBB_[i]))) {
					return bblock::lsb64_intrinsic(bb) + WMUL(i);
				}
			}
			return BBObject::noBit;
		}

		int Bitset::msbn64_lup() const {

			union u {
				U16 c[4];
				BITBOARD b;
			} val{};

			//reverse loop (most significant bit block early exit)
			for (int i = nBB_ - 1 ; i >= 0; i--) {
				val.b = vBB_[i];
				if (val.b) {
					if (val.c[3]) return (Tables::msba[3][val.c[3]] + WMUL(i));
					if (val.c[2]) return (Tables::msba[2][val.c[2]] + WMUL(i));
					if (val.c[1]) return (Tables::msba[1][val.c[1]] + WMUL(i));
					if (val.c[0]) return (Tables::msba[0][val.c[0]] + WMUL(i));
				}
			}

			return BBObject::noBit;		//should not reach here
		}

		int Bitset::msbn64_intrin() const
		{
			Ul posInBB;

			for (int i = nBB_ - 1; i >= 0; --i) {

				if (_BitScanReverse64(&posInBB, vBB_[i])) {
					return (posInBB + WMUL(i));
				}
			}

			return BBObject::noBit;
		}

		int Bitset::next_bit(bit_index_t bit) const noexcept {

			//bit = -1 is a special case of early exit
			//typically used in a loop, in the first bitscan call.
			//Determines the least significant bit in the bitsring
			if (bit == BBObject::noBit) {
				return lsb();
			}

			//compute bitlbock of the bit
			int blockL = WDIV(bit);

			//looks for the next bit in the current block
			int pos = bblock::lsb64_de_Bruijn(Tables::mask_high[bit - WMUL(blockL) /*WMOD(bit)*/] & vBB_[blockL]);
			if (pos >= 0) {
				//////////////////////////////
				return (pos + WMUL(blockL));
				////////////////////////////
			}

			//looks in remaining biblocks
			for (auto i = blockL + 1; i < nBB_; ++i) {
				if (vBB_[i]) {
					return(bblock::lsb64_de_Bruijn(vBB_[i]) + WMUL(i));
				}
			}

			//should not reach here
			return BBObject::noBit;
		}

		int Bitset::prev_bit(bit_index_t bit) const noexcept{

			//special case - first bitscan,
			//calls for the most-significant bit in the bitstring
			if (bit == BBObject::noBit) {
				return msb();
			}

			//bitblock of input bit
			int blockH = WDIV(bit);

			//looks for the msb in the (trimmed) current block
			int pos = bblock::msb64_lup(Tables::mask_low[ /*bit - WMUL(blockH)*/  WMOD(bit) ] & vBB_[blockH]);
			if (pos != BBObject::noBit) {
				return (pos + WMUL(blockH));
			}

			//looks for the msb in the remaining blocks
			union u {
				U16 c[4];
				BITBOARD b;
			}val{};

			for (int i = static_cast<int>(blockH - 1) /* must be signed! */; i >= 0; --i)
			{
				val.b = vBB_[i];
				if (val.b) {
					if (val.c[3]) return (Tables::msba[3][val.c[3]] + WMUL(i));
					if (val.c[2]) return (Tables::msba[2][val.c[2]] + WMUL(i));
					if (val.c[1]) return (Tables::msba[1][val.c[1]] + WMUL(i));
					if (val.c[0]) return (Tables::msba[0][val.c[0]] + WMUL(i));
				}
			}

			return BBObject::noBit;		//should not reach here
		}

		bool Bitset::is_empty_block(block_index_t firstBlock, block_index_t lastBlock) const {

			const auto last_block = (lastBlock == Bitset::npos) ? nBB_ - 1 : lastBlock;

			///////////////////////////////////////////////////////////////////////////////
			assert((firstBlock >= 0) && (last_block < num_blocks()) && (firstBlock <= last_block));
			///////////////////////////////////////////////////////////////////////////////


			for (auto i = firstBlock; i <= last_block; ++i) {
				if (vBB_[i]) {
					return false;
				}
			}

			return true;
		}


		bool Bitset::is_disjoint(const Bitset& rhs) const noexcept
		{
			for (auto i = 0; i < nBB_; ++i) {
				if (vBB_[i] & rhs.vBB_[i]) {
					return false;
				}
			}
			return true;
		}


		bool Bitset::is_disjoint(const Bitset& lhs, const Bitset& rhs)	const
		{
			for (auto i = 0; i < nBB_; ++i) {
				if (vBB_[i] & lhs.vBB_[i] & rhs.vBB_[i]) {
					return false;
				}
			}
			return true;
		}


		bool Bitset::is_disjoint_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& rhs)	const {

			const auto last_block = (lastBlock == Bitset::npos) ? nBB_ - 1 : lastBlock;

			///////////////////////////////////////////////////////////////////////////////
			assert((firstBlock >= 0) && (last_block < num_blocks()) && (firstBlock <= last_block));
			///////////////////////////////////////////////////////////////////////////////


			for (auto i = firstBlock; i <= last_block; ++i) {
				if (vBB_[i] & rhs.vBB_[i]) {
					return false;
				}
			}
			return true;
		}



		Bitset& Bitset::set_bit(bit_index_t lastBit, const Bitset& bb_add) {

			block_index_t blockH = WDIV(lastBit);

			for (auto i = 0; i < blockH; ++i) {
				vBB_[i] = bb_add.vBB_[i];
			}

			//copy the appropiate part of the blockH bitblock (including high)
			bblock::copy_low(/*lastBit - WMUL(blockH)*/ WMOD(lastBit), bb_add.vBB_[blockH], this->vBB_[blockH]);


			return *this;
		}


		int  Bitset::is_singleton(bit_index_t firstBit, bit_index_t lastBit) const {

			int blockL = WDIV(firstBit);
			int blockH = WDIV(lastBit);
			int pc = 0;

			//both ends
			if (blockL == blockH) {
				if ((pc = bblock::popc64(vBB_[blockL] & bblock::MASK_1(/*firstBit - WMUL(blockL)*/ WMOD(firstBit), /*lastBit - WMUL(blockH)*/  WMOD(lastBit)))) > 1) {
					return -1;
				}
			}
			else {

				//checks first block
				if ((pc = bblock::popc64(vBB_[blockL] & bblock::MASK_1_HIGH(/*firstBit - WMUL(blockL)*/ WMOD(firstBit)  ))) > 1) {
					return -1;
				}

				//checks intermediate blocks
				for (auto i = blockL + 1; i < blockH; ++i) {
					if (pc += bblock::popc64(vBB_[i]) > 1) {
						return -1;
					}
				}

				//checks last block
				if ((pc += bblock::popc64(vBB_[blockH] & bblock::MASK_1_LOW(/*lastBit - WMUL(blockH) */ WMOD(lastBit)  ))) > 1) {
					return -1;
				}
			}

			//reasons on return value - 0 empty, 1 singleton (-1 early exit)
			if (pc == 0) { return 0; }
			return 1;						//MUST BE singleton
		}


		int  Bitset::find_singleton(bit_index_t firstBit, bit_index_t lastBit, bit_index_t& singleton) const {

			int blockL = WDIV(firstBit);
			int	blockH = WDIV(lastBit);
			int offsetL = WMOD(firstBit);			//firstBit - WMUL(blockL);		
			int offsetH = WMOD(lastBit);			//lastBit - WMOD(blockH);		
			int pc = 0;
			bool vertex_not_found = true;
			singleton = BBObject::noBit;

			//both ends
			if (blockL == blockH) {
				BITBOARD bbl = vBB_[blockL] & bblock::MASK_1(offsetL, offsetH);
				pc = bblock::popc64(bbl);
				if ((pc = bblock::popc64(bbl)) == 1) {
					singleton = bblock::lsb(bbl) + WMUL(blockL);
					/////////
					return 1;
					////////
				}
			}
			else {

				//checks first block
				BITBOARD bbl = vBB_[blockL] & bblock::MASK_1_HIGH(offsetL);

				if ((pc = bblock::popc64(bbl)) > 1) {
					/////////
					return -1;
					/////////
				}
				else if (pc == 1) {
					vertex_not_found = false;
					singleton = bblock::lsb(bbl) + WMUL(blockL);
				}

				//checks intermediate blocks
				for (auto i = blockL + 1; i < blockH; ++i) {
					if ((pc += bblock::popc64(vBB_[i])) > 1) {
						/////////
						return -1;
						/////////
					}
					else if (vertex_not_found && (pc == 1)) {
						singleton = bblock::lsb(vBB_[i]) + WMUL(i);
						vertex_not_found = false;
					}
				}

				//checks last block
				BITBOARD bbh = vBB_[blockH] & bblock::MASK_1_LOW(offsetH);

				if ((pc += bblock::popc64(bbh)) > 1) {
					/////////
					return -1;
					/////////
				}
				else if (vertex_not_found && (pc == 1)) {
					singleton = bblock::lsb(bbh) + WMUL(blockH);
				}
			}

			//reason with pc
			if (pc == 0) { return 0; }

			//must be singleton
			return 1;
		}


		Bitset& Bitset::set_bit(bit_index_t bit) noexcept {

			vBB_[WDIV(bit)] |= Tables::mask[WMOD(bit)];
			return *this;
		}



		Bitset& Bitset::set_bit(bit_index_t firstBit, bit_index_t lastBit) noexcept {

			////////////////////////////////////////////////
			assert(firstBit >= 0 && firstBit <= lastBit);
			///////////////////////////////////////////////

			int blockL = WDIV(firstBit);
			int blockH = WDIV(lastBit);


			if (blockL == blockH)
			{
				vBB_[blockH] |= bblock::MASK_1( /*firstBit - WMUL(blockL)*/ WMOD(firstBit), /*lastBit - WMUL(blockH)*/ WMOD(lastBit));
			}
			else
			{
				//set to one the intermediate blocks
				for (auto i = blockL + 1; i < blockH; ++i) {
					vBB_[i] = ONE;
				}

				//sets the first and last blocks
				vBB_[blockH] |= bblock::MASK_1_LOW(/*lastBit - WMUL(blockH)*/ WMOD(lastBit));
				vBB_[blockL] |= bblock::MASK_1_HIGH(/*firstBit - WMUL(blockL)*/ WMOD(firstBit));

			}

			return *this;
		}
		

		Bitset& Bitset::set_bit(const Bitset& bb_add) {

			/////////////////////////////////
			assert(nBB_ <= bb_add.nBB_);
			/////////////////////////////////

			for (auto i = 0; i < nBB_; ++i) {
				vBB_[i] |= bb_add.vBB_[i];
			}

			return *this;
		}

		Bitset& Bitset::assign_bit(const Bitset& bb_add)
		{
			/////////////////////////////////
			assert(nBB_ <= bb_add.nBB_);
			/////////////////////////////////

			for (auto i = 0; i < nBB_; ++i) {
				vBB_[i] = bb_add.vBB_[i];
			}

			return *this;
		}



		Bitset& Bitset::set_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& bb_add) {

			const auto last_block = (lastBlock == Bitset::npos) ? nBB_ - 1 : lastBlock;

			////////////////////////////////////////////////////////////////////////////////////////////
			assert((firstBlock >= 0) && (last_block < bb_add.num_blocks()) && (firstBlock <= last_block));
			///////////////////////////////////////////////////////////////////////////////////////////


			for (auto i = firstBlock; i <= last_block; ++i) {
				vBB_[i] |= bb_add.vBB_[i];
			}

			return *this;
		}

		Bitset& Bitset::assign_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& bb_add)
		{
			const auto last_block = (lastBlock == Bitset::npos) ? nBB_ - 1 : lastBlock;

			////////////////////////////////////////////////////////////////////////////////////////////
			assert((firstBlock>=0) && (last_block < bb_add.num_blocks()) && (firstBlock <= last_block));
			///////////////////////////////////////////////////////////////////////////////////////////


			for (auto i = firstBlock; i <= last_block; ++i) {
				vBB_[i] = bb_add.vBB_[i];
			}

			return *this;
		}
		

		Bitset& Bitset::erase_bit() noexcept {

			for (auto i = 0; i < nBB_; ++i) {
				vBB_[i] = ZERO;
			}

			return *this;
		}

		Bitset& Bitset::erase_bit(bit_index_t nBit) noexcept {

			vBB_[WDIV(nBit)] &= ~Tables::mask[WMOD(nBit)];
			return *this;
		}



		Bitset& Bitset::erase_bit(bit_index_t firstBit, bit_index_t lastBit) noexcept{

			//general comment: low - WMUL(blockL) = WMOD(blockL) but supposed to be less expensive (CHECK 01/02/25)

			/////////////////////////////////////////////////////////////////
			assert(firstBit >= 0 && (firstBit <= lastBit || lastBit == -1));
			///////////////////////////////////////////////////////////////////

			int blockL = WDIV(firstBit);
			int blockH = (lastBit == BBObject::noBit) ? (nBB_ - 1) : WDIV(lastBit);


			if (blockL == blockH)
			{
				if (lastBit == BBObject::noBit) {
					vBB_[blockH] &= bblock::MASK_0_HIGH(WMOD(firstBit));
				}
				else {
					vBB_[blockH] &= bblock::MASK_0(WMOD(firstBit), WMOD(lastBit));
				}
			}
			else
			{
				//set to one the intermediate blocks
				for (auto i = blockL + 1; i < blockH; ++i) {
					vBB_[i] = ZERO;
				}

				//last bitblock
				if (lastBit == BBObject::noBit) {
					vBB_[blockH] = ZERO;
				}
				else {
					vBB_[blockH] &= bblock::MASK_0_LOW(WMOD(lastBit));
				}

				//first  bitblock
				vBB_[blockL] &= bblock::MASK_0_HIGH(WMOD(firstBit));
			}

			return *this;
		}

		int Bitset::lsbn64_non_intrin() const {
			/////////////////
			// different implementations of lsbn depending on configuration

#ifdef DE_BRUIJN
			for (auto i = 0; i < nBB_; ++i) {
				if (vBB_[i])
#ifdef ISOLANI_LSB
					return(Tables::indexDeBruijn64_ISOL[((vBB_[i] & -vBB_[i]) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT] + WMUL(i));
#else
					return(Tables::indexDeBruijn64_SEP[((vBB_[i] ^ (vBB_[i] - 1)) * bblock::DEBRUIJN_MN_64_SEP/*magic num*/) >>
						bblock::DEBRUIJN_MN_64_SHIFT] + WMUL(i));
#endif
			}
#elif LOOKUP
			union u {
				U16 c[4];
				BITBOARD b;
			};

			u val;

			for (int i = 0; i < nBB_; i++) {
				val.b = vBB_[i];
				if (val.b) {
					if (val.c[0]) return (Tables::lsba[0][val.c[0]] + WMUL(i));
					if (val.c[1]) return (Tables::lsba[1][val.c[1]] + WMUL(i));
					if (val.c[2]) return (Tables::lsba[2][val.c[2]] + WMUL(i));
					if (val.c[3]) return (Tables::lsba[3][val.c[3]] + WMUL(i));
				}
			}

#endif

			return  BBObject::noBit;
		}

		int Bitset::lsbn64_intrin() const
		{
			Ul posInBB;

			for (auto i = 0; i < nBB_; ++i) {
				if (_BitScanForward64(&posInBB, vBB_[i])) {
					return(posInBB + WMUL(i));
				}
			}

			return BBObject::noBit;
		}

		int Bitset::is_singleton() const {

			int pc = 0;
			for (auto i = 0; i < nBB_; ++i) {
				if ((pc += bblock::popc64(vBB_[i])) > 1) {
					return -1;
				}
			}

			//reasons with pc: 1-singleton, 0-empty
			if (pc == 1) { return 1; }

			//must be empty bitset
			return 0;
		}


		int Bitset::is_singleton_block(block_index_t firstBlock, block_index_t lastBlock) const
		{
			const auto last_block = (lastBlock == Bitset::npos) ? nBB_ - 1 : lastBlock;


			///////////////////////////////////////////////////////////////////////////////////
			assert((firstBlock >= 0) && (firstBlock <= last_block) && (last_block < num_blocks()));
			/////////////////////////////////////////////////////////////////////////////////


			int pc = 0;
			for (auto i = firstBlock; i < last_block; ++i) {
				if ((pc += bblock::popc64(vBB_[i])) > 1) {
					return -1;
				}
			}

			//reasons with pc: 1- singleton, 0-empty
			if (pc == 1) { return 1; }

			//must be empty bitset
			return 0;
		}

		int Bitset::popcn64() const noexcept {

			int pc = 0;

			for (auto i = 0; i < nBB_ /*vBB_.size()*/; ++i) {
				pc += bblock::popc64(vBB_[i]);
			}

			return pc;
		}


		int Bitset::popcn64(bit_index_t firstBit, bit_index_t lastBit) const noexcept
		{

			/////////////////////////////////////////////////////////////////
			assert(firstBit > 0 && ((firstBit <= lastBit) || (lastBit == -1)));
			////////////////////////////////////////////////////////////////

			int pc = 0;
			block_index_t blockL = WDIV(firstBit);
			block_index_t blockH = (lastBit == BBObject::noBit) ? static_cast<block_index_t>(nBB_ - 1) : WDIV(lastBit);


			if (blockL == blockH)
			{
				//same block
				pc = bblock::popc64(vBB_[blockL] & bblock::MASK_1(WMOD(firstBit), WMOD(lastBit)));

			}
			else
			{
				//count the population of the intermediate blocks
				for (auto i = blockL + 1; i < blockH; ++i) {
					pc += bblock::popc64(vBB_[i]);
				}

				//count the population of the first and last blocks
				pc += bblock::popc64(vBB_[blockH] & bblock::MASK_1_LOW(WMOD(lastBit)));
				pc += bblock::popc64(vBB_[blockL] & bblock::MASK_1_HIGH(WMOD(firstBit)));

			}

			return pc;
		}

		int Bitset::find_common_singleton(const Bitset& rhs, bit_index_t& bit) const {

			int pc = 0;
			bool is_first_vertex = true;
			bit = BBObject::noBit;

			//main loop
			for (auto i = 0; i < nBB_; ++i) {
				pc += bblock::popc64(vBB_[i] & rhs.vBB_[i]);
				if (pc > 1) {
					bit = BBObject::noBit;
					return -1;
				}
				else if (is_first_vertex && pc == 1) { //stores bit the first time pc == 1 

					bit = bblock::lsb64_intrinsic(vBB_[i] & rhs.vBB_[i]) + WMUL(i);
					is_first_vertex = false;
				}
			}

			//disjoint - pc = 0
			return pc;
		}

		int	Bitset::find_common_singleton_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& rhs, int& bit) const 
		{
			const auto last_block = (lastBlock == Bitset::npos) ? nBB_ - 1 : lastBlock;

			///////////////////////////////////////////////////////////////////////////////
			assert((firstBlock >= 0) && (last_block < num_blocks()) && (firstBlock <= last_block));
			///////////////////////////////////////////////////////////////////////////////


			int pc = 0;
			bit = BBObject::noBit;
			bool is_first_vertex = true;

			for (block_index_t i = firstBlock; i <= last_block; ++i) {
				pc += bblock::popc64(vBB_[i] & rhs.vBB_[i]);
				if (pc > 1) {
					bit = BBObject::noBit;
					return -1;
				}
				else if (is_first_vertex && pc == 1) {	//stores bit the first time pc == 1 

					bit = bblock::lsb64_intrinsic(vBB_[i] & rhs.vBB_[i]) + WMUL(i);
					is_first_vertex = false;
				}
			}

			//pc = 0 (disjoint) /pc = 1 (intersection between *this and rhs a single bit)	
			return pc;
		}


		int Bitset::find_diff_singleton(const Bitset& rhs, bit_index_t& bit) const {

			int pc = 0;
			bit = BBObject::noBit;
			bool is_first_vertex = true;

			for (auto i = 0; i < nBB_; ++i) {

				//popcount of set difference - removes bits of rhs from *this
				pc += bblock::popc64(vBB_[i] & ~rhs.vBB_[i]);

				if (pc > 1) {
					bit = BBObject::noBit;
					return -1;
				}
				else if (pc == 1 && is_first_vertex) { //stores bit the first time pc == 1 

					bit = bblock::lsb64_intrinsic(vBB_[i] & ~rhs.vBB_[i]) + WMUL(i);
					is_first_vertex = false;
				}
			}

			//pc = 0 (*this subset of rhs, empty setdiff) / pc = 1 (singleton setdiff)
			return pc;
		}

		int Bitset::find_diff_pair(const Bitset& rhs, bit_index_t& bit1, bit_index_t& bit2) const {

			int pc = 0;
			bool is_first_bit = true;
			bool is_second_bit = true;
			bit1 = BBObject::noBit;
			bit2 = BBObject::noBit;

			//main loop
			for (auto i = 0; i < nBB_; ++i) {

				//popcount of set difference - removes bits of rhs from *this
				BITBOARD bb = vBB_[i] & ~rhs.vBB_[i];
				pc += bblock::popc64(bb);

				if (pc > 2) {

					bit1 = BBObject::noBit;
					bit2 = BBObject::noBit;
					return BBObject::noBit;

				}
				else if (pc == 1 && is_first_bit) {  //stores bit the first time pc == 1 

					bit1 = bblock::lsb(bb) + WMUL(i);
					is_first_bit = false;

				}
				else if (pc == 2 && is_second_bit) {  //stores the two bits the first time pc == 2 

					if (is_first_bit) {

						//determines the two bits in the same block
						bit1 = bblock::lsb(bb) + WMUL(i);
						bit2 = bblock::msb(bb) + WMUL(i);

					}
					else {

						//determines the second bit directly since the
						//two bits of the set difference are in different bitblocks
						bit2 = bblock::lsb(bb) + WMUL(i);
					}

					is_second_bit = false;
				}
			}

			//pc=0, 1, 2 (size of the set difference)
			return pc;
		}
			

		Bitset& Bitset::erase_bit(const Bitset& bbn) {

			for (auto i = 0; i < nBB_; ++i) {
				vBB_[i] &= ~bbn.vBB_[i];
			}

			return *this;
		}

		Bitset& Bitset::erase_bit(bit_index_t firstBit, bit_index_t lastBit, const Bitset& bbn) noexcept {

			//general comment: low - WMUL(blockL) = WMOD(blockL) but supposed to be less expensive (CHECK 01/02/25)

			/////////////////////////////////////////////////////////////
			assert(firstBit <= lastBit || lastBit == BBObject::noBit);
			/////////////////////////////////////////////////////////////

			block_index_t blockL = WDIV(firstBit);
			block_index_t blockH = (lastBit == BBObject::noBit) ? static_cast<block_index_t>(nBB_ - 1) : WDIV(lastBit);

			//special case - both ends in the same bitblock
			if (blockL == blockH)
			{
				if (lastBit == BBObject::noBit) {
					vBB_[blockH] &= ~(bbn.vBB_[blockH] & bblock::MASK_1_HIGH(WMOD(firstBit)));					
				}
				else {
					vBB_[blockH] &= ~(bbn.vBB_[blockH] & bblock::MASK_1(WMOD(firstBit), WMOD(lastBit)));					
				}
			}
			else
			{
				//set to one the intermediate blocks
				for (auto i = blockL + 1; i < blockH; ++i) {
					vBB_[i] &= ~bbn.vBB_[i];
				}

				//last bitblock
				if (lastBit == BBObject::noBit) {
					vBB_[blockH] &= ~bbn.vBB_[blockH];
				}
				else {
					vBB_[blockH] &= ~(bbn.vBB_[blockH] & bblock::MASK_1_LOW(WMOD(lastBit)));
				}

				//first  bitblock
				vBB_[blockL] &= ~(bbn.vBB_[blockL] & bblock::MASK_1_HIGH(WMOD(firstBit)));
			}
			
			return *this;
		}


		Bitset& Bitset::erase_bit(const Bitset& bb_lhs, const Bitset& bb_rhs) {

			for (auto i = 0; i < nBB_; i++) {
				vBB_[i] &= ~(bb_lhs.vBB_[i] | bb_rhs.vBB_[i]);
			}

			return *this;
		}

		Bitset& Bitset::erase_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& bb_lhs, const Bitset& bb_rhs) {


			block_index_t last_block = (lastBlock == Bitset::npos)? nBB_ - 1 : lastBlock;

			///////////////////////////////////////////////////////////////////////////////
			assert((firstBlock >= 0) && (last_block < num_blocks()) && (firstBlock <= last_block));
			///////////////////////////////////////////////////////////////////////////////
			
			for (auto i = firstBlock; i <= last_block; ++i) {
				vBB_[i] &= ~(bb_lhs.vBB_[i] | bb_rhs.vBB_[i]);
			}

			return *this;
		}

		Bitset& Bitset::erase_block(block_index_t firstBlock, block_index_t lastBlock, const Bitset& bb_del) {

			const auto last_block = (lastBlock == Bitset::npos) ? nBB_ - 1 : lastBlock;

			///////////////////////////////////////////////////////////////////////////////
			assert((firstBlock>=0) && (last_block < bb_del.num_blocks()) && (firstBlock <= last_block));
			///////////////////////////////////////////////////////////////////////////////

			for (auto i = firstBlock; i <= last_block; ++i) {
				vBB_[i] &= ~bb_del.vBB_[i];
			}

			return *this;
		}

		void Bitset::extract_stack(utils::FixedStack<bit_index_t>& s)	const {
			s.clear();

			bit_index_t bit = BBObject::noBit;
			while ((bit = next_bit(bit)) != BBObject::noBit) {
				s.push(bit);
			}
		}

}//end namespace bitgraph


/////////////////////////////////
// -----------------------------------------------------------------------------
// Helpers (free factories) to build `BitSets` (header-only, sin romper API)
// 
// TODO: since they only return a Bitset, implement as named factories in the Bitset class (17/09/2026).
// -----------------------------------------------------------------------------
// Typical use
//   auto bb1 = bitgraph::make_bitset(6);                 // empty, 6 bits
//   auto bb2 = bitgraph::make_bitset(6, {0,3,4});        // 1-bits in 0,3,4
//   std::vector<int> lv = {1,2,5};
//   auto bb3 = bitgraph::make_bitset(6, lv);             // from container
//   auto bb4 = bitgraph::make_bitset_full(6);            // all bits to 1
//
// Notas:
// - nPop is population size ( maximum number of bits).
// - Values outside [0, nPop) are ignored (negative values causes assertion).

namespace bitgraph {

	/**
	* @brief Creates an empty Bitset with a maximum of @nPop bits (all bits set to 0).
	**/
	Bitset make_bitset(int nPop) { return Bitset(nPop, false); }

	/**
	* @brief Creates a Bitset with a maximum @nPop bits (all bits set to 1).
	**/
	Bitset make_bitset_full(int nPop) {
		Bitset bs(nPop, false);
		if (nPop > 0) bs.set_bit(0, nPop - 1);
		return bs;
	}

	///**
	//* @brief Creates a Bitset given a maximum @nPop and a range of iterators [first, last).
	//* @details: - negative values are ignored (asserted in debug mode).
	//*			- values >= nPop are ignored.
	//**/
	//template<class It>
	//inline Bitset make_bitset_from(int nPop, It first, It last) {
	//	Bitset bs(nPop, false);
	//	for (auto it = first; it != last; ++it) {
	//		int v = static_cast<int>(*it);

	//		//////////////////
	//		assert(v >= 0);
	//		//////////////////

	//		if (v >= 0 && v < nPop) bs.set_bit(v);
	//	}
	//	return bs;
	//}

	///**
	//* @brief Creates a Bitset given a maximum @nPop and collection of values
	//* @details: - negative values are ignored (asserted in debug mode).
	//*			- values >= nPop are ignored.
	//**/
	//template<class Col>
	//inline Bitset make_bitset(int nPop, const Col& lv) {
	//	return make_bitset_from(nPop, std::begin(lv), std::end(lv));
	//}


	/**
	* @brief Creates a Bitset given a maximum @nPop and a list of values in brackets
	* @details: - negative values are ignored (asserted in debug mode).
	*			- values >= nPop are ignored.
	**/
	Bitset make_bitset(int nPop, std::initializer_list<int> lv) {
		return make_bitset_from(nPop, lv.begin(), lv.end());
	}

} // namespace bitgraph

