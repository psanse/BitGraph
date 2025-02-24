 /**
  * @file bbset_sparse.cpp
  * @brief implementations for the sparse BitSetSp class wrapper for sparse bitstrings (header bbset_sparse.h)
  * @author pss
  * @details created 10/02/2015?, @last_update 20/02/2025
  *
  * TODO refactoring and testing 15/02/2025 - follow the interface of the refactored BitSet
  **/

#include "bbset_sparse.h"
#include "utils/logger.h"
#include <iostream>
#include <sstream>

//uncomment undef to avoid assertions
#define NDEBUG
#undef NDEBUG
#include <cassert>
 
using namespace std;

////////////////////
//static members

int BitSetSp::nElem = EMPTY_ELEM;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BitSetSp::BitSetSp(int nPop, bool is_popsize ){

	(is_popsize)? nBB_ = INDEX_1TO1(nPop) : nBB_ = nPop;
	vBB_.reserve(DEFAULT_CAPACITY);							
}

BitSetSp::BitSetSp(int nPop, const vint& lv):
	nBB_(INDEX_1TO1(nPop))
{
	try {
		
		//VBB_ initially empty.. with default capacity

		//sets bit conveniently
		for (auto& bit : lv) {

			//////////////////
			assert(bit >= 0 && bit < nPop);
			/////////////////

			//sets bits - adds pBlocks in place
			set_bit(bit);

		}

	}
	catch (...) {
		LOG_ERROR("Error during construction - BitSet::BitSetSp()");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}

}


