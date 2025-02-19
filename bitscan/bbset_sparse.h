 /**
  * @file bbset_sparse.h
  * @brief header for sparse class equivalent to the BitSetSp class
  * @author pss
  * @details created ?, @last_update 15/02/2025
  *
  * TODO refactoring and testing 15/02/2025 - follow the interface of the refactored BitSet
  **/

#ifndef __BBSET_SPARSE_H__
#define __BBSET_SPARSE_H__

#include "bbobject.h"
#include "bitblock.h"
#include "utils/logger.h"
#include <vector>	
#include <algorithm>
#include <iterator>

//uncomment #undef NDEBUG in bbconfig.h to enable run-time assertions
#include <cassert>

using namespace std;

//initial reserve of bit blocks for any new sparse bitstring - CHECK efficiency (17/02/2025)
constexpr int DEFAULT_CAPACITY = 2;		

//aliases
using vint = std::vector<int>; 
 
/////////////////////////////////
//
// class BitSetSp
// (manages sparse bit strings, except efficient bitscanning)
//
///////////////////////////////////

class BitSetSp: public BBObject{
	
public:
	struct pBlock_t{
		int idx_;
		BITBOARD bb_;

		pBlock_t(int idx = BBObject::noBit, BITBOARD bb = 0) : idx_(idx), bb_(bb){}

		bool operator ==	(const pBlock_t& e)	const	{ return (idx_ == e.idx_ && bb_ == e.bb_); }
		bool operator !=	(const pBlock_t& e)	const	{ return (idx_ != e.idx_ || bb_ != e.bb_); }
		bool operator !		()					const	{ return !bb_ ; }
		bool is_empty		()					const	{ return !bb_ ; }
		void clear			()							{ bb_ = 0;}

		ostream& print		(ostream& o)		const	{ return o << idx_ << " : " << bb_<< '\n'; }
	};

	//aliases
	using vPB		= vector<pBlock_t>;
	using vPB_it	= vector<pBlock_t>::iterator;
	using vPB_cit	= vector<pBlock_t>::const_iterator;
		
	//functor for sorting - check if it is necessary, or throwing lambdas is enough
	struct pBlock_less {
		bool operator()(const pBlock_t& lhs, const pBlock_t& rhs) const {
			return lhs.idx_ < rhs.idx_;
		}
	};

/////////////////////
// static data members 
private:

	//Caches current bitblock position in the collection (not its idx_ in the bitstring)  - CHECK arquitecture (17/02/2025)
	static int nElem;	

/////////////////////
// independent operators / masks 
public:	

	friend bool	operator ==			(const BitSetSp&, const BitSetSp&);
	friend bool operator!=			(const BitSetSp& lhs, const BitSetSp& rhs);

    friend inline BitSetSp&  AND	(const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res);
	friend inline BitSetSp&  AND	(int first_block, const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res);
	friend inline BitSetSp&  AND	(int first_block, int last_block, const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res);
	friend BitSetSp&  OR			(const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res);
	friend BitSetSp&  ERASE			(const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res);			//removes rhs from lhs

/////////////////////
// constructors / destructors

	BitSetSp						():nBB_(0)		{}	

	/**
	* @brief Constructor of an EMPTY bitset given a population size nPop
	*		  The capacity of the bitset is set according to the population size
	* @param nBits : population size if is_popsize is true, otherwise the capacity of the bitset
	**/
explicit BitSetSp					(int nPop, bool is_popsize = true );

	/**
	 * @brief Constructor of an bitset given an initial vector lv of 1-bit elements
	 *		  and a population capacity determined by the population maximum size nPop
	 *		  The capacity of the bitset is set according to nPop
	 * @param nPop: population size
	 * @param lv : vector of integers representing 1-bits in the bitset
	 **/
	 BitSetSp						(int nPop, const vint& lv);

	//Allows copy and move semantics
	BitSetSp				(const BitSetSp& bbN)			= default;
	BitSetSp				(BitSetSp&&)		noexcept	= default;
	BitSetSp& operator =	(const BitSetSp&)				= default;
	BitSetSp& operator =	(BitSetSp&&)		noexcept	= default;

	virtual ~BitSetSp				() = default;

////////////
//reset / init (heap allocation)

	/**
	* @brief resets the sparse bitset to a new population size if is_popsize is true, 
	*	     otherwise the maximum number of bitblocks of the bitset
	* @param nPop: population size 
	* @param is_popsize: if true, the population size is set to nPop, otherwise the 
	*					  it is the maximum number of bitblocks of the bitset
	* @details: if exception is thrown, the program exits
	**/
	void reset						(int nPop, bool is_popsize = true);
	
	/**
	* @brief resets the sparse bitset to a new population size with lv 1-bits
	* @param nPop: population size
	* @param lv: vector of 1-bits to set
	* @details: if exception is thrown, the program exits
	**/
	void reset						(int nPop, const vint& lv);

	/**
	* @brief resets the sparse bitset to a new population size, old syntax
	*		 now favoured by reset(...).
	**/
	void init						(int nPop, bool is_popsize = true);
	
/////////////////////
//setters and getters (will not allocate memory)
	
