// BitBoardN.cpp: implementation of the BitBoardN class.
//
//////////////////////////////////////////////////////////////////////

#include "bitboardn.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <utils/logger.h>

 
using namespace std;

BitBoardN&  AND (const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res){

	for(auto i = 0; i < lhs.nBB_; ++i){
		res.vBB_[i] = lhs.vBB_[i] & rhs.vBB_[i];
	}

	return res;
}

template<bool EraseAll = false>
BitBoardN& AND(int firstBit, int lastBit, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res)
{

	//////////////////////////////
	assert(firstBit <= lastBit && firstBit > 0);
	//////////////////////////////

	int bbl = WDIV(firstBit);
	int bbh = WDIV(lastBit);


	//clears all bits if required - could be optimized
	if (EraseAll) {
		erase_bit();
	}

	if (bbl == bbh)
	{
		res.vBB_[bbh] = lhs.vBB_[bbh] & rhs.vBB_[bbh] & bblock::MASK_1(firstBit - WMUL(bbl), lastBit - WMUL(bbh));
	}
	else
	{
		//set to one the intermediate blocks
		for (int i = bbl + 1; i < bbh; ++i) {
			res.vBB_[i] = lhs.vBB_[i] & rhs.vBB_[i];
		}

		//sets the first and last blocks
		res.vBB_[bbh] = lhs.vBB_[bbh] & rhs.vBB_[bbh] &  bblock::MASK_1_RIGHT(lastBit - WMUL(bbh));
		res.vBB_[bbl] = lhs.vBB_[bbh] & rhs.vBB_[bbh] &  bblock::MASK_1_LEFT(firstBit - WMUL(bbl));

	}

	return res;

}

BitBoardN&  OR	(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res){
	for(auto i = 0; i < lhs.nBB_; ++i){
		res.vBB_[i] = lhs.vBB_[i] | rhs.vBB_[i];
	}

	return res;
}

BitBoardN&  OR (int from, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res){
////////////////////////
// lhs OR rhs, (rhs [from, END[)
	int first_block = WDIV(from);

	for(auto i = 0; i < first_block; ++i){
		res.vBB_[i]=lhs.vBB_[i];
	}
		
	for(auto i = first_block + 1; i < lhs.nBB_; ++i){
		res.vBB_[i] = rhs.vBB_[i] | lhs.vBB_[i];
	}		

	//special case-first block
	res.vBB_[first_block] = lhs.vBB_[first_block] | ( rhs.vBB_[first_block] & ~Tables::mask_right[from - WMUL(first_block)] );
	
	return res;
}

BitBoardN&  OR(int v, bool from, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res) {
	////////////////////////
	// lhs OR rhs, (rhs [v, END[ if from = TRUE,  rhs [0, v[ if from = false)
	//
	// date@26/10/19
	
	int nBB = WDIV(v);
	int pos = WMOD(v);

	if (from) {
		for (auto i = 0; i < nBB; i++) {
			res.vBB_[i] = lhs.vBB_[i];
		}
		for (auto i = nBB + 1; i < lhs.nBB_; i++) {
			res.vBB_[i] = lhs.vBB_[i] | rhs.vBB_[i];
		}

		//critical block
		res.vBB_[nBB] = lhs.vBB_[nBB] | (rhs.vBB_[nBB] & ~Tables::mask_right[pos]);

	}
	else {
		for (auto i = nBB+1; i < lhs.nBB_; i++) {
			res.vBB_[i] = lhs.vBB_[i];
		}

		for (auto i = 0; i < nBB; i++) {
			res.vBB_[i] = lhs.vBB_[i] | rhs.vBB_[i];
		}

		//critical block
		res.vBB_[nBB] = lhs.vBB_[nBB] | (rhs.vBB_[nBB] & ~Tables::mask_left[pos]);
	}
		
	return res;
}


