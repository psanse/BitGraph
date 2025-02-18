 /**
  * @file bbset_sparse.cpp
  * @brief implementations for the sparse BitSetSp class wrapper for sparse bitstrings (header bbset_sparse.h)
  * @author pss
  * @details created ?, @last_update 17/02/2025
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
			assert(set_bit(bit) != -1);

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
			assert(set_bit(bit) != -1);

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

void BitSetSp::sort (){
	std::sort(vBB_.begin(), vBB_.end(), pBlock_less());
}	


int	 BitSetSp::set_bit	(int low, int high){
///////////////////
// sets bits to one in the corresponding CLOSED range
//
// REMARKS: 
// 1.only one binary search is performed for the lower block
// 2.does not check maximum possible size
//
//*** OPTIMIZATION: restrict sorting when required to [low,high] interval
			
	int	bbh=WDIV(high); 
	int bbl=WDIV(low); 
	bool req_sorting=false;
	//vBB_.reserve(vBB_.size()+high-low+1);					//maximum possible size, to push_back in place without allocation
	vPB vapp;

	//checks consistency (ASSERT)
	if(bbh<bbl || bbl<0 || low>high || low<0){
		cerr<<"Error in set bit in range"<<endl;
		return -1;
	}

	//A) bbh==bbl
	pair<bool,vPB_it> p=find_block_ext(bbl);
	if(bbh==bbl){
		if(p.first){
			BITBOARD bb_high=p.second->bb_| ~Tables::mask_high[high-WMUL(bbh)];
			BITBOARD bb_low=p.second->bb_| ~Tables::mask_low[low-WMUL(bbl)];
			p.second->bb_=bb_low & bb_high;
			return 0;		//no need for sorting
		}else{
			BITBOARD bb_high= ~Tables::mask_high[high-WMUL(bbh)];
			vBB_.push_back(pBlock_t(bbl,bb_high&~ Tables::mask_low[low-WMUL(bbl)]));
			return 0;
		}
	}

	//B) bbl outside range 
	if(p.second==vBB_.end()){
		//append blocks at the end
		vBB_.push_back(pBlock_t(bbl,~Tables::mask_low[low-WMUL(bbl)]));
		for(int i=low+1; i<bbh; ++i){
				vBB_.push_back(pBlock_t(i,ONE));
		}
		vBB_.push_back(pBlock_t(bbh,~Tables::mask_high[high-WMUL(bbh)]));
		return 0;
	}
	

	//C) bbl  
	int block=bbl;
	if(p.first){	//block exists	
		p.second->bb_|=~Tables::mask_low[low-WMUL(bbl)];
		++p.second;
	}else{ //block does not exist:marked for append
		vapp.push_back(pBlock_t(bbl,~Tables::mask_low[low-WMUL(bbl)]));
	}
	++block;
	
	//D) Remaining blocks
	while(true){
		//exit condition I
		if(p.second==vBB_.end()){
			//append blocks at the end
			for(int i=block; i<bbh; ++i){
				vBB_.push_back(pBlock_t(i,ONE));
			}
			vBB_.push_back(pBlock_t(bbh,~Tables::mask_high[high-WMUL(bbh)]));
			req_sorting=true;
			break;
		}

		//exit condition II
		if(block==bbh){
			if(p.second->idx_==block){ //block exists: trim and overwrite
				p.second->bb_|=~Tables::mask_high[high-WMUL(bbh)];
				break;
			}else{ //block doesn't exist, trim and append
				vBB_.push_back(pBlock_t(bbh,~Tables::mask_high[high-WMUL(bbh)]));
				req_sorting=true;
				break;
			}
		}
						

		//update before either of the bitstrings has reached its end
		if(p.second->idx_==block){
			p.second->bb_=ONE;
			++p.second; ++block; 
		}else if(block<p.second->idx_){				//vBB_[pos].idx_<block cannot occur
			vapp.push_back(pBlock_t(block,ONE));			//not added in place to avoid loosing indexes
		//	vBB_.push_back(pBlock_t(block,ONE));
			req_sorting=true;
			++block;
		}
	}

	//sorts if necessary
	vBB_.insert(vBB_.end(), vapp.begin(), vapp.end());
	if(req_sorting) 
				sort();				

return 0;
}

int BitSetSp::init_bit (int low, int high){
/////////////////////////
// sets bits in the closed range and clears the rest
	
	//*** ASSERT (MAX SIZE)  

	int	bbh=WDIV(high); 
	int bbl=WDIV(low);
	vBB_.clear();
	
	//same bitblock
	if(bbh==bbl){
		BITBOARD bb= ~Tables::mask_low[low-WMUL(bbl)];
		vBB_.push_back(pBlock_t(bbl, bb & ~Tables::mask_high[high-WMUL(bbh)]));
		return 0;
	}

	//first
	vBB_.push_back(pBlock_t(bbl, ~Tables::mask_low[low-WMUL(bbl)]));

	//middle
	for(int block=bbl+1; block<bbh; ++block){
		vBB_.push_back(pBlock_t(block,ONE));
	}

	//last
	vBB_.push_back(pBlock_t(bbh, ~Tables::mask_high[high-WMUL(bbh)]));
	
return 0;
}



BitSetSp& BitSetSp::set_bit (const BitSetSp& rhs){
/////////////////////////////////
// sets 1-bits in rhs (equivalent to an |=)
//
// REMARKS: experimental, currently only defined for bit strings of same maximum size

	if(rhs.is_empty()) 	return *this;
	//vBB_.reserve(rhs.vBB_.size()+vBB_.size());		//maximum possible size, to push_back in place without allocation
	vPB vapp;
	vPB_cit i2=rhs.vBB_.cbegin();
	vPB_it i1=vBB_.begin();
	
	
	bool req_sorting;
	while(true){
		//exit condition I
		if( i2==rhs.vBB_.end() ){		
					break;		
		}

		//exit condition II
		if(i1==vBB_.end()){
			//append blocks at the end
			vBB_.insert(vBB_.end(),i2, rhs.vBB_.end());
			req_sorting=true;
			break;
		}
		
		//update before either of the bitstrings has reached its end
		if(i1->idx_==i2->idx_){
			i1->bb_|=i2->bb_;
			++i1, ++i2; 
		}else if(i1->idx_<i2->idx_){
			++i1;
		}else if(i2->idx_<i1->idx_){
			vapp.push_back(*i2);
			//vBB_.push_back(*i2);
			req_sorting=true;
			++i2;
		}
	}

	//always keep array sorted
	vBB_.insert(vBB_.end(), vapp.begin(), vapp.end());
	if(req_sorting)
				sort();

return *this;		
}

BitSetSp&  BitSetSp::set_block (int first_block, const BitSetSp& rhs){
/////////////////////////////////
//
// REMARKS: experimental, currently only defined for bit strings of same size

	//vBB_.reserve(rhs.vBB_.size()+vBB_.size());						//maximum possible size, to push_back in place without allocation
	vPB vapp;
	pair<bool, BitSetSp::vPB_it> p1=find_block_ext(first_block);
	pair<bool, BitSetSp::vPB_cit> p2=rhs.find_block_ext(first_block);
	bool req_sorting=false;
		

	if(p2.second==rhs.vBB_.end()){		//check in this order (captures rhs empty on init)
		return *this;
	}

	if(p1.second==vBB_.end()){
		//append rhs at the end
		vBB_.insert(vBB_.end(),p2.second, rhs.vBB_.end());	
		req_sorting=true;
		sort();
		return *this;
	}
			
	while(true){
		if(p2.second==rhs.vBB_.end()){	//exit condition I
			break;
		}else if(p1.second==vBB_.end()){	 //exit condition II  		
			vBB_.insert(vBB_.end(),p2.second, rhs.vBB_.end());
			req_sorting=true;
			break;			
		}

		//update before either of the bitstrings has reached its end
		if(p1.second->idx_==p2.second->idx_){
			p1.second->bb_|=p2.second->bb_;
			++p1.second, ++p2.second; 
		}else if(p1.second->idx_<p2.second->idx_){
			++p1.second;
		}else if(p2.second->idx_<p1.second->idx_){
			vapp.push_back(*p2.second);	
			//vBB_.push_back(*p2.second);		
			req_sorting=true;
			++p2.second;
		}
	}

	//always keep array sorted
	vBB_.insert(vBB_.end(), vapp.begin(), vapp.end());
	if(req_sorting)
				sort();
	
return *this;		
}

BitSetSp&  BitSetSp::set_block (int first_block, int last_block, const BitSetSp& rhs){
/////////////////////////////////
//
// REMARKS: experimental, currently only defined for bit strings of same size 
	
	//vBB_.reserve(vBB_.size()+ last_block-first_block+1);						//maximum possible size, to push_back in place without allocation
	vPB vapp;
	pair<bool, BitSetSp::vPB_it> p1i=find_block_ext(first_block);
	pair<bool, BitSetSp::vPB_cit> p2i=rhs.find_block_ext(first_block);
	pair<bool, BitSetSp::vPB_cit> p2f=rhs.find_block_ext(last_block);
	bool req_sorting=false;
		
	if(p2i.second==rhs.vBB_.end()){ //check in this order (captures rhs empty on init)
		return *this;
	}

	BitSetSp::vPB_cit p2it_end=(p2f.first)? p2f.second+1 : p2f.second;			//iterator to the last block +1 in the rhs
	if(p1i.second==vBB_.end()){
		//append rhs at the end
		vBB_.insert(vBB_.end(),p2i.second, p2it_end);	
		sort();
		return *this;
	}

	do{
		//update before either of the bitstrings has reached its end
		if(p1i.second->idx_==p2i.second->idx_){
			p1i.second->bb_|=p2i.second->bb_;
			++p1i.second, ++p2i.second; 
		}else if(p1i.second->idx_<p2i.second->idx_){
			++p1i.second;
		}else if(p2i.second->idx_<p1i.second->idx_){
			vapp.push_back(*p2i.second);
			//vBB_.push_back(*p2i.second);		//*** iterators!!!
			req_sorting=true;
			++p2i.second;
		}
						
		//exit condition   
		if( p1i.second==vBB_.end() || p1i.second->idx_>last_block ){			
			vBB_.insert(vBB_.end(),p2i.second, (p2f.first)? p2f.second+1 : p2f.second);
			req_sorting=true;
			break;			
		}else if(p2i.second == p2it_end ){			//exit condition II
			break;
		}

	}while(true);

	//always keep array sorted
	vBB_.insert(vBB_.end(), vapp.begin(), vapp.end());
	if(req_sorting)
				sort();
	
return *this;		
}



int BitSetSp::clear_bit (int low, int high){
///////////////////
// clears bits in the corresponding CLOSED range (effectively deletes bitblocks)
//
// PARTICULAR CASES: low = EMPTY_ELEM, high= EMPTY_ELEM mark bitstring bounds
//
// EXPERIMENTAL
	
	
	int bbl=EMPTY_ELEM, bbh=EMPTY_ELEM; 
	pair<bool, BitSetSp::vPB_it> pl;
	pair<bool, BitSetSp::vPB_it> ph;

////////////////////////
//special cases
	if(high==EMPTY_ELEM && low==EMPTY_ELEM){
		vBB_.clear();
		return 0;
	}

	if(high==EMPTY_ELEM){
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
	}else if(low==EMPTY_ELEM){
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
////////////////////
// removes 1-bits from current object (equialent to set_difference)
	int i1=0, i2=0;
	while(true){
		//exit condition I
		if(i1==vBB_.size() || i2==rhs.vBB_.size() ){		//size should be the same
					return *this;
		}

		//update before either of the bitstrings has reached its end
		if(vBB_[i1].idx_==rhs.vBB_[i2].idx_){
			vBB_[i1].bb_&=~rhs.vBB_[i2].bb_;
			i1++, i2++; 
		}else if(vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			i1++;
		}else if(rhs.vBB_[i2].idx_<vBB_[i1].idx_){
			i2++;
		}
	}

return *this;
}

BitSetSp& BitSetSp::operator &= (const BitSetSp& rhs){
///////////////////
// AND mask in place

	int i1=0, i2=0;
	while(true){
		//exit conditions 
		if(i1==vBB_.size() ){		//size should be the same
					return *this;
		}else if(i2==rhs.vBB_.size()){  //fill with zeros from last block in rhs onwards
			for(; i1<vBB_.size(); i1++)
				vBB_[i1].bb_=ZERO;
			return *this;
		}

		//update before either of the bitstrings has reached its end
		if(vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			vBB_[i1].bb_=0;
			i1++;
		}else if (rhs.vBB_[i2].idx_<vBB_[i1].idx_){
			i2++;
		}else{
			vBB_[i1].bb_ &= rhs.vBB_[i2].bb_;
			i1++, i2++; 
		}

		/*if(vBB_[i1].idx_==rhs.vBB_[i2].idx_){
			vBB_[i1].bb_ &= rhs.vBB_[i2].bb_;
			i1++, i2++; 
		}else if(vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			vBB_[i1].bb_=0;
			i1++;
		}else if(rhs.vBB_[i2].idx_<vBB_[i1].idx_){
			i2++;
		}*/
	}