	/**
	* @brief number of non-zero bitblocks in the bitstring
	* @details As opposed to the non-sparse case, it can be zero if there are no 1-bits
	*		   even though the maximum number of bitblocks determined in construction nBB_
	*		   can be anything.
	**/
	int number_of_blocks			()						const { return vBB_.size();}
			
	/**
	* @brief maximum number of bitblocks in the bitset
	* @details the capacity is determined by the population size (construction),
	*		   it is NOT the number_of_blocks(), which are the non-zero bitblocks in the bitset
	* 
	**/
	int capacity					()						const { return nBB_; }
	
	/**
	* @brief returns the bitblock at position blockID, not the id-th block of the
	*		 equivalent non-sparse bitset in the general case. To find the id-th block
	*		 use find_bitblock function.
	**/
	BITBOARD  block					(int blockID)			const	{return vBB_[blockID].bb_;}			
	BITBOARD& block					(int blockID)					{return vBB_[blockID].bb_;}
		
	pBlock_t  pBlock				(int blockID)			const	{ return vBB_[blockID]; }
	pBlock_t& pBlock				(int blockID)					{ return vBB_[blockID]; }

	const vPB& bitset				()						const	{ return vBB_; }
	vPB& bitset						()								{ return vBB_; }
		
	/**
	* @brief finds the bitblock of the block index 
	* @returns bitblock of the block index or BBOjbect::noBit if it does not exist, i.e., it is empty
	* @details O(log) complexity
	**/
	BITBOARD find_block				(int blockID)			const;	

	/**
	* @brief extended version of find_block. The template parameter LB_policy determines
	*		 the search policy: lower_bound (true) or upper_bound (false). If true, it searches
	*		 for the block with the same index as blockID, otherwise it searches for the closest
	*		 block with a greater index than blockID.
	* 
	*		By default this function uses lower_bound policy, since with upper_bound policy the block
	*		cannot be found even if it exists.		
	*		
	* @param blockID: input index of the block searched
	* @returns a pair with first:true if the block exists, and second: the iterator to the block (policy = true) or the
	*		  iterator closest to the block with a greater index than blockID (policy=false / policy=true the block does not exist).
	*		  The iterator can return END in the following two cases:
	*		  a) Policy = true - the block does not exist and all the blocks have index less than blockID
	*		  b) Policy = false - all the blocks have index lower than OR EQUAL to blockID
	**/
	template<bool LB_policy = true>
	std::pair<bool, vPB_it>  
	find_block_ext					(int blockID);
	
	/**
	* @brief extended version of find_block, returning a const iterator
	*		 (see non-const function)
	**/
	template<bool LB_policy = true>
	std::pair<bool, vPB_cit> 
	find_block_ext					(int blockID)			const;
		
	/**
	* @brief finds the position (index) of the bitblock with blockID
	* @param blockID: input index of the block searched
	* @returns a pair with first:true if the block exists, and second: the index of the block in the collection
	*		   In the case the block does not exist (first element of the pair is false),
	*		   the second element is the closest block index ABOVE the blockID or -1 if no such block exists
	* @details O(log) complexity
	**/
	pair<bool, int>	 find_block_pos(int blockID)			const;
											
	/**
	* @brief commodity iterators / const iterators for the bitset
	**/
	vPB_it  begin					()									{return vBB_.begin();}
	vPB_it  end						()									{return vBB_.end();}
	vPB_cit cbegin					()						const		{return vBB_.cbegin();}
	vPB_cit cend					()						const		{return vBB_.cend();}

//////////////////////////////
// Bitscanning

	//find least/most signinficant bit
virtual inline	int msbn64			(int& nElem)			const;				
virtual inline	int lsbn64			(int& nElem)			const; 	
virtual	int msbn64					()						const;		//lookup
virtual	int lsbn64					()						const; 		//de Bruijn	/ lookup

	//for scanning all bits
	
virtual inline	int next_bit		(int nBit)				;				//uses cached elem position for fast bitscanning
virtual inline	int prev_bit		(int nBit)				;				//uses cached elem position for fast bitscanning

private:
	int next_bit					(int nBit)				const;			//de Bruijn 
	int prev_bit					(int nbit)				const;			//lookup

public:	
/////////////////
// Popcount
		inline   int size			()						const			{ return popcn64(); }
virtual inline	 int popcn64		()						const;			//lookup 
virtual inline	 int popcn64		(int nBit)				const;			

/////////////////////
//Setting (ordered insertion) / Erasing bits  

	/**
	* @brief resets the bitset to all 0-bits ans sets bit to 1
	**/
	void  reset_bit					(int bit);	
	
	/**
	* @brief resets the bitset to all 0-bits and then sets bit to 1 in the
	*		 range [firstBit, lastBit]
	* TODO- refactor
	**/
	int   reset_bit					(int firstBit, int lastBit);	

	/**
	* @brief resets the bitset to all 0-bits and then copies bitset in the
	*		 range [0, lastBit]
	* TODO- refactor
	**/
	int   reset_bit					(int lastBit, const BitSetSp& bitset);	
	