void BitSetSp::reset(int size, bool is_popsize)
{
	try {
		(is_popsize) ? nBB_ = INDEX_1TO1(size) : nBB_ = size;
		decltype(vBB_)().swap(vBB_);
		vBB_.reserve(DEFAULT_CAPACITY);
	}
	catch (...) {
		LOG_ERROR("Error during reset - BitSet::reset");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}

void BitSetSp::reset(int nPop, const vint& lv)
{

	try {
		
		nBB_ = INDEX_1TO1(nPop);
		decltype(vBB_)().swap(vBB_);
		vBB_.reserve(DEFAULT_CAPACITY);

		//sets bit conveniently
		for (auto& bit : lv) {

			//////////////////
			assert(bit >= 0 && bit < nPop);
			/////////////////

			//sets bits - adds pBlocks in place
			set_bit(bit);

		}
	}
	catch (...) {
		LOG_ERROR("Error during reset - BitSet::reset");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}
}

void BitSetSp::init (int size, bool is_popsize){
	try {
		(is_popsize) ? nBB_ = INDEX_1TO1(size) : nBB_ = size;
		vBB_.clear();
		vBB_.reserve(DEFAULT_CAPACITY);
	}
	catch (...) {
		LOG_ERROR("Error during initialization - BitSet::init");
		LOG_ERROR("exiting...");
		std::exit(-1);
	}	
}

BitSetSp& BitSetSp::set_bit(int firstBit, int lastBit)
{
	auto bbl = WDIV(firstBit);					//block index firstBit
	auto bbh = WDIV(lastBit);					//block index lastBit

	/////////////////////////////////////////////////////////
	assert(firstBit >= 0 && firstBit <= lastBit && bbh<nBB_ );
	////////////////////////////////////////////////////////

	const auto sizeL = vBB_.size();				//stores the original size of *this since it will be enlarged
	auto offsetl = firstBit - WMUL(bbl);		//offset in the lower block
	auto offseth = lastBit - WMUL(bbh);			//offset in the upper block
	bool flag_sort = false;						//flag to sort the collection

	//finds position of block closest to blockID (equal or greater index)
	auto posTHIS = 0;							//block position *this	
	auto itbl = find_block(bbl, posTHIS);
			

	/////////////////////
	// ALGORITHM 	 
	// 1) special case: all existing blocks are outside the range
	// 2) special case: singleton range
	// 3) first block
	// 4) blocks in between analysis
	// 5) last block
	// 6) exit because last block has been reached
	// 7) exit because no more blocks in *this
	////////////////////
 
	//I. special case: all existing blocks are outside the range
	if (posTHIS == BBObject::noBit) {

		if (bbl == bbh) {
			vBB_.emplace_back(pBlock_t(bbl, bblock::MASK_1(offsetl, offseth)));
		}
		else {

			//add blocks trimming both ends 
			vBB_.emplace_back(pBlock_t(bbl, bblock::MASK_1_HIGH(offsetl)));
			vBB_.emplace_back(pBlock_t(bbh, bblock::MASK_1_LOW(offseth)));
			for (int i = bbl + 1; i < bbh; i++) {
				vBB_.emplace_back(pBlock_t(i, ONE));
			}
		}
		
		return *this;
	}

	//II. special case - singleton range
	if (bbl == bbh) {

		if (vBB_[posTHIS].idx_ == bbl) {
			vBB_[posTHIS].bb_ |= bblock::MASK_1(offsetl, offseth);
		}
		else {
						
			if (vBB_[posTHIS].idx_ > bbl) {
						
				//insert block to avoid sorting - special case
				vBB_.insert(itbl, pBlock_t(bbl, bblock::MASK_1(offsetl, offseth)));
			}
			else {

				//places at the end - no sorting required
				vBB_.emplace_back(pBlock_t(bbl, bblock::MASK_1(offsetl, offseth)));
			}			
		}

		return *this;
	}

	//III. first block
	if (vBB_[posTHIS].idx_ == bbl) {
		vBB_[posTHIS].bb_ |= bblock::MASK_1_HIGH(offsetl);		
	}
	else {
		vBB_.emplace_back(pBlock_t(bbl, bblock::MASK_1_HIGH(offsetl)));
		if (vBB_[posTHIS].idx_ > bbl) {
			flag_sort = true;
		}
	}

	////////////////////////////////////////////
	//main loop
	auto block = bbl + 1;
	posTHIS++;
	while (posTHIS < sizeL && block < bbh) {

		if (vBB_[posTHIS].idx_ < block) {

			//add block
			vBB_.emplace_back(pBlock_t(block, ONE));			
			
			posTHIS++;
		}
		else if (vBB_[posTHIS].idx_ > block) {
			
			//add block
			vBB_.emplace_back(pBlock_t(block, ONE));
			flag_sort = true;
	
			block++;
		}
		else {

			//index match - overwrite
			vBB_[posTHIS].bb_ = ONE;				
			
			posTHIS++;
			block++;
		}
		
	}//end while

	/////////////////////
	// Treatment depending on exit condition of MAIN LOPP
	 
	//I. exit because last block has been reached
	if (block == bbh) {
		if (vBB_[posTHIS].idx_ == bbh) {
			vBB_[posTHIS].bb_ |= bblock::MASK_1_LOW(offseth);
		}
		else {
			vBB_.emplace_back(pBlock_t(block, bblock::MASK_1_LOW(offseth)));
			if (vBB_[posTHIS].idx_ > block) {
				flag_sort = true;
			}
		}
	}

	//II. Exit because no more blocks in *this
	//	  Note: no sorting required in this case if blocks are appended at the end
	if (posTHIS == sizeL) {

		//[block, bbh[
		for (int i = block; i < bbh; i++) {
			vBB_.emplace_back(pBlock_t(i, ONE));
		}
		
		//last block - bbh
		vBB_.emplace_back(pBlock_t(bbh, bblock::MASK_1_LOW(offseth)));
	}
	
	//keep the collection sorted if required
	if (flag_sort) {
		sort();
	}

	return *this;
}

BitSetSp& BitSetSp::reset_bit(int firstBit, int lastBit){

	auto bbh = WDIV(lastBit);
	auto bbl = WDIV(firstBit);

	///////////////////////////////// 
	assert(bbl >= 0 & bbh < nBB_);
	/////////////////////////////////

	vBB_.clear();
	
	//special case: same bitblock
	if(bbh == bbl){	
		vBB_.emplace_back( pBlock_t( bbl, bblock::MASK_1(firstBit - WMUL(bbl), lastBit - WMUL(bbh))));
	}
	else {

		//first and last blocks
		vBB_.emplace_back(pBlock_t(bbl, bblock::MASK_1_HIGH(firstBit - WMUL(bbl))));
		vBB_.emplace_back(pBlock_t(bbh, bblock::MASK_1_LOW(lastBit - WMUL(bbh))));

		//in-between blocks
		for (auto block = bbl + 1; block < bbh; ++block) {
			vBB_.emplace_back(pBlock_t(block, ONE));
		}
	}

	return *this;
}

BitSetSp& BitSetSp::set_bit (const BitSetSp& rhs){

	///////////////////////////////////////////////
	assert(rhs.capacity() == this->capacity());
	//////////////////////////////////////////////

	//special case
	if (rhs.is_empty()) {
		return *this;
	}

	///////////////////////////////////	
	auto rIt = rhs.vBB_.cbegin();				//iterator to rhs	
	const auto sizeL = vBB_.size();			//stores the original size of *this since it will be enlarged
	auto posL = 0;								//position of bitblocks in  *this
	auto flag_sort = false;
	///////////////////////////////////
			
	//main loop
	while( posL != sizeL && rIt != rhs.vBB_.end()) {
		
		if (vBB_[posL].idx_ < rIt->idx_) {
			++posL;
		}
		else if (vBB_[posL].idx_ > rIt->idx_) {

			vBB_.push_back(*rIt);					
			flag_sort = true;
			++rIt;
		}
		else {
			/////////////////////////
			vBB_[posL].bb_ |= rIt->bb_;
			////////////////////////

			++posL,
			++rIt;
		}
	}
	//end while


	//I. exit condition
	if (posL == sizeL)			
	{
		//append rhs blocks at the end
		vBB_.insert(vBB_.end(), rIt, rhs.vBB_.end());
	}

	//always keep array sorted
	if (flag_sort) {
		std::sort(vBB_.begin(), vBB_.end(), pBlock_less());
	}

	return *this;		
}

BitSetSp&  BitSetSp::set_block (int firstBlock, int lastBlock, const BitSetSp& rhs){
			
	//special case - the full range
	if (lastBlock == -1) {
		return set_block(firstBlock, rhs);
	}

	//////////////////////////////////////////////////////////////////////////////////
	assert(firstBlock >= 0 && firstBlock <= lastBlock && lastBlock < rhs.capacity());
	//////////////////////////////////////////////////////////////////////////////////

	////////////////////////
	// Initialization

	//this		
	auto posL = BBObject::noBit;										//position of firstBlock in THIS or closest block
	auto itL = find_block(firstBlock, posL);							//iterator for THIS O(log n)
	(itL != vBB_.end()) ? posL = itL - vBB_.begin() : 1;				//sets posL to itL
	
	//stores the original size of THIS since it can be enlarged
	const auto sizeL = vBB_.size();									

	///////////
	//rhs
	auto pRlow = rhs.find_block_ext(firstBlock);						//O(log n)

	//flag to sort the collection
	bool flag_sort = false;

	////////////////////////////////////////////

	//I) special case - bitset rhs has no information in the range
	if( pRlow.second == rhs.vBB_.end()){ 
		return *this;
	}

	//II) special case  - this bitset has no information to mask in the range
	if (itL == vBB_.end())
	{
		//append rhs at the end
		for (; pRlow.second != rhs.vBB_.end() && pRlow.second->idx_ <= lastBlock; ++pRlow.second) {
			vBB_.emplace_back(*pRlow.second);
		}
		return *this;
	}

	//III) special case first block = last block and the block exists in rhs
	if (firstBlock == lastBlock && pRlow.second->idx_ == firstBlock) {
		if (itL->idx_ != firstBlock) {

			//block does not exist in THIS, add and sort
			vBB_.emplace_back(*pRlow.second);
			///////////////////////////////////

			//necessary because itL must have an index greatert than firstBlock
			sort();			
		}
		else {

			//block exists in THIS, overwrite
			vBB_[posL].bb_ |= pRlow.second->bb_;
		}			
		return *this;
	}	

	///////////////////
	//main loop
	while (posL < sizeL				&&
			vBB_[posL].idx_ <= lastBlock &&
			pRlow.second != rhs.vBB_.end()  &&
			pRlow.second->idx_ <= lastBlock		)
	{
	
		//update before either of the bitstrings has reached its end
		if(vBB_[posL].idx_ < pRlow.second->idx_)
		{
			++posL;
		}
		else if (vBB_[posL].idx_ > pRlow.second->idx_)
		{
			///////////////////////////////////
			vBB_.emplace_back(*pRlow.second);	
			//////////////////////////////////

			++pRlow.second;

			//sorting is necessary since the block added has less index
			flag_sort = true;
		}
		else {			//must have same indexes		

			//////////////////////////////////////
			vBB_[posL].bb_ |= pRlow.second->bb_;
			//////////////////////////////////////

			++posL;
			++pRlow.second;
		}	

	}//end while
		
	//exit conditions   
	if (posL == sizeL || vBB_[posL].idx_ > lastBlock)
	{
		//add remaining blocks in rhs to *this
		for (; pRlow.second != rhs.vBB_.end() && pRlow.second->idx_ <= lastBlock; ++pRlow.second) {
			vBB_.emplace_back(*pRlow.second);
		}	

		if (vBB_[posL].idx_ > lastBlock) {
			flag_sort = true;
		}	
		
	}

	//sort if required
	if (flag_sort) {
		std::sort(vBB_.begin(), vBB_.end(), pBlock_less());

	}
	
	return *this;		
}

int BitSetSp::clear_bit (int low, int high){
	
	int bbl = EMPTY_ELEM, bbh = EMPTY_ELEM; 
	pair<bool, BitSetSp::vPB_it> pl;
	pair<bool, BitSetSp::vPB_it> ph;

////////////////////////
//special cases
	if(high == EMPTY_ELEM && low == EMPTY_ELEM){
		vBB_.clear();
		return 0;
	}

	if(high == EMPTY_ELEM){
		bbl=WDIV(low);
		pl = find_block_ext(bbl);
		if(pl.second==vBB_.end()) return 0;

		if(pl.first){	//lower block exists
			pl.second->bb_&=Tables::mask_low[low-WMUL(bbl)];
			++pl.second;
		}

		//remaining
		vBB_.erase(pl.second, vBB_.end());
		return 0;
	}else if(low == EMPTY_ELEM){
		bbh=WDIV(high); 
		ph=find_block_ext(bbh);
		if(ph.first){	//upper block exists
			ph.second->bb_&=Tables::mask_high[high-WMUL(bbh)];
		}

		//remaining
		vBB_.erase(vBB_.begin(), ph.second);
		return 0;
	}

////////////////
// general cases

	//check consistency
	if(low>high){
		cerr<<"Error in set bit in range"<<endl;
		return -1;
	}
		

	bbl=WDIV(low);
	bbh=WDIV(high); 
	pl=find_block_ext(bbl);
	ph=find_block_ext(bbh);	


	//tratamiento
	if(pl.second!=vBB_.end()){
		//updates lower bitblock
		if(pl.first){	//lower block exists
			if(bbh==bbl){		//case update in the same bitblock
				BITBOARD bb_low=pl.second->bb_ & Tables::mask_high[high-WMUL(bbh)];
				BITBOARD bb_high=pl.second->bb_ &Tables::mask_low[low-WMUL(bbl)];
				pl.second->bb_=bb_low | bb_high;
				return 0;
			}

			//update lower block
			pl.second->bb_&=Tables::mask_low[low-WMUL(bbl)];
			++pl.second;
		}

		//updates upper bitblock
		if(ph.first){	//lower block exists
			if(bbh==bbl){		//case update in the same bitblock
				BITBOARD bb_low=pl.second->bb_ & Tables::mask_high[high-WMUL(bbh)];
				BITBOARD bb_high=pl.second->bb_ &Tables::mask_low[low-WMUL(bbl)];
				pl.second->bb_=bb_low | bb_high;
				return 0;
			}

			//update lower block
			ph.second->bb_&=Tables::mask_high[high-WMUL(bbh)];
		}

		//remaining
		vBB_.erase(pl.second, ph.second);
	}

	
return 0;
}

BitSetSp&  BitSetSp::erase_bit (const BitSetSp& rhs ){

	auto itL = vBB_.begin();		//iterator to *this
	auto itR = rhs.vBB_.cbegin();	//iterator to rhs
	
	
	while (itL != vBB_.end() && itR != rhs.vBB_.end()) {
				
		//update before either of the bitstrings has reached its end
		if (itL->idx_ < itR->idx_)
		{
			++itL;
		}
		else if (itL->idx_ > itR->idx_)
		{
			++itR;
		}
		else{

			//equal indexes - erase bits
			itL->bb_ &= ~itR->bb_;
			
			++itL;
			++itR;
		}
	}

	return *this;
}

BitSetSp& BitSetSp::operator &= (const BitSetSp& rhs){
	
	auto itL = vBB_.begin();		//iterator to *this	
	auto itR = rhs.vBB_.cbegin();	//iterator to rhs	

	while( itL != vBB_.end() && itR != rhs.vBB_.end()  ){
	
		//update before either of the bitstrings has reached its end
		if(itL->idx_ < itR->idx_)
		{
			///////////////////////
			itL->bb_ = 0;
			///////////////////////

			++itL;

		}else if (itL->idx_ > itR->idx_ )
		{
			++itR;
		}else
		{
			//equal indexes

			//////////////////////////////////////
			itL->bb_ &= itR->bb_;
			/////////////////////////////////////
			
			++itL;
			++itR;
		}
			

	}//end while

	//if loop terminates because rhs has reached its end then
	//delete remaining blocks in THIS 
	if (itR == rhs.vBB_.end()) {
		
		for (; itL != vBB_.end(); ++itL) {

			////////////////////////////
			itL->bb_ = ZERO;
			///////////////////////////
		}
	}


	return *this;
}

BitSetSp& BitSetSp::operator |= (const BitSetSp& rhs){

	auto posL = 0;					//position *this
	auto itR = rhs.cbegin();		//iterator to rhs
	auto sizeL = vBB_.size();		//stores the original size of *this since it will be modified
	bool flag_sort = false;			//flag to sort the collection
		
	//OR before all the blocks of one of the bitsets have been examined
	while (posL < sizeL && itR != rhs.vBB_.end()) {
			
		if(vBB_[posL].idx_ < itR->idx_)
		{
			posL++;
		}else if(vBB_[posL].idx_ > itR->idx_ )
		{
			//////////////////////////////////
			vBB_.emplace_back(*itR);
			///////////////////////////////////
			flag_sort = true;
			itR++;
		}else{

			//equal indexes

			///////////////////////////////////////////
			vBB_[posL].bb_ |= itR->bb_;
			///////////////////////////////////////////

			posL++;
			itR++;
		}
	}

	//rhs unfinished with index below the last block of *this
	if (posL == sizeL) {

		for (; itR != rhs.vBB_.end(); ++itR) {

			//////////////////////////////////
			vBB_.emplace_back(*itR);
			///////////////////////////////////
		}
	}

	//keep the collection sorted
	if (flag_sort) {
		sort();
	}	
	
	return *this;
}

BitSetSp& BitSetSp::operator ^= (const BitSetSp& rhs) {

	auto posL = 0;					//position *this	
	auto itR = rhs.cbegin();		//iterator to rhs
	auto sizeL = vBB_.size();		//stores the original size of *this since it will be modified
	bool flag_sort = false;			//flag to sort the collection
	
	//XOR before all the blocks of one of the bitsets have been examined
	while ((posL < sizeL) && (itR !=  rhs.vBB_.end())) {

		if (vBB_[posL].idx_ < itR->idx_)
		{
			posL++;
		}
		else if (vBB_[posL].idx_ > itR->idx_)
		{
			//////////////////////////////////
			vBB_.emplace_back(*itR);
			///////////////////////////////////
			flag_sort = true;
			itR++;
		}
		else {

			//equal indexes

			/////////////////////////////
			vBB_[posL].bb_ ^= itR->bb_;
			/////////////////////////////

			posL++;
			itR++;
		}
	}

	//rhs unfinished with index below the last block of *this
	if (posL == sizeL) {

		for (; itR != rhs.vBB_.end(); ++itR) {

			//////////////////////////////////
			vBB_.emplace_back(*itR);
			///////////////////////////////////
		}
	}

	//keep the collection sorted
	if (flag_sort) {
		sort();
	}	

	return *this;
}


BITBOARD BitSetSp::find_block (int blockID) const{

	////////////////////////////////////////////////////////////////////////////////////////////
	auto it = lower_bound(vBB_.cbegin(), vBB_.cend(), pBlock_t(blockID), pBlock_less());
	////////////////////////////////////////////////////////////////////////////////////////////

	if(it != vBB_.end() && it->idx_ == blockID){
		return it->bb_;
	}
	else {
		return BBObject::noBit;
	}
}


std::pair<bool, int>
BitSetSp::find_block_pos (int blockID) const{

	std::pair<bool, int> res(false, EMPTY_ELEM);

	////////////////////////////////////////////////////////////////////////////////////////////
	auto it = lower_bound(vBB_.cbegin(), vBB_.cend(), pBlock_t(blockID), pBlock_less());
	////////////////////////////////////////////////////////////////////////////////////////////

	if(it != vBB_.end()){
		res.second = it - vBB_.begin();
		if(it->idx_ == blockID){
			res.first = true;
		}
	}

	return res;
}

template<bool LB_policy>
std::pair<bool, BitSetSp::vPB_it>
BitSetSp::find_block_ext (int blockID) 
{
	pair<bool, BitSetSp::vPB_it> res;

	if (LB_policy) {
		////////////////////////////////////////////////////////////////////////////////////////////
		res.second = lower_bound(vBB_.begin(), vBB_.end(), pBlock_t(blockID), pBlock_less());
		////////////////////////////////////////////////////////////////////////////////////////////

		res.first = (res.second != vBB_.end()) && (res.second->idx_ == blockID);
	}
	else {
		////////////////////////////////////////////////////////////////////////////////////////////
		res.second = upper_bound(vBB_.begin(), vBB_.end(), pBlock_t(blockID), pBlock_less());
		////////////////////////////////////////////////////////////////////////////////////////////

		res.first = false;			//the same block cannot be found with upper_bound, only the closest with greater index
	}
	
	return res;
}

template<bool LB_policy>
pair<bool, BitSetSp::vPB_cit> 
BitSetSp::find_block_ext (int blockID) const
{
	pair<bool, BitSetSp::vPB_cit>res;

	if (LB_policy) {
		////////////////////////////////////////////////////////////////////////////////////////////
		res.second = lower_bound(vBB_.cbegin(), vBB_.cend(), pBlock_t(blockID), pBlock_less());
		////////////////////////////////////////////////////////////////////////////////////////////

		res.first = (res.second != vBB_.end()) && (res.second->idx_ == blockID);
	}
	else {
		////////////////////////////////////////////////////////////////////////////////////////////
		res.second = upper_bound(vBB_.cbegin(), vBB_.cend(), pBlock_t(blockID), pBlock_less());		
		////////////////////////////////////////////////////////////////////////////////////////////

		res.first = false;			//the same block cannot be found with upper_bound, only the closest with greater index
	}
		
	return res;
}

///////////////////////////////
//
// Bit updates
//
//
////////////////////////////////



int BitSetSp::next_bit(int nBit/* 0 based*/)  const {
////////////////////////////
// RETURNS next bit from nBit in the bitstring (to be used in a bitscan loop)
//			 if nBit is FIRST_BITSCAN returns lsb
//
// NOTES: 
// 1. A  preliminay implementation. It becomes clear the necessity of caching the index of the vector at each iteration (*)
	
	
	if(nBit==EMPTY_ELEM){
		return lsb();
	}
		
	//look in the remaining bitblocks (needs caching last i visited)
	int idx = WDIV(nBit);
	for(int i=0; i<vBB_.size(); i++){
		if(vBB_[i].idx_< idx) continue;					//(*)
		if(vBB_[i].idx_== idx){
			int npos=bblock::lsb64_de_Bruijn(Tables::mask_high[WMOD(nBit) /* WORD_SIZE*idx */] & vBB_[i].bb_);
			if(npos!=EMPTY_ELEM) return (WMUL(idx) + npos);
			continue;
		}
		//new bitblock
		if( vBB_[i].bb_){
			return bblock::lsb64_de_Bruijn(vBB_[i].bb_) + WMUL(vBB_[i].idx_);
		}
	}
	
return -1;	
}




int BitSetSp::prev_bit(int nBit/* 0 bsed*/) const{
////////////////////////////
// Gets the previous bit to nBit. 
// If nBits is FIRST_BITSCAN is a MSB
//
// NOTES: 
// 1. A  preliminary implementation. It becomes clear the necessity of caching the index of the vector at each iteration (*)
		
	if (nBit == EMPTY_ELEM) {
		return msbn64_lup();
	}
		
	//look in the remaining bitblocks
	int idx = WDIV(nBit);
	for(int i=vBB_.size()-1; i>=0; i--){
		if(vBB_[i].idx_> idx) continue;							//(*)
		if(vBB_[i].idx_== idx){
			int npos=bblock::msb64_lup(Tables::mask_low[WMOD(nBit) /* WORD_SIZE*idx */] & vBB_[i].bb_);
			if(npos!=EMPTY_ELEM) return (WMUL(idx) + npos);
			continue;
		}
		if(vBB_[i].bb_ ){
			return bblock::msb64_lup(vBB_[i].bb_) + WMUL(vBB_[i].idx_);
		}
	}
	
return -1;	
}


//////////////
//
// Operators
//
///////////////

BitSetSp&  OR	(const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res){
///////////////////////////
// OR between sparse sets out of place (does not requires sorting)
// 	
// REMARKS: does not check for null information

	int i1=0, i2=0;
	res.erase_bit();		//experimental (and simplest solution)
	while(true){
		//exit condition I
		if(i1==lhs.vBB_.size()){
			res.vBB_.insert(res.vBB_.end(), rhs.vBB_.begin()+i2, rhs.vBB_.end());
			return res;
		}else if(i2==rhs.vBB_.size()){
			res.vBB_.insert(res.vBB_.end(), lhs.vBB_.begin()+i1, lhs.vBB_.end());
			return res;
		}
		
		//update before either of the bitstrings has reached its end
		if(lhs.vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			BitSetSp::pBlock_t e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ );
			res.vBB_.push_back(e);
			++i1;
		}else if(rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_){
			BitSetSp::pBlock_t e(rhs.vBB_[i2].idx_, rhs.vBB_[i2].bb_ );
			res.vBB_.push_back(e);
			++i2;
		}else{
			BitSetSp::pBlock_t e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ | rhs.vBB_[i2].bb_);
			res.vBB_.push_back(e);
			++i1, ++i2; 
		}

	/*	if(lhs.vBB_[i1].idx_==rhs.vBB_[i2].idx_){
			BitSetSp::pBlock_t e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ | rhs.vBB_[i2].bb_);
			res.vBB_.push_back(e);
			++i1, ++i2; 
		}else if(lhs.vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			BitSetSp::pBlock_t e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ );
			res.vBB_.push_back(e);
			++i1;
		}else if(rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_){
			BitSetSp::pBlock_t e(rhs.vBB_[i2].idx_, rhs.vBB_[i2].bb_ );
			res.vBB_.push_back(e);
			++i2;
		}*/
	}