BitBoardN&   AND_block (int firstBlock, int lastBlock, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res){
		
	//////////////////////////////////////////////////////////////////
	assert( (firstBlock >= 0) && (LastBlock < lhs.nBB_) && 
			(firstBlock <= lastBlock) && (rhs.nBB_ == lhs.nBB_) );
	//////////////////////////////////////////////////////////////////

	int last_block = ((lastBlock == -1) ? lhs.nBB_ - 1 : lastBlock);

	for (auto i = firstBlock; i <= last_block; ++i) {
		res.vBB_[i] = rhs.vBB_[i] & lhs.vBB_[i];
	}

	return res;
}


int* AND (int lastBit, const BitBoardN& lhs, const BitBoardN& rhs,  int* res, int& size){
///////////////
// AND circumscribed to vertices [0,last_vertex(
//
// RETURNS result as a set of vertices (res) and number of vertices in the set (size)
//
// OBSERVATIONS: ***Experimental-efficiency
		
	BITBOARD bb;
	int offset;
	size = 0;
	
	int nbb = WDIV(lastBit);
	for(auto i = 0; i < nbb; ++i){
		bb = rhs.vBB_[i] & lhs.vBB_[i];
		offset = WMUL(i);

		while(bb){
			int v = bblock::lsb64_intrinsic(bb);
			res[size++] = offset + v;
			bb ^= Tables::mask[v];
		}

	}

	//trim last
	bb = rhs.vBB_[nbb] & lhs.vBB_[nbb] & Tables::mask_right[WMOD(lastBit)];
	while(bb){
		int v = bblock::lsb64_intrinsic(bb);
		res[size++] = WMUL(nbb) + v;
		bb ^= Tables::mask[v];
	}

	return res;
}


BitBoardN&  ERASE(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res){
/////////////
// removes rhs FROM lhs

	for(auto i = 0; i < lhs.nBB_; ++i){
		res.vBB_[i] = lhs.vBB_[i] &~ rhs.vBB_[i];
	}

	return res;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BitBoardN::BitBoardN(int popsize) :
	nBB_(INDEX_1TO1(popsize))
{
	
	try {
		vBB_.resize(nBB_, 0);
	}
	catch (...) {
		LOG_ERROR("Error during construction - BitBoardN::BitBoardN");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}


BitBoardN::BitBoardN(const vint& v):
	nBB_(EMPTY_ELEM)
{
	try {
		nBB_ = INDEX_0TO1(*(max_element(v.begin(), v.end())));
		vBB_.resize(nBB_, 0);

		for (auto& bit : v) {

			//////////////////
			assert(bit >= 0);
			/////////////////

			//sets bits - no prior erasing
			set_bit(bit);
		}
	}
	catch (...) {
		LOG_ERROR("Error during construction - BitBoardN::BitBoardN()");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}

BitBoardN::BitBoardN (int popsize, const vint& lv):
	nBB_(INDEX_1TO1(popsize))
{

	try {
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
		LOG_ERROR("Error during construction - BitBoardN::BitBoardN()");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}

void BitBoardN::init(int popsize) {

	try {
		nBB_ = INDEX_1TO1(popsize);
		vBB_.resize(nBB_, 0);
	}
	catch (...) {
		LOG_ERROR("Error during allocation - BitBoardN::init");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}

}

void BitBoardN::init(int popsize, const vint& lv){
		
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
		LOG_ERROR("Error during allocation - BitBoardN::init");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}	
}

void BitBoardN::reset(int popsize) {

	try {
		nBB_ = INDEX_1TO1(popsize);
		vBB_.resize(nBB_, 0);
	}
	catch (...) {
		LOG_ERROR("Error during allocation - BitBoardN::reset");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}

}
void BitBoardN::reset(int popsize, const vint& lv) {

	try {
		nBB_ = INDEX_1TO1(popsize);
		vBB_.resize(nBB_, 0);

		//sets bit conveniently
		for (auto& bit : lv) {

			//////////////////
			assert(bit >= 0 && bit < popsize);
			/////////////////

			//sets bits - no prior erasing
			set_bit<false>(bit);

		}
	}
	catch (...) {
		LOG_ERROR("Error during allocation - BitBoardN::reset");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}



//////////////////////////
//
// BITSET OPERATORS
// (size is determined by *this)
/////////////////////////

BitBoardN& BitBoardN::operator &=	(const BitBoardN& bbn){

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] &= bbn.vBB_[i];
	}
				
	return *this;
}

BitBoardN& BitBoardN::operator |=	(const BitBoardN& bbn){
	
	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] |= bbn.vBB_[i];
	}

	return *this;
}