	/**
	* @brief resets the bitset to all 0-bits and then copies bitset in the
	*		 range [firstBit, lastBit]
	* TODO- refactor
	**/
	int   reset_bit					(int firstBit, int lastBit, const BitSetSp& bitset);
		
	/**
	* @brief Sets bit in the sparse bitset
	* @param nbit: position of the bit to set
	* @returns 0 if the bit was set, -1 if error
	* @details emplaces pBlock in the bitstring or changes an existing bitblock
	* @detials uses lower_bound for insertion (log overhead)
	* @details uses internal assert macro for error checking
	**/
inline	BitSetSp& set_bit			(int bit);

	/**
	* @brief sets bits in the closed range [firstBit, lastBit]
	* @params firstBit, lastBit: range
	* @returns 0 if the bits were set, -1 if error
	* @details only one binary search is performed for the lower block
	**/
BitSetSp&	 set_bit				(int firstBit, int lastBit);

	/**
	* @brief Adds the bits from the bitstring bb_add in the population
	*		 range of the bitstring (bitblocks are copied).
	*
	*		 I. Both bitsets should have the SAME capacity (number of blocks).
	*		
	* @details  Equivalent to OR operation / set union
	* @returns reference to the modified bitstring
	**/		
BitSetSp&    set_bit				(const BitSetSp& bb_add);											



BitSetSp&  set_block				(int first_block, const BitSetSp& bb_add);							//OR:closed range
BitSetSp&  set_block				(int first_block, int last_block, const BitSetSp& rhs);				//OR:closed range
		
inline	void  erase_bit				(int nbit);	
inline	vPB_it  erase_bit			(int nbit, vPB_it from_it);
		int	  erase_bit				(int lbit, int rbit);
		int	  clear_bit				(int lbit, int rbit);											//deallocates blocks
		void  shrink_to_fit			()	{vBB_.shrink_to_fit();}
		void  erase_bit				()	{vBB_.clear();}												//clears all bit blocks
BitSetSp&    erase_bit				(const BitSetSp&);				

BitSetSp&    erase_block			(int first_block, const BitSetSp& rhs );
BitSetSp&    erase_block			(int first_block, int last_block, const BitSetSp& rhs );
BitSetSp&    erase_block_pos		(int first_pos_of_block, const BitSetSp& rhs );

////////////////////////
//Operators 
 BitSetSp& operator &=				(const BitSetSp& );					
 BitSetSp& operator |=				(const BitSetSp& );
 BitSetSp& AND_EQ					(int first_block, const BitSetSp& rhs );						//in range
 BitSetSp& OR_EQ					(int first_block, const BitSetSp& rhs );						//in range
  

/////////////////////////////
//Boolean functions

	 /**
	 * @brief TRUE if there is a 1-bit in the position bit
	 **/
 inline	bool is_bit					(int bit)				const;								

	 /**
	 * @brief TRUE if the bitstring has all 0-bits
	 * @details O(num_bitblocks) complexity. Assumes it is possible that all existing bitblocks are 0, so 
	 *			all of them need to be checked.
	 **/
 inline	bool is_empty				()						const;		

		bool is_disjoint			(const BitSetSp& bb)	const;
		bool is_disjoint			(int first_block, int last_block, const BitSetSp& bb)   const;

////////////////////////
 //Other operations 
		void sort();
		
/////////////////////
//I/O 
	ostream& print					(ostream& = cout, bool show_pc = true, bool endl = true ) const override;

/////////////////////
//Conversions				
	string to_string				();																//TODO - operator string	();
	void to_vector					(std::vector<int>& )	const;



////////////////////////
//data members
protected:
	vPB vBB_;					//a vector of sorted pairs of a non-empty bitblock and its index in a non-sparse bitstring
	int nBB_;					//maximum number of bitblocks


};  //end class BitSetSp


//////////////////////////
//
// BOOLEAN FUNCTIONS
//
//////////////////////////

bool BitSetSp::is_bit(int bit)	const{
//note: could use find_block as well

	int blockID = WDIV(bit);

	/////////////////////////////////////////////////////////////////////////////////////
	vPB_cit it=lower_bound(vBB_.begin(), vBB_.end(), pBlock_t(blockID), pBlock_less());
	/////////////////////////////////////////////////////////////////////////////////////
	
	return ( it != vBB_.end()		&&
			 it->idx_ == blockID	&& 
			 (it->bb_ & Tables::mask[bit - WMUL(blockID) /*WMOD(bit)*/])
			);
}


bool BitSetSp::is_empty ()	const{

	//special case, no 1-bits in the bitset
	if (vBB_.empty()) {
		return true;
	}

	//check if all bitblocks are empty - assumes it is possible
	for(int i = 0; i < vBB_.size(); ++i){
		if (vBB_[i].bb_) {
			return false;
		}
	}
	return true;
}

inline
bool BitSetSp::is_disjoint	(const BitSetSp& rhs) const{
///////////////////
// true if there are no bits in common 
	int i1=0, i2=0;
	while(true){
		//exit condition I
		if(i1==vBB_.size() || i2==rhs.vBB_.size() ){		//size should be the same
					return true;
		}

		//update before either of the bitstrings has reached its end
		if(vBB_[i1].idx_==rhs.vBB_[i2].idx_){
			if(vBB_[i1].bb_ & rhs.vBB_[i2].bb_)
							return false;	//bit in common	
			++i1; ++i2;
		}else if(vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			i1++;
		}else if(rhs.vBB_[i2].idx_<vBB_[i1].idx_){
			i2++;
		}
	}

return true;
}