return res;
}


////////////////////////////
////
//// I/O FILES
////
////////////////////////////

ostream& BitSetSp::print (std::ostream& o, bool show_pc, bool endl ) const  {
	
	/////////////
	o << '[';
	/////////////
		
	int nBit = BBObject::noBit;
	while( (nBit = next_bit(nBit)) != BBObject::noBit) {
		o << nBit << ' ';
	}

	if(show_pc){
		int pc = popcn64();
		if (pc) {
			o << '(' << popcn64() << ')';
		}
	}
	
	/////////////
	o << ']';
	//////////////

	//add eol if requires
	if (endl) {
		o << std::endl;
	}

	return o;
}

string BitSetSp::to_string (){

	ostringstream sstr;

	/////////////
	sstr << '[';
	/////////////
		
	int nBit = BBObject::noBit;
	while( (nBit = next_bit(nBit)) != BBObject::noBit) {
		sstr << nBit << ' ';
	}

	sstr << '(' << size() << ')';

	///////////////
	sstr << ']';
	///////////////

	return sstr.str();
}

void BitSetSp::to_vector (std::vector<int>& lb)const{

	lb.clear();

	int nBit = BBObject::noBit;
	while((nBit = next_bit(nBit)) != BBObject::noBit ){
		lb.push_back(nBit);
	}
}

BitSetSp::operator vint() const
{
	vint lb;
	to_vector(lb);
	return lb;
}

