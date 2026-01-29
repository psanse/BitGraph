/**
 * @file bbset.cpp file
 * @brief implementation of the Bitset class for non-sparsearrays of bit
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


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Bitset::Bitset(std::size_t nPop, bool val) :
	nBB_(static_cast<int>(INDEX_1TO1(nPop)))
{
	
	try {
		vBB_.assign(nBB_, val ? ONE : 0);		
	}
	catch (...) {
		LOG_ERROR("Error during construction - Bitset::Bitset");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}

	//trim last bitblock to ZERO if val = TRUE
	if (val) {
		vBB_.back() &= bblock::MASK_0_HIGH(WMOD(nPop) /*nPop - WMUL(nBB_ - 1)*/);
	}
}


Bitset::Bitset(const vint& v):
	nBB_(BBObject::noBit)
{
	try {
		nBB_ = INDEX_0TO1(*(max_element(v.begin(), v.end())));
		vBB_.assign(nBB_, 0);

		for (auto& bit : v) {

			//////////////////
			assert(bit >= 0);
			/////////////////

			//sets bits - no prior erasing
			set_bit(bit);
		}
	}
	catch (...) {
		LOG_ERROR("Error during construction - Bitset::Bitset()");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}
}

Bitset::Bitset(std::size_t nPop, std::initializer_list<int> l):
	nBB_(static_cast<index_t>(INDEX_1TO1(nPop)))
{
	try {
		vBB_.assign(nBB_, 0);

		//sets bit conveniently
		for (auto& bit : l) {

			//////////////////
			assert(bit >= 0 && bit < static_cast<int>(nPop));
			/////////////////

			//sets bits - no prior erasing
			set_bit(bit);

		}

	}
	catch (...) {
		LOG_ERROR("Error during construction - Bitset::Bitset()");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}
}

void Bitset::init(std::size_t nPop) noexcept {

	try {
		nBB_ = static_cast<index_t>(INDEX_1TO1(nPop));
		vBB_.assign(nBB_, 0);
	}
	catch (...) {
		LOG_ERROR("Error during allocation - Bitset::init");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}

}

void Bitset::init(std::size_t nPop, const vint& lv) noexcept {
		
	try {
		nBB_ = static_cast<index_t>(INDEX_1TO1(nPop));
		vBB_.assign(nBB_, 0);

		//sets bit conveniently
		for (auto& bit : lv) {

			//////////////////
			assert(bit >= 0 && bit < static_cast<int>(nPop));
			/////////////////

			//sets bits - no prior erasing
			set_bit(bit);

		}
	}
	catch (...) {
		LOG_ERROR("Error during allocation - Bitset::init");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}	
}

void Bitset::reset(std::size_t nPop) noexcept {

	try {
		nBB_ = static_cast<index_t>(INDEX_1TO1(nPop));
		vBB_.assign(nBB_, 0);
	}
	catch (...) {
		LOG_ERROR("Error during allocation - Bitset::reset");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}

}
void Bitset::reset(std::size_t nPop, const vint& lv) noexcept {

	try {
		nBB_ = static_cast<index_t>(INDEX_1TO1(nPop));
		vBB_.assign(nBB_, 0);

		//sets bit conveniently
		for (auto& bit : lv) {

			//////////////////
			assert(bit >= 0 && bit < static_cast<index_t>(nPop));
			/////////////////

			//sets bits - no prior erasing
			set_bit(bit);

		}
	}
	catch (...) {
		LOG_ERROR("Error during allocation - Bitset::reset");
		LOG_ERROR("exiting...");
		std::exit(EXIT_FAILURE);
	}
}



//////////////////////////
//
// BITSET OPERATORS
// (size is determined by *this)
/////////////////////////

Bitset& Bitset::operator &=	(const Bitset& bbn){

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] &= bbn.vBB_[i];
	}
				
	return *this;
}

Bitset& Bitset::operator |=	(const Bitset& bbn){
	
	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] |= bbn.vBB_[i];
	}

	return *this;
}

Bitset& Bitset::operator ^=	(const Bitset& bbn) {
	
	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] ^= bbn.vBB_[i];
	}

	return *this;
}


Bitset& Bitset::flip (){

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] = ~vBB_[i];
	}

	return *this;
}

Bitset& Bitset::flip_block(index_t firstBlock, index_t lastBlock)
{
	
	///////////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < num_blocks()));
	/////////////////////////////////////////////////////////////////////////////////

	for (auto i = firstBlock; i < lastBlock; ++i) {
		vBB_[i] = ~vBB_[i];
	}

	return *this;
}


//////////////////////////
//
// I/O FILES
//
//////////////////////////

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


void Bitset::extract (vint& lv ) const {

	lv.clear();
	const int pc = this->count();
	if (pc == 0) return;				
	lv.reserve(pc);		


	int v = BBObject::noBit;
	while( (v = next_bit(v)) != BBObject::noBit){
		lv.emplace_back(v);
	}
}

void Bitset::extract_set(sint& ls) const
{
	ls.clear();
	const int pc = this->count();
	if (pc == 0) return;			//fast exit for empty set
	
	int v = BBObject::noBit;
	while ((v = next_bit(v)) != BBObject::noBit) {
		ls.insert(v);
	}
}

Bitset::operator vint() const {
	vint result;
	extract(result);
	return result;
}