inline 
bool BitSetSp::is_disjoint	(int first_block, int last_block, const BitSetSp& rhs)   const{
///////////////////
// true if there are no bits in common in the closed range [first_block, last_block]
//
// REMARKS: 
// 1) no assertions on valid ranges

	int i1=0, i2=0;

	//updates initial element indexes it first_block is defined
	if(first_block>0){

		pair<bool, int> p1=this->find_block_pos(first_block);
		pair<bool, int> p2=rhs.find_block_pos(first_block);

		//checks whether both sparse bitstrings have at least one block greater or equal to first_block
		if(p1.second==BBObject::noBit || p2.second==BBObject::noBit) return true;
		i1=p1.second; i2=p2.second;
	}

	//main loop
	int nElem=this->vBB_.size(); int nElem_rhs=rhs.vBB_.size();
	while(! ((i1>=nElem || this->vBB_[i1].idx_>last_block ) || (i2>=nElem_rhs || rhs.vBB_[i2].idx_>last_block )) ){

		//update before either of the bitstrings has reached its end
		if(this->vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			i1++;
		}else if(rhs.vBB_[i2].idx_<this->vBB_[i1].idx_){
			i2++;
		}else{
			if(this->vBB_[i1].bb_ & rhs.vBB_[i2].bb_)
				return false;				
			i1++, i2++; 
		}

		/*if(lhs.vBB_[i1].idx_==rhs.vBB_[i2].idx_){
		BitSetSp::elem e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ & rhs.vBB_[i2].bb_);
		res.vBB_.push_back(e);
		i1++, i2++; 
		}else if(lhs.vBB_[i1].idx_<rhs.vBB_[i2].idx_){
		i1++;
		}else if(rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_){
		i2++;
		}*/
	}
	

return true;		//disjoint
}

////////////////
//
// Bit updates
//
/////////////////

void BitSetSp::erase_bit(int nbit /*0 based*/){
//////////////
// clears bitblock information (does not remove bitblock if empty) 
// REMARKS: range must be sorted

	int idx = WDIV(nbit);

	//equal_range implementation 
	/*pair<vPB_it, vPB_it> p=equal_range(vBB_.begin(), vBB_.end(), elem(WDIV(nbit)), pBlock_less());
	if(distance(p.first,p.second)!=0){
		(*p.first).bb_&=~Tables::mask[WMOD(nbit)];
	}*/
	
	//lower_bound implementation
	vPB_it it=lower_bound(vBB_.begin(), vBB_.end(), pBlock_t(idx), pBlock_less());
	if(it!=vBB_.end()){
		//check if the element exists already
		if (it->idx_ == idx) {
			it->bb_ &= ~Tables::mask[WMOD(nbit)];
		}
	}
}

BitSetSp::vPB_it  BitSetSp::erase_bit (int nbit, BitSetSp::vPB_it from_it){
	
	int idx=WDIV(nbit);
		
	//lower_bound implementation
	vPB_it it=lower_bound(from_it, vBB_.end(), pBlock_t(idx), pBlock_less());
	if(it!=vBB_.end()){
		//check if the element exists already
		if(it->idx_== idx)
			it->bb_&=~Tables::mask[WMOD(nbit)];
	}
return it;
}

inline
BitSetSp& BitSetSp::set_bit (int bit ){

	int block = WDIV(bit);
		
	//////////////////////////
	if (block >= nBB_) {
		LOGG_ERROR("attempted to set a bit: ", bit, "out of range - BitSetSp::set_bit ");
		return *this;
	}
	/////////////////////////
		
	//ordered insertion - lower_bound implementation which returns the first element NOT LESS than block
	vPB_it it = std::lower_bound (vBB_.begin(), vBB_.end(), pBlock_t(block), pBlock_less());
	if(it != vBB_.end()){
				
		if(it->idx_ == block){

			//check if the element exists already
			it->bb_|=Tables::mask[bit - WMUL(block) /* WMOD(bit) */];
		}
		else {

			//inserts new pBlock BEFORE iterator
			vBB_.insert(it, pBlock_t(block, Tables::mask[bit - WMUL(block) /* WMOD(bit) */]));
		}
	
	}else{
		//insertion at the end
		vBB_.emplace_back(pBlock_t(block,Tables::mask[bit - WMUL(block) /* WMOD(bit) */]));
	}
	
	return *this;  
}

inline
void BitSetSp::reset_bit (int bit){

	vBB_.clear();

	int blockID = WDIV(bit);		
	vBB_.push_back(pBlock_t(blockID, Tables::mask[bit - blockID /*WMOD(bit)*/]));

}	

////////////////
//
// Bit scanning
//
/////////////////