return *this;
}

BitSetSp& BitSetSp::operator |= (const BitSetSp& rhs){
///////////////////
// OR mask in place
// date:10/02/2015
// last_update: 10/02/2015

	int i1=0, i2=0;
	while(true){
		//exit conditions 
		if(i1==vBB_.size() || i2==rhs.vBB_.size() ){				//size should be the same
					return *this;
		}

		//update before either of the bitstrings has reached its end
		if(vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			i1++;
		}else if(rhs.vBB_[i2].idx_<vBB_[i1].idx_){
			i2++;
		}else{
			vBB_[i1].bb_ |= rhs.vBB_[i2].bb_;
			i1++, i2++; 
		}
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

int BitSetSp::lsbn64() const{
/////////////////
// different implementations of lsbn depending on configuration

#ifdef DE_BRUIJN
	for(int i=0; i<vBB_.size(); i++){
		if(vBB_[i].bb_)
#ifdef ISOLANI_LSB
			return(Tables::indexDeBruijn64_ISOL[((vBB_[i].bb_ & -vBB_[i].bb_) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(vBB_[i].idx_));	
#else
			return(Tables::indexDeBruijn64_SEP[((vBB_[i].bb_^ (vBB_[i].bb_-1)) * bblock::DEBRUIJN_MN_64_SEP/*magic num*/) >> bblock::DEBRUIJN_MN_64_SHIFT]+ WMUL(vBB_[i].idx_));
#endif
	}
#elif LOOKUP
	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=0; i<m_nBB; i++){
		val.b=vBB_[i].bb_;
		if(val.b){
			if(val.c[0]) return (Tables::lsba[0][val.c[0]]+WMUL(vBB_[i].idx_));
			if(val.c[1]) return (Tables::lsba[1][val.c[1]]+WMUL(vBB_[i].idx_));
			if(val.c[2]) return (Tables::lsba[2][val.c[2]]+WMUL(vBB_[i].idx_));
			if(val.c[3]) return (Tables::lsba[3][val.c[3]]+WMUL(vBB_[i].idx_));
		}
	}

#endif
return EMPTY_ELEM;	
}

 int BitSetSp::msbn64() const{
///////////////////////
// Look up table implementation (best found so far)

	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(auto i = vBB_.size() - 1; i >= 0; --i){
		val.b = vBB_[i].bb_;
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]] + WMUL(vBB_[i].idx_));
			if(val.c[2]) return (Tables::msba[2][val.c[2]] + WMUL(vBB_[i].idx_));
			if(val.c[1]) return (Tables::msba[1][val.c[1]] + WMUL(vBB_[i].idx_));
			if(val.c[0]) return (Tables::msba[0][val.c[0]] + WMUL(vBB_[i].idx_));
		}
	}

