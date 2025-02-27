/**
 * @file bbset.cpp file
 * @brief implementation of the BitSet class for non-sparsearrays of bit
 * @author pss
 **/

#include "bbset.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <utils/logger.h>
 
using namespace std;

BitSet&  AND (const BitSet& lhs, const BitSet& rhs,  BitSet& res){

	for(auto i = 0; i < lhs.nBB_; ++i){
		res.vBB_[i] = lhs.vBB_[i] & rhs.vBB_[i];
	}

	return res;
}

BitSet&  OR	(const BitSet& lhs, const BitSet& rhs,  BitSet& res){

	for(auto i = 0; i < lhs.nBB_; ++i){
		res.vBB_[i] = lhs.vBB_[i] | rhs.vBB_[i];
	}

	return res;
}





//BitSet AND_block(int firstBlock, int lastBlock, BitSet lhs, const BitSet& rhs)
//{
//	////////////////////////////////////////////////////////////////////
//	//assert((firstBlock >= 0) && (LastBlock < lhs.nBB_) &&
//	//	(firstBlock <= lastBlock) && (rhs.nBB_ == lhs.nBB_));
//	////////////////////////////////////////////////////////////////////
//
//	//int last_block = ((lastBlock == -1) ? lhs.nBB_ - 1 : lastBlock);
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



BitSet&  erase_bit(const BitSet& lhs, const BitSet& rhs,  BitSet& res){


	for(auto i = 0; i < lhs.nBB_; ++i){
		res.vBB_[i] = lhs.vBB_[i] &~ rhs.vBB_[i];
	}

	return res;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BitSet::BitSet(int popsize) :
	nBB_(INDEX_1TO1(popsize))
{
	
	try {
		vBB_.assign(nBB_, 0);
	}
	catch (...) {
		LOG_ERROR("Error during construction - BitSet::BitSet");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}


BitSet::BitSet(const vint& v):
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
		LOG_ERROR("Error during construction - BitSet::BitSet()");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}

BitSet::BitSet (int popsize, const vint& lv):
	nBB_(INDEX_1TO1(popsize))
{

	try {
		vBB_.assign(nBB_, 0);
		
		//sets bit conveniently
		for (auto& bit : lv) {

			//////////////////
			assert(bit >= 0 && bit < popsize);
			/////////////////

			//sets bits - no prior erasing
			set_bit(bit);

		}

	}
	catch (...) {
		LOG_ERROR("Error during construction - BitSet::BitSet()");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}

void BitSet::init(int popsize) {

	try {
		nBB_ = INDEX_1TO1(popsize);
		vBB_.resize(nBB_, 0);
	}
	catch (...) {
		LOG_ERROR("Error during allocation - BitSet::init");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}

}

void BitSet::init(int popsize, const vint& lv){
		
	try {
		nBB_ = INDEX_1TO1(popsize); 
		vBB_.resize(nBB_, 0);

		//sets bit conveniently
		for (auto& bit : lv) {

			//////////////////
			assert(bit >= 0 && bit < popsize);
			/////////////////

			//sets bits - no prior erasing
			set_bit(bit);

		}
	}
	catch (...) {
		LOG_ERROR("Error during allocation - BitSet::init");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}	
}

void BitSet::reset(int popsize) {

	try {
		nBB_ = INDEX_1TO1(popsize);
		vBB_.resize(nBB_, 0);
	}
	catch (...) {
		LOG_ERROR("Error during allocation - BitSet::reset");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}

}
void BitSet::reset(int popsize, const vint& lv) {

	try {
		nBB_ = INDEX_1TO1(popsize);
		vBB_.resize(nBB_, 0);

		//sets bit conveniently
		for (auto& bit : lv) {

			//////////////////
			assert(bit >= 0 && bit < popsize);
			/////////////////

			//sets bits - no prior erasing
			set_bit(bit);

		}
	}
	catch (...) {
		LOG_ERROR("Error during allocation - BitSet::reset");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}



//////////////////////////
//
// BITSET OPERATORS
// (size is determined by *this)
/////////////////////////

BitSet& BitSet::operator &=	(const BitSet& bbn){

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] &= bbn.vBB_[i];
	}
				
	return *this;
}

BitSet& BitSet::operator |=	(const BitSet& bbn){
	
	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] |= bbn.vBB_[i];
	}

	return *this;
}

BitSet& BitSet::operator ^=	(const BitSet& bbn) {
	
	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] ^= bbn.vBB_[i];
	}

	return *this;
}


BitSet& BitSet::flip (){

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] = ~vBB_[i];
	}

	return *this;
}

BitSet& BitSet::flip_block(int firstBlock, int lastBlock)
{
	
	///////////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < capacity()));
	/////////////////////////////////////////////////////////////////////////////////

	for (auto i = firstBlock; i < lastBlock; ++i) {
		vBB_[i] = ~vBB_[i];
	}

	return *this;
}

//////////////////////////
//
// BOOLEAN FUNCTIONS
//
//////////////////////////


//////////////////////////
//
// I/O FILES
//
//////////////////////////

std::ostream& BitSet::print(std::ostream& o, bool show_pc, bool endl ) const
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

string BitSet::to_string ()
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


void BitSet::to_vector (vint& lv ) const {

	lv.clear();
	lv.reserve(popcn64());		

	int v = BBObject::noBit;
	while( (v = next_bit(v)) != BBObject::noBit){
		lv.emplace_back(v);
	}
}

BitSet::operator vint() const {
	vint result;
	to_vector (result);
	return result;
}


void BitSet::to_stack(com::stack_t<int>& s)	const {
	s.erase();

	int v = BBObject::noBit;
	while ((v = next_bit(v)) != BBObject::noBit) {
		s.push(v);
	}
}


int* BitSet::to_C_array (int* lv, std::size_t& size, bool rev) 	{
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
	return lv;
}

BitSet& BitSet::set_bit(const vint& lv) {

	//copies elements up to the maximum capacity of the bitstring
	auto maxPopSize = WMUL(nBB_);
	for (auto i = 0; i < lv.size(); ++i) {

		/////////////////////
		assert(lv[i] >= 0);
		////////////////////

		if (lv[i] < maxPopSize /* 1-based*/) {
			set_bit(lv[i]);
		}
	}

	return *this;

}

int find_first_common(const BitSet& lhs, const BitSet& rhs) {

	for (auto i = 0; i < lhs.nBB_; ++i) {
		BITBOARD bb = lhs.vBB_[i] & rhs.vBB_[i];
		if (bb) {
			return bblock::lsb64_intrinsic(bb) + WMUL(i);
		}
	}

	return BBObject::noBit;		//disjoint
}

int find_first_common_block(int firstBlock, int lastBlock, const BitSet& lhs, const BitSet& rhs) {
	
	///////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < lhs.capacity()));
	///////////////////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? rhs.nBB_ - 1 : lastBlock);

	for (auto i = firstBlock; i < last_block; i++) {
		BITBOARD bb = lhs.vBB_[i] & rhs.vBB_[i];
		if (bb) {
			return bblock::lsb64_intrinsic(bb) + WMUL(i);
		}
	}

	return BBObject::noBit;		//disjoint
}

/////////////////
//
// DEPRECATED STATELESS MASKING FUNCTIONS
//
//////////////////

//int* AND(int lastBit, const BitSet& lhs, const BitSet& rhs, int bitset[], int& size) {
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


//BitSet& OR(int from, const BitSet& lhs, const BitSet& rhs, BitSet& res) {
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


//BitSet& OR(int v, bool from, const BitSet& lhs, const BitSet& rhs, BitSet& res) {
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