int BitSetSp::prev_bit	(int nBit){
/////////////////
// Uses cache of last index position for fast bit scanning
//

	if(nBit==BBObject::noBit)
			return msbn64(nElem);		//updates nElem with the corresponding bitblock
	
	int index = WDIV(nBit);
	int npos=bblock::msb64_lup(Tables::mask_low[WMOD(nBit) /*nBit-WMUL(index)*/] & vBB_[nElem].bb_);
	if(npos!=BBObject::noBit)
		return (WMUL(index) + npos);
	
	for(int i=nElem-1; i>=0; i--){  //new bitblock
		if( vBB_[i].bb_){
			nElem=i;
			return bblock::msb64_de_Bruijn(vBB_[i].bb_) + WMUL(vBB_[i].idx_);
		}
	}
return BBObject::noBit;
}


int BitSetSp::next_bit(int nBit){
/////////////////
// Uses cache of last index position for fast bit scanning
//	
	if(nBit==BBObject::noBit)
		return lsbn64(nElem);		//updates nElem with the corresponding bitblock
	
	int idx = WDIV(nBit);
	int npos=bblock::lsb64_de_Bruijn(Tables::mask_high[WMOD(nBit) /* WORD_SIZE * idx */] & vBB_[nElem].bb_);
	if(npos!=BBObject::noBit)
		return (WMUL(idx) + npos);
	
	for(int i=nElem+1; i<vBB_.size(); i++){
		//new bitblock
		if(vBB_[i].bb_){
			nElem=i;
			return bblock::lsb64_de_Bruijn(vBB_[i].bb_) + WMUL(vBB_[i].idx_);
		}
	}
return BBObject::noBit;
}

int BitSetSp::msbn64	(int& nElem)	const{
///////////////////////
// Look up table implementation (best found so far)
//
// RETURNS element index of the bitblock

	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=vBB_.size()-1; i>=0; i--){
		val.b=vBB_[i].bb_;
		if(val.b){
			nElem=i;
			if(val.c[3]) return (Tables::msba[3][val.c[3]]+WMUL(vBB_[i].idx_));
			if(val.c[2]) return (Tables::msba[2][val.c[2]]+WMUL(vBB_[i].idx_));
			if(val.c[1]) return (Tables::msba[1][val.c[1]]+WMUL(vBB_[i].idx_));
			if(val.c[0]) return (Tables::msba[0][val.c[0]]+WMUL(vBB_[i].idx_));
		}
	}

return BBObject::noBit;		//should not reach here
}