bitgraph::Bitset::operator sint() const
{
	sint result;
	extract_set(result);
	return result;
}


void Bitset::extract_stack(com::FixedStack<int>& s)	const {
	s.clear();

	int v = BBObject::noBit;
	while ((v = next_bit(v)) != BBObject::noBit) {
		s.push(v);
	}
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

Bitset& Bitset::set_bit(const vint& lv) {

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
// friend functions of Bitset
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



	//Bitset AND_block(int firstBlock, int lastBlock, Bitset lhs, const Bitset& rhs)
	//{
	//	////////////////////////////////////////////////////////////////////
	//	//assert((firstBlock >= 0) && (LastBlock < lhs.nBB_) &&
	//	//	(firstBlock <= lastBlock) && (rhs.nBB_ == lhs.nBB_));
	//	////////////////////////////////////////////////////////////////////
	//
	//	//int last_block = ((lastBlock == Bitset::npos) ? lhs.nBB_ - 1 : lastBlock);
	//
	//	//for (auto i = firstBlock; i <= last_block; ++i) {
	//	//	lhs.vBB_[i] &= rhs.vBB_[i];
	//	//}
	//
	//	////set bits to 0 outside the range 
	//	//for (int i = lastBlock + 1; i < lhs.nBB_; ++i) {
	//	//	lhs.vBB_[i] = ZERO;
	//	//}
	//	//for (int i = 0; i < firstBlock; ++i) {
	//	//	lhs.vBB_[i] = ZERO;
	//	//}
	//
	//	//return lhs;
	//}



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

	int find_first_common_block(Bitset::index_t firstBlock, Bitset::index_t lastBlock, const Bitset& lhs, const Bitset& rhs) {

		///////////////////////////////////////////////////////////////////////////////
		assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < lhs.num_blocks()));
		///////////////////////////////////////////////////////////////////////////////

		Bitset::index_t last_block = (lastBlock == Bitset::npos) ? static_cast<Bitset::index_t>(rhs.nBB_ - 1) : lastBlock;

		for (auto i = firstBlock; i <= last_block; i++) {
			BITBOARD bb = lhs.vBB_[i] & rhs.vBB_[i];
			if (bb) {
				return bblock::lsb(bb) + static_cast<int>(WMUL(i));
			}
		}

		return BBObject::noBit;		//disjoint
	}

}//end namespace bitgraph

/////////////////
//
// DEPRECATED STATELESS MASKING FUNCTIONS
//
//////////////////

//int* AND(int lastBit, const Bitset& lhs, const Bitset& rhs, int bitset[], int& size) {
//
//	BITBOARD bb;
//	int offset;
//	size = 0;
//	int nbb = WDIV(lastBit);
//
//	for (auto i = 0; i < nbb; ++i) {
//		bb = rhs.vBB_[i] & lhs.vBB_[i];
//		offset = WMUL(i);
//
//		while (bb) {
//			int v = bblock::lsb64_intrinsic(bb);
//			bitset[size++] = offset + v;
//			bb ^= Tables::mask[v];
//		}
//
//	}
//
//	//trim last
//	bb = rhs.vBB_[nbb] & lhs.vBB_[nbb] & Tables::mask_low[WMOD(lastBit)];
//	while (bb) {
//		int v = bblock::lsb64_intrinsic(bb);
//		bitset[size++] = WMUL(nbb) + v;
//		bb ^= Tables::mask[v];
//	}
//
//	return bitset;
//}


//Bitset& OR(int from, const Bitset& lhs, const Bitset& rhs, Bitset& res) {
//
//	int first_block = WDIV(from);
//
//	for (auto i = 0; i < first_block; ++i) {
//		res.vBB_[i] = lhs.vBB_[i];
//	}
//
//	for (auto i = first_block + 1; i < lhs.nBB_; ++i) {
//		res.vBB_[i] = rhs.vBB_[i] | lhs.vBB_[i];
//	}
//
//	//special case-first block
//	res.vBB_[first_block] = lhs.vBB_[first_block] | (rhs.vBB_[first_block] & ~Tables::mask_low[from - WMUL(first_block)]);
//
//	return res;
//}


//Bitset& OR(int v, bool from, const Bitset& lhs, const Bitset& rhs, Bitset& res) {
//
//
//	int nBB = WDIV(v);
//	int pos = WMOD(v);
//
//	if (from) {
//		for (auto i = 0; i < nBB; i++) {
//			res.vBB_[i] = lhs.vBB_[i];
//		}
//		for (auto i = nBB + 1; i < lhs.nBB_; i++) {
//			res.vBB_[i] = lhs.vBB_[i] | rhs.vBB_[i];
//		}
//
//		//critical block
//		res.vBB_[nBB] = lhs.vBB_[nBB] | (rhs.vBB_[nBB] & ~Tables::mask_low[pos]);
//
//	}
//	else {
//		for (auto i = nBB + 1; i < lhs.nBB_; i++) {
//			res.vBB_[i] = lhs.vBB_[i];
//		}
//
//		for (auto i = 0; i < nBB; i++) {
//			res.vBB_[i] = lhs.vBB_[i] | rhs.vBB_[i];
//		}
//
//		//critical block
//		res.vBB_[nBB] = lhs.vBB_[nBB] | (rhs.vBB_[nBB] & ~Tables::mask_high[pos]);
//	}
//
//	return res;
//}