return EMPTY_ELEM;		//should not reach here
}
 

int BitSetSp::next_bit(int nBit/* 0 based*/)  const {
////////////////////////////
// RETURNS next bit from nBit in the bitstring (to be used in a bitscan loop)
//			 if nBit is FIRST_BITSCAN returns lsb
//
// NOTES: 
// 1. A  preliminay implementation. It becomes clear the necessity of caching the index of the vector at each iteration (*)
	
	
	if(nBit==EMPTY_ELEM){
		return lsbn64();
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
		
	if(nBit==EMPTY_ELEM)
				return msbn64();
		
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
/////////////////////////
// shows bit string as [bit1 bit2 bit3 ... <(pc)>]  (if empty: [ ]) (<pc> optional)
	
	o<<"[";
		
	int nBit=EMPTY_ELEM;
	while(1){
		nBit=next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		o<<nBit<<" ";
	}

	if(show_pc){
		int pc=popcn64();
		if(pc)	o<<"("<<popcn64()<<")";
	}
	
	o<<"]";

	if (endl) {
		o << std::endl;
	}

	return o;
}

string BitSetSp::to_string (){
	ostringstream sstr;
	sstr<<"[";
	this->print();
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		sstr<<nBit<<" ";
	}

	sstr<<"("<<popcn64()<<")";
	sstr<<"]";

	return sstr.str();
}

void BitSetSp::to_vector (std::vector<int>& vl)const{
//////////////////////
// copies bit string to vector 
//
	vl.clear();

	int v=EMPTY_ELEM;
	while(true){
		v=next_bit(v);
		if(v==EMPTY_ELEM) break;

		vl.push_back(v);
	}
}