int BitSetSp::lsbn64 (int& nElem)		const	{
/////////////////
// different implementations of lsbn depending on configuration
//
// RETURNS element index of the bitblock
	
#ifdef DE_BRUIJN
	for(int i=0; i<vBB_.size(); i++){
		if(vBB_[i].bb_){
			nElem=i;
#ifdef ISOLANI_LSB
			return(Tables::indexDeBruijn64_ISOL[((vBB_[i].bb_ & -vBB_[i].bb_) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(vBB_[i].idx_));	
#else
			return(Tables::indexDeBruijn64_SEP[((vBB_[i].bb_ ^ (vBB_[i].bb_ - 1)) * bblock::DEBRUIJN_MN_64_SEP/*magic num*/) >> bblock::DEBRUIJN_MN_64_SHIFT] + WMUL(vBB_[i].idx_));
#endif
		}
	}
#elif LOOKUP
	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=0; i<m_nBB; i++){
		val.b=vBB_[i].bb_;
		nElem=i;
		if(val.b){
			if(val.c[0]) return (Tables::lsba[0][val.c[0]]+WMUL(vBB_[i].idx_));
			if(val.c[1]) return (Tables::lsba[1][val.c[1]]+WMUL(vBB_[i].idx_));
			if(val.c[2]) return (Tables::lsba[2][val.c[2]]+WMUL(vBB_[i].idx_));
			if(val.c[3]) return (Tables::lsba[3][val.c[3]]+WMUL(vBB_[i].idx_));
		}
	}

#endif
return BBObject::noBit;	
}

inline
int BitSetSp::popcn64() const{
	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	for(int i=0; i<vBB_.size(); i++){
		val.b = vBB_[i].bb_; 
		npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

return npc;
}

inline
int BitSetSp::popcn64(int nBit) const{
	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	int nBB=WDIV(nBit);

	//find the biblock if it exists
	vPB_cit it=lower_bound(vBB_.begin(), vBB_.end(), pBlock_t(nBB), pBlock_less());
	if(it!=vBB_.end()){
		if(it->idx_==nBB){
			val.b= it->bb_&~Tables::mask_low[WMOD(nBit)];
			npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
			it++;
		}
		
		//searches in the rest of elements with greater index than nBB
		for(; it!=vBB_.end(); ++it){
			val.b = it->bb_; //Loads union
			npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
		}
	}
	
return npc;
}

inline
BitSetSp& AND (const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res){
///////////////////////////
// AND between sparse sets
//		
	int i2=0;
	res.erase_bit();					//experimental (and simplest solution)
	const int MAX=rhs.vBB_.size()-1;

	//empty check of rhs required, the way it is implemented
	if(MAX==BBObject::noBit) return res;
	
	//optimization which works if lhs has less 1-bits than rhs
	int lhs_SIZE=lhs.vBB_.size();
	for (int i1 = 0; i1 < lhs_SIZE;i1++){
		for(; i2<MAX && rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_; i2++){}
		
		//update before either of the bitstrings has reached its end
		if(lhs.vBB_[i1].idx_ == rhs.vBB_[i2].idx_){
				res.vBB_.push_back(BitSetSp::pBlock_t(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ & rhs.vBB_[i2].bb_));
		}
	}

	//general purpose code assuming no a priori knowledge about density in lhs and rhs
	//int i1=0, i2=0;
	//while(i1!=lhs.vBB_.size() && i2!=rhs.vBB_.size() ){
	//	//update before either of the bitstrings has reached its end
	//	if(lhs.vBB_[i1].idx_<rhs.vBB_[i2].idx_){
	//		i1++;
	//	}else if(rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_){
	//		i2++;
	//	}else{
	//		BitSetSp::elem e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ & rhs.vBB_[i2].bb_);
	//		res.vBB_.push_back(e);
	//		i1++, i2++; 
	//	}
	//	
	//}
return res;
}

inline
BitSetSp& AND (int first_block, const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res){
///////////////////////////
// AND between sparse sets from first_block (included) onwards
//
			
	res.erase_bit();
	pair<bool, int> p1=lhs.find_block_pos(first_block);
	pair<bool, int> p2=rhs.find_block_pos(first_block);
	if(p1.second!=BBObject::noBit && p2.second!=BBObject::noBit){
		int i1=p1.second, i2=p2.second;
		while( i1!=lhs.vBB_.size() && i2!=rhs.vBB_.size() ){
			
			//update before either of the bitstrings has reached its end
			if(lhs.vBB_[i1].idx_<rhs.vBB_[i2].idx_){
				i1++;
			}else if(rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_){
				i2++;
			}else{
				BitSetSp::pBlock_t e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ & rhs.vBB_[i2].bb_);
				res.vBB_.push_back(e);
				i1++, i2++; 
			}


		/*	if(lhs.vBB_[i1].idx_==rhs.vBB_[i2].idx_){
				BitSetSp::elem e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ & rhs.vBB_[i2].bb_);
				res.vBB_.push_back(e);
				i1++, i2++; 
			}else if(lhs.vBB_[i1].idx_<rhs.vBB_[i2].idx_){
				i1++;
			}else if(rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_){
				i2++;
			}*/
		}
	}
return res;
}


inline
BitSetSp& AND (int first_block, int last_block, const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res){
///////////////////////////
// AND between sparse sets in closed range
// last update: 21/3/15 -BUG corrected concerning last_block and first_block value optimization  
//
// REMARKS: no assertions on valid ranges
		
	res.erase_bit();
	int i1=0, i2=0;

	//updates initial element indexes it first_block is defined
	if(first_block>0){

		pair<bool, int> p1=lhs.find_block_pos(first_block);
		pair<bool, int> p2=rhs.find_block_pos(first_block);

		//checks whether both sparse bitstrings have at least one block greater or equal to first_block
		if(p1.second==BBObject::noBit || p2.second==BBObject::noBit) return res;
		i1=p1.second; i2=p2.second;
	}


	//main loop	
	int nElem_lhs=lhs.vBB_.size(); int nElem_rhs=rhs.vBB_.size();
	while(! ((i1>=nElem_lhs|| lhs.vBB_[i1].idx_>last_block) || (i2>=nElem_rhs || rhs.vBB_[i2].idx_>last_block)) ){
			
		//update before either of the bitstrings has reached its end
		if(lhs.vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			i1++;
		}else if(rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_){
			i2++;
		}else{
			BitSetSp::pBlock_t e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ & rhs.vBB_[i2].bb_);
			res.vBB_.push_back(e);
			i1++, i2++; 
		}

		/*if(lhs.vBB_[i1].idx_==rhs.vBB_[i2].idx_){
			BitSetSp::elem e(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ & rhs.vBB_[i2].bb_);
			res.vBB_.push_back(e);
			i1++, i2++; 
		}else if(lhs.vBB_[i1].idx_<rhs.vBB_[i2].idx_){
			i1++;
		}else if(rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_){
			i2++;
		}*/
	}

return res;
}

inline
BitSetSp&  ERASE (const BitSetSp& lhs, const BitSetSp& rhs,  BitSetSp& res){
/////////////////
// removes rhs from lhs
// date of creation: 17/12/15

	
	const int MAX=rhs.vBB_.size()-1;
	if(MAX==BBObject::noBit){ return (res=lhs);  }		//copy before returning
	res.erase_bit();

	//this works better if lhs is as sparse as possible (iterating first over rhs is illogical here becuase the operation is not symmetrical)
	int i2=0;
	int lhs_SIZE=lhs.vBB_.size();
	for (int i1 = 0; i1 < lhs_SIZE; i1++){
		for(; i2<MAX && rhs.vBB_[i2].idx_<lhs.vBB_[i1].idx_; i2++){}
		
		//update before either of the bitstrings has reached its end
		if(lhs.vBB_[i1].idx_==rhs.vBB_[i2].idx_){
				res.vBB_.push_back(BitSetSp::pBlock_t(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_ &~ rhs.vBB_[i2].bb_));
		}else{
			res.vBB_.push_back(BitSetSp::pBlock_t(lhs.vBB_[i1].idx_, lhs.vBB_[i1].bb_));
		}
	}
return res;
}