BitBoardN& BitBoardN::operator ^=	(const BitBoardN& bbn) {
	
	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] ^= bbn.vBB_[i];
	}

	return *this;
}

BitBoardN&  BitBoardN::AND_EQUAL_block (int firstBlock, int lastBlock, const BitBoardN& rhs ){

	///////////////////////////////////////////////////////////////////////////////////
	assert( (firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < nBB_));
	/////////////////////////////////////////////////////////////////////////////////

	auto last_block = ((lastBlock == -1) ? nBB_ - 1 : lastBlock);

	for (auto i = firstBlock; i <= last_block; ++i) {
		vBB_[i] &= rhs.vBB_[i];
	}

	return *this;
}

BitBoardN&  BitBoardN::OR_EQUAL_block (int firstBlock, int lastBlock, const BitBoardN& rhs ){

	///////////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < nBB_));
	/////////////////////////////////////////////////////////////////////////////////

	auto last_block = ((lastBlock == -1) ? nBB_ - 1 : lastBlock);

	for (auto i = firstBlock; i < last_block; ++i) {
		vBB_[i] |= rhs.vBB_[i];
	}
	return *this;
}

BitBoardN& BitBoardN::flip (){

	for (auto i = 0; i < nBB_; ++i) {
		vBB_[i] = ~vBB_[i];
	}

	return *this;
}

BitBoardN& BitBoardN::flip_block(int firstBlock, int lastBlock)
{
	
	///////////////////////////////////////////////////////////////////////////////////
	assert((firstBlock >= 0) && (firstBlock <= lastBlock) && (lastBlock < nBB_));
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

std::ostream& BitBoardN::print(std::ostream& o, bool show_pc, bool endl ) const
{
	o << "[";
	
	//scans de bitstring and serializes it to the output stream
	int nBit = EMPTY_ELEM;
	while( (nBit = next_bit(nBit)) != EMPTY_ELEM ){
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

string BitBoardN::to_string ()
{
	ostringstream sstr;

	print(sstr);

	/*sstr << "[";

	this->print();
	int nBit = EMPTY_ELEM;
	while ((nBit = next_bit(nBit)) != EMPTY_ELEM) {
		sstr << nBit << " ";
	}

	sstr << "(" << popcn64() << ")";
	sstr << "]";*/

	return sstr.str();
}


void BitBoardN::to_vector (vint& lv ) const {

	lv.clear();
	lv.reserve(popcn64());		

	int v = EMPTY_ELEM;
	while( (v = next_bit(v)) != EMPTY_ELEM){
		lv.emplace_back(v);
	}
}

BitBoardN::operator vint() const {
	vint result;
	to_vector (result);
	return result;
}


void BitBoardN::to_stack(com::stack_t<int>& s)	const {
	s.erase();

	int v = EMPTY_ELEM;
	while ((v = next_bit(v)) != EMPTY_ELEM) {
		s.push(v);
	}
}


int* BitBoardN::to_C_array (int* lv, std::size_t& size, bool rev) 	{
	size = 0;
	int v = EMPTY_ELEM;

	if (rev) {
		while ((v = prev_bit(v)) != EMPTY_ELEM) {
			lv[size++] = v;
		}
	}
	else {
		while ((v = next_bit(v)) != EMPTY_ELEM) {
			lv[size++] = v;
		}
	}
	return lv;
}


//BitBoardN& BitBoardN::operator =  (const BitBoardN& bbN){
//
//	if(nBB_ != bbN.nBB_){
//		//allocates memory
//		init(bbN.nBB_);		
//	}
//
//	for (auto i = 0; i < nBB_; ++i) {
//		vBB_[i] = bbN.vBB_[i];
//	}
//
//	return *this;
//}