inline
BitSetSp&  BitSetSp::erase_block (int first_block, const BitSetSp& rhs ){
////////////////////
// removes 1-bits from current object (equialent to set_difference) from first_block (included) onwards			

	pair<bool, BitSetSp::vPB_it> p1=find_block_ext(first_block);
	pair<bool, BitSetSp::vPB_cit> p2=rhs.find_block_ext(first_block);
	
	//optimization based on the size of rhs being greater
	//for (int i1 = 0; i1 < lhs.vBB_.size();i1++){

	//iteration
	while( ! ( p1.second==vBB_.end() ||  p2.second==rhs.vBB_.end() ) ){
		////exit condition I
		//if(p1.second==vBB_.end() || p2.second==rhs.vBB_.end() ){		//should be the same
		//	return *this;
		//}

		//update before either of the bitstrings has reached its end
		if(p1.second->idx_<p2.second->idx_){
			++p1.second;
		}else if(p2.second->idx_<p1.second->idx_){
			++p2.second;
		}else{
			p1.second->bb_&=~p2.second->bb_;
			++p1.second, ++p2.second; 
		}

	}
	
return *this;
}

inline
BitSetSp&  BitSetSp::AND_EQ(int first_block, const BitSetSp& rhs ){
//////////////////////
// left intersection (AND). bits in rhs remain starting from closed range [first_block, END[

	pair<bool, BitSetSp::vPB_it> p1 = this->find_block_ext(first_block);
	pair<bool, BitSetSp::vPB_cit> p2 = rhs.find_block_ext(first_block);
	
	//optimization based on the size of rhs being greater
	//for (int i1 = 0; i1 < lhs.vBB_.size();i1++){...}

	//iteration
	while( true ){
		//exit condition 
		if(p1.second==vBB_.end() ){		//size should be the same
					return *this;
		}else if( p2.second==rhs.vBB_.end()){  //fill with zeros from last block in rhs onwards
			for(; p1.second!=vBB_.end(); ++p1.second)
				p1.second->bb_=ZERO;
			return *this;
		}

		//update before either of the bitstrings has reached its end
		if(p1.second->idx_<p2.second->idx_){
			p1.second->bb_=0;
			++p1.second;
		}else if(p2.second->idx_<p1.second->idx_){
			++p2.second;
		}else{
			p1.second->bb_&=p2.second->bb_;
			++p1.second, ++p2.second; 
		}

	}
	
return *this;
}

inline
BitSetSp&  BitSetSp::OR_EQ(int first_block, const BitSetSp& rhs ){
//////////////////////
// left union (OR). Bits in rhs are added starting from closed range [first_block, END[

	pair<bool, BitSetSp::vPB_it> p1=find_block_ext(first_block);
	pair<bool, BitSetSp::vPB_cit> p2=rhs.find_block_ext(first_block);
	
	//iteration
	while(true){
		//exit condition 
		if(p1.second==vBB_.end() || p2.second==rhs.vBB_.end() ){				//size should be the same
					return *this;
		}

		//update before either of the bitstrings has reached its end
		if(p1.second->idx_<p2.second->idx_){
			++p1.second;
		}else if(p2.second->idx_<p1.second->idx_){
			++p2.second;
		}else{
			p1.second->bb_|=p2.second->bb_;
			++p1.second, ++p2.second; 
		}
	}
	
return *this;
 }


inline
BitSetSp&  BitSetSp::erase_block_pos (int first_pos_of_block, const BitSetSp& rhs ){
/////////////////////
// erases bits from a starting block in *this (given as the position in the bitstring collection, not its index) till the end of the bitstring, 

	int i2=0;							//all blocks in rhs are considered
	const int MAX=rhs.vBB_.size()-1;
	
	//optimization which works if rhs has less 1-bits than this
	for (int i1 = first_pos_of_block; i1 <vBB_.size(); i1++){
		for(; i2<MAX && rhs.vBB_[i2].idx_< vBB_[i1].idx_; i2++){}
		
		//update before either of the bitstrings has reached its end
		if(vBB_[i1].idx_==rhs.vBB_[i2].idx_){
				vBB_[i1].bb_&=~rhs.vBB_[i2].bb_;
		}
	}

return *this;
}


inline
BitSetSp&  BitSetSp::erase_block (int first_block, int last_block, const BitSetSp& rhs ){
////////////////////
// removes 1-bits from current object (equialent to set_difference) from CLOSED RANGE of blocks	

	pair<bool, BitSetSp::vPB_it> p1 = find_block_ext(first_block);
	pair<bool, BitSetSp::vPB_cit> p2 = rhs.find_block_ext(first_block);
	if(p1.second==vBB_.end() || p2.second==rhs.vBB_.end() )
		 return *this;

	//iterates	
	do{
		//update before either of the bitstrings has reached its end
		if(p1.second->idx_==p2.second->idx_){
			p1.second->bb_&=~p2.second->bb_;
			++p1.second, ++p2.second; 
		}else if(p1.second->idx_<p2.second->idx_){
			++p1.second;
		}else if(p2.second->idx_<p1.second->idx_){
			++p2.second;
		}

		//exit condition I
		if(p1.second==vBB_.end() || p1.second->idx_>last_block || p2.second==rhs.vBB_.end() || p2.second->idx_>last_block ){
			break;
		}
	}while(true);
	
return *this;
}


inline
int	 BitSetSp::erase_bit (int low, int high){
///////////////////
// clears bits in the corresponding CLOSED range
		
	int	bbh=WDIV(high); 
	int bbl=WDIV(low); 

	//checks consistency (ASSERT)
	if(bbh<bbl || bbl<0 || low>high || low<0){
		cerr<<"Error in set bit in range"<<endl;
		return -1;
	}
		
	//finds low bitblock and updates forward
	vPB_it itl=lower_bound(vBB_.begin(), vBB_.end(), pBlock_t(bbl), pBlock_less());
	if(itl!=vBB_.end()){
		if(itl->idx_==bbl){	//lower block exists
			if(bbh==bbl){		//case update in the same bitblock
				BITBOARD bb_low=itl->bb_ & Tables::mask_high[high-WMUL(bbh)];
				BITBOARD bb_high=itl->bb_ &Tables::mask_low[low-WMUL(bbl)];
				itl->bb_=bb_low | bb_high;
				return 0;
			}

			//update lower block
			itl->bb_&=Tables::mask_low[low-WMUL(bbl)];
			++itl;
		}

		//iterate over the rest
		for(; itl!=vBB_.end(); ++itl){
			if(itl->idx_>=bbh){		//exit condition
				if(itl->idx_==bbh){	//extra processing if the end block exists
					if(bbh==bbl){		
						BITBOARD bb_low=itl->bb_ & Tables::mask_high[high-WMUL(bbh)];
						BITBOARD bb_high=itl->bb_ &Tables::mask_low[low-WMUL(bbl)];
						itl->bb_=bb_low | bb_high;
						return 0;
					}

					itl->bb_ &=Tables::mask_high[high-WMUL(bbh)];
				}
			return 0;
			}
			//Deletes block
			itl->bb_=ZERO;
		}
	}
return 0;
}

inline
int BitSetSp::reset_bit(int high, const BitSetSp& bb_add){

	
	vBB_.clear();
	int	bbh=WDIV(high); 
	pair<bool, BitSetSp::vPB_cit> p = bb_add.find_block_ext(bbh);
	if(p.second==bb_add.cend())
		copy(bb_add.cbegin(), bb_add.cend(),insert_iterator<vPB>(vBB_,vBB_.begin()));
	else{
		if(p.first){
			copy(bb_add.cbegin(), p.second, insert_iterator<vPB>(vBB_,vBB_.begin()));
			vBB_.emplace_back(pBlock_t(bbh, p.second->bb_ & ~Tables::mask_high[high-WMUL(bbh)]));
		}else{
			copy(bb_add.cbegin(), ++p.second, insert_iterator<vPB>(vBB_,vBB_.begin()));
		}
	}
return 0;
}

inline
int  BitSetSp::reset_bit (int low, int high,  const BitSetSp& bb_add){
/////////////
// fast copying of bb_add in the corresponding CLOSED range
// date of creation: 19/12/15 (bug corrected in 6/01/2016)

	//***ASSERT

	vBB_.clear();
	int	bbl=WDIV(low);
	int	bbh=WDIV(high);


	//finds low bitblock and updates forward
	vPB_cit itl=lower_bound(bb_add.cbegin(), bb_add.cend(), pBlock_t(bbl), pBlock_less());
	if(itl!=bb_add.cend()){
		if(itl->idx_==bbl){	//lower block exists
			if(bbh==bbl){		//case update in the same bitblock
				vBB_.push_back(pBlock_t( bbh, itl->bb_ & bblock::MASK_1(low-WMUL(bbl), high-WMUL(bbh)) ));
				return 0;
			}else{
				//add lower block
				vBB_.push_back(pBlock_t(bbl, itl->bb_ &~ Tables::mask_low[low-WMUL(bbl)] ));
				++itl;
			}
		}

		//copied the rest if elements
		for(; itl!=bb_add.cend(); ++itl){
			if(itl->idx_>=bbh){		//exit condition
				if(itl->idx_==bbh){	
					vBB_.push_back(pBlock_t(bbh, itl->bb_&~Tables::mask_high[high-WMUL(bbh)]));
				}
			return 0;
			}
			//copies the element as is
			vBB_.push_back(*itl);
		}
	}

return 0;		//should not reach here
}

inline
bool operator == (const BitSetSp& lhs, const BitSetSp& rhs){
/////////////////////
// Simple equality check which considers exact copy of bit strings 
// REMARKS: does not take into account information, i.e. bit blocks=0

	return(lhs.vBB_==rhs.vBB_);
}

inline
bool operator!=	(const BitSetSp& lhs, const BitSetSp& rhs){
	return ! operator==(lhs, rhs);
}

#endif