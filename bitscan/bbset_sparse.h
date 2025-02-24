 /**
  * @file bbset_sparse.h
  * @brief header for sparse class equivalent to the BitSetSp class
  * @author pss
  * @details created 19/12/2015?, @last_update 15/02/2025
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

/////////////////////////////
// Independent operators / masks  
public:	
		
	friend bool operator ==			(const BitSetSp& lhs, const BitSetSp& rhs);
	friend bool operator !=			(const BitSetSp& lhs, const BitSetSp& rhs) { return !(lhs == rhs); }

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

	/**
	* @brief reallocates memory to the number of blocks of the bitset
	**/
	void  shrink_to_fit				()				 { vBB_.shrink_to_fit(); }


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
	* @brief Finds the bitblock given its block index blockID and its position in the collection. If it does not exists
	*		 provides bitblock closes to blockID with greater index. Depending on the Policy parameter, it can also provide
	*		 the position of the latter bitblock.		
	* @param blockID: index of the block
	* @param pos: OUTPUT param which provides I) the position of the block in the collection with index BlockID, 
	*			  II) BBObject::noBit depending if the block does not exist or the Policy parameter
	* param Policy_iterPos: template param which, if true, asks for the position of the block returned
	* @returns: iterator to the closest bitblock with index greater or equal to blocKID
	*			or END() if the block does not exist. Depending on the Policy parameter, it can also provide
	*			the position of the bitblock pointed by the iterator. If the block does not exist, and the Policy iterPos = false
	*			pos = BBObject::noBit.
	* @details O(log) complexity
	* @details two implementations, read only and read-write
	**/
	template<bool Policy_iterPos = false>
	vPB_cit find_block				(int blockID, int& pos)	const;

	template<bool Policy_iterPos = false>
	vPB_it  find_block				(int blockID, int& pos);

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
	* @brief finds the position (index) of the bitblock with index blockID
	* @param blockID: input index of the block searched
	* @returns a pair with first:true if the block exists, and second: the index of the block in the collection
	*		   In the case the block does not exist (first element of the pair is false),
	*		   the second element is the closest block index ABOVE the blockID or -1 if no such block exists
	* @details O(log) complexity
	* 
	* TODO - possibly remove - since it can be obained from find_block_ext trivially (21/02/2025)
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
	* @brief Sets THIS to the singleton bit 
	**/
BitSetSp&  reset_bit				(int bit);
	
	/**
	* @brief Sets THIS to 1-bits in the	 range [firstBit, lastBit]
	**/
	BitSetSp&   reset_bit			(int firstBit, int lastBit);	

	/**
	* @brief Sets THIS to rhs in the range [0, lastBit]
	* @details: more efficient than using the more general reset_bit in the
	*			range [firstBit=0, lastBit]
	**/
protected:
	BitSetSp&   reset_bit			(int lastBit, const BitSetSp& rhs);	
	
	/**
	* @brief Sets THIS to rhs in the closed range [firstBit, lastBit]
	**/
public:
	BitSetSp&   reset_bit			(int firstBit, int lastBit, const BitSetSp& rhs);
		
	/**
	* @brief Sets bit in THIS. If bit is outside the capacity of the bitset 
	*		 it is ignored (feature)
	* @param bit: position of the bit to set
	* @returns 0 if the bit was set, -1 if error
	* @details emplaces pBlock in the bitstring or changes an existing bitblock
	* @detials O (log n) to determine if the block constaining firstBit is present
	* @details O (log n) to insert the new block according to index 
	*		   ( + n potential shifts since it is a vector) 
	**/
inline	BitSetSp& set_bit			(int bit);

	/**
	* @brief sets bits in the closed range [firstBit, lastBit]
	* @params firstBit, lastBit: range
	* @returns 0 if the bits were set, -1 if error
	* @details only one binary search is performed for the lower block
	* @details  I.  O(n)  to update/add blocks, where n = lastBit - firstBit
	* @details  II. O(log n) to determine the closest block to 
	*				the first block of the range
	* @details  III.O(n log n) average case for sorting (if required)
	**/
BitSetSp&	set_bit					(int firstBit, int lastBit);

	/**
	* @brief Adds the bits from the bitstring bitset in the population
	*		 range of *THIS (bitblocks are copied). 
	*
	*		 I. Both bitsets MUST have the SAME capacity (number of blocks).
	*		II. Should have the same expected maximum population size 
	*		
	* @details  Equivalent to OR operation / set union
	* @details  Does not use iterators - not necessary to allocate memory a priori
	* @returns  reference to the modified bitstring
	**/		
BitSetSp&   set_bit					(const BitSetSp& bitset);	
BitSetSp&	set_bit					(int firstBit, int lastBit, const BitSetSp& bitset) = delete;	 //TODO

	/**
	* @brief Adds 1-bits in bitset in the closed range [firstBlock, lastBlock] to *this
	*		 (0 <= firstBlock <= lastBlock < the number of bitblocks in the bitset).
	* @param bitset input bitstring to be copied
	* @param firstBlock, lastBlock: input closed range of bitset
	* @returns reference to the modified bitstring
	* @details Does not require to allocate memory a priori
	**/
BitSetSp&  set_block				(int firstBlock, int lastBlock, const BitSetSp& bitset);

protected:
	/**
	* @brief Bitwise OR operator with bitset in the semi open range [firstBlock, END)
	* @details apply for set union
	*
	* TODO - CHECK, possible OR operation is missing (23/02/2025)
	**/
BitSetSp& set_block					(int firstBlock, const BitSetSp& bitset);

public:
	/**
	* @brief sets bit to 0 in the bitstring
	* @param  bit: position of the 1-bit to set (>=0)
	* @returns reference to the modified bitstring
	* @details: zero blocks are not removed
	**/
inline	BitSetSp& erase_bit			(int bit);

	/**
	* @brief sets the bits in the closed range [firstBit, lastBit] to 0 in the bitstring
	* @param firstBit:, lastBit: 0 < firstBit <= lastBit
	* @details: last_update 19/02/25
	* @details: zero blocks are not removed
	**/
BitSetSp& erase_bit					(int firstBit, int lastBit);
	
	/**
	/* @brief sets bit to 0 in the bitstring and returns t
	 * @returns the iterator to the block where the bit was set to 0, if it exists, otherwise
	 *			the iterator to the block with the closest greater index than the bit (can be END).
	 * @details oriented to basic bitscanning
	 * @details: zero blocks are not removed
	 * 
	 * TODO -  improve or remove (19/02/2025)
	 **/
inline	vPB_it  erase_bit			(int bit, vPB_it from_it);

	/**
	* @brief sets all bits to 0
	* @details: As opposed to the non-sparse case, this is an O(1) operation 
	**/
	void  erase_bit					()								{ vBB_.clear(); }												
		
	/**
	* @brief Removes the 1-bits from bitset (inside *this capacity).
	*
	*		 I. bitset must have a maximum population greater or equal than the bitstring (CHECK - 19/02/2025)
	*
	* @details Equivalent to a set minus operation
	* @details: zero blocks are not removed
	* @returns reference to the modified bitstring
	**/
BitSetSp& erase_bit					(const BitSetSp& bitset);

	/**
	* @brief erase operation which effectively removes the zero blocks
	* @details EXPERIMENTAL - does not look efficient, 
	* 
	* TODO - refactor, actually used elsewhere in GRAPH. CHECK!! (19/02/2025)
	**/
	int	  clear_bit					(int firstBit, int lastBit);											
		
	/**
	* @brief Deletes the 1-bits from the bitstring bb_del in the closed range [firstBlock, lastBlock].
	*		 If lastBlock == -1, the range is [firstBlock, END).
	* @details:  0 <= firstBlock <= lastBlock < the number of bitblocks in the 
	* @details:  Equivalent to a set minus operation
	*
	* @param firstBlock:, lastBlock: input closed range of bitset
	* @param bitset: input bitstring whose 1-bits are to be removed from *this
	* @returns reference to the modified bitstring
	**/
BitSetSp& erase_block				(int firstBlock, int lastBlock, const BitSetSp& bitset);

protected:
	/**
	* @brief Deletes the 1-bits from the bitstring bb_del in the SEMI_OPEN range [firstBlock, min{rhs.nBB_, *this->nBB_})
	*		 (set minus operation)
	* @details: can be substituted by the call erase_block(firstBLock, *this->nBB_), but should be more efficient
	*
	* @param firstBlock: initial block of the reange
	* @param bitset: input bitstring whose 1-bits are to be removed from *this
	* @returns reference to the modified bitstring
	**/
BitSetSp&  erase_block				(int firstBlock, const BitSetSp& bitset );

public:
	/**
	* @brief erase_block in the range [position of the block in the bitset, END)
	* @details weird function, referring to the position in the bitset and not the
	*		   block index. 
	* 
	* TODO - remove after checking calls  (22/02/2025)
	**/
BitSetSp& erase_block_pos			(int firstBlockPos, const BitSetSp& rhs) = delete;

////////////////////////
//Operators (member functions)

	/**
	* @brief Bitwise AND operator with rhs
	* @details apply for set intersection
	**/
BitSetSp& operator &=				(const BitSetSp& rhs);	

	/**
	* @brief Bitwise OR operator with rhs
	* @details set union operation
	**/
BitSetSp& operator |=				(const BitSetSp& rhs);

	/**
	* @brief Bitwise XOR operator with rhs
	* @details set symmetric difference operation
	**/
BitSetSp& operator ^=				(const BitSetSp& rhs) ;

	/**
	* @brief Bitwise AND operator with rhs in the range [firstBlock, END)
	* @details set intersection operation
	* @details Capacities of THIS and bitset should be the same.
	*
	* TODO - check semantics when the capacity of bitset is greater than the capacity of THIS (23/02/2025)
	**/	
BitSetSp& AND_block					(int firstBlock, const BitSetSp& rhs);

	/////////////////////////
	//TODO - (19/02/2025)

BitSetSp& AND_block					(int firstBlock, int lastBlock, const BitSetSp& bitset) = delete;

/////////////////////////////
//Boolean functions

	 /**
	 * @brief TRUE if there is a 1-bit in the position bit
	 **/
 inline	bool is_bit					(int bit)				const;								

	 /**
	 * @brief TRUE if the bitstring has all 0-bits
	 * @details O(num_bitblocks) complexity. Assumes it is possible 
	 *			that there are blocks with all 0-bits, so all of them need
	 *			to be checked.
	 **/
 inline	bool is_empty				()						const;		

	/**
	* @brief TRUE if this bitstring has no bits in common with rhs
	**/
	bool is_disjoint				(const BitSetSp& bb)	const;

	/**
	* @brief TRUE if this bitstring has no bits in common with rhs 
	*		 in the closed range [first_block, last_block]	
	**/
	bool is_disjoint_block			(int first_block, int last_block, const BitSetSp& bb)   const;

////////////////////////
 //Other operations 

		void sort					()				{ std::sort(vBB_.begin(), vBB_.end(), pBlock_less()); }
		
/////////////////////
//I/O 

	/**
	* @brief streams bb and its size to @o  with format 
	*		 [bit1 bit2 bit3 ... <(pc)>] - (if empty: [ ]) 
	* @param o: output stream
	* @param show_pc: if true, shows popcount
	* @param endl: if true, adds a new line at the end
	* @returns output stream
	* @details uses basic bitscanning implementation to enumerate the bits
	**/
	ostream& print					(ostream& o = cout, bool show_pc = true, bool endl = true ) const override;

/////////////////////
//Conversions
// 		
 	/**
	* @brief converts the bitstring to a string of 1-bits with format
	*        [bit1 bit2 bit3 ... <(pc)>] - (if empty: [ ]) 
	* @details same as print, but returns a string
	* @returns string representation of the bitstring
	* 
	* TODO implement - cast operator (24/02/2025)
	**/
	string to_string				();													
	
	/**
	* @brief Converts the bitstring to a std::vector of non-negative integers.
	*		 The size of the vector is the number of bits in the bitstring.
	* @param lb: output vector
	**/
	void to_vector					(std::vector<int>& lb)		const;							

	/**
	* @brief Casts the bitstring to a vector of non-negative integers
	* @details calls to_vector
	**/
	operator vint()								const;

/////////////////////
//data members

protected:
	vPB vBB_;					//a vector of sorted pairs of a non-empty bitblock and its index in a non-sparse bitstring
	int nBB_;					//maximum number of bitblocks

}; //end BitSetSp class


/////////////////////////////////////////
//
// INLINE FUNCTIONS - necessary implementations of BitSetSP in the header


inline
bool BitSetSp::is_bit(int bit)	const{

	int bb = WDIV(bit);

	/////////////////////////////////////////////////////////////////////////////////////
	vPB_cit it = lower_bound(vBB_.begin(), vBB_.end(), pBlock_t(bb), pBlock_less());
	/////////////////////////////////////////////////////////////////////////////////////
	
	return ( it != vBB_.end()								&&
			 it->idx_ == bb									&& 
			 (it->bb_ & bblock::MASK_BIT(bit - WMUL(bb))) 		);
}

inline
bool BitSetSp::is_empty ()	const{

	if (!vBB_.empty()) {

		//check if all bitblocks are empty - assumes it is possible, since erase operations allow it
		for (int i = 0; i < vBB_.size(); ++i) {
			if (vBB_[i].bb_) {
				return false;
			}
		}
	}

	return true;
}

inline
bool BitSetSp::is_disjoint	(const BitSetSp& rhs) const{
	
	auto itL = vBB_.begin();	
	auto itR = rhs.vBB_.begin();	
		
	//main loop
	while(itL != vBB_.end() && itR != rhs.vBB_.end())
	{
		if (itL->idx_ < itR->idx_) {
			itL++;
		}
		else if (itL->idx_ > itR->idx_ ) {
			itR++;
		}
		else{		
			if (itL->bb_ & itR->bb_) {
				return false;				//bits in common	
			}
			++itL; 
			++itR;
		}
	}

return true;
}

inline 
bool BitSetSp::is_disjoint_block (int first_block, int last_block, const BitSetSp& rhs)   const{

	////////////////////////////////////////////////////////
	assert(first_block >= 0 && first_block <= last_block);
	////////////////////////////////////////////////////////

	auto posL = BBObject::noBit;
	auto posR = BBObject::noBit;
	auto itL = find_block(first_block, posL);
	auto itR = rhs.find_block(first_block, posR);

	//main loop
	while (	itL != vBB_.end()			&&
			itL->idx_ <= last_block		&&
			itR != rhs.vBB_.end()		&&
			itR->idx_ <= last_block			)
	{

		if (itL->idx_ < itR->idx_) {
			++itL;
		}
		else if (itL->idx_ > itR->idx_) {
			++itR;

		}
		else {
			//same block index
			if (itL->bb_ & itR->bb_) {
				//////////////
				return false;					//bits in common	
				//////////////
			}
			++itL;
			++itR;
		}	

	}//end while


	return true;		
}

///////////////////
// Bit updates

inline
BitSetSp& 
BitSetSp::erase_bit (int bit){

	auto bb = WDIV(bit);

	//find closest block to blockID greater or equal
	auto it = lower_bound(vBB_.begin(), vBB_.end(), pBlock_t(bb), pBlock_less());

	if(it!=vBB_.end() && it->idx_ == bb){

		//removes the bit
		it->bb_ &= ~bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */);
	}

	return *this;
}

inline
BitSetSp::vPB_it  
BitSetSp::erase_bit (int bit, BitSetSp::vPB_it from_it){
	
	int bb = WDIV(bit);
		
	//iterator to the block of the bit if it exists or the closest non-empty block with greater index
	auto it = lower_bound (from_it, vBB_.end(), pBlock_t(bb), pBlock_less());

	if(it != vBB_.end() && it->idx_ == bb ){	

		//erase bit if the block exists
		it->bb_ &= ~bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */);		
	}

	return it;
}

inline
BitSetSp& 
BitSetSp::set_bit (int bit ){

	auto bb = WDIV(bit);

	//////////////////////
	assert(bb < nBB_);
	//////////////////////
			
	//find closest block to bb in THIS
	auto pL = find_block_ext(bb);
	
	if (pL.first) {
		//bb exists - overwrite
		pL.second->bb_ |= bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */);
	}
	else {
		if (pL.second == vBB_.end()) {
			//there are not blocks with higher index
			vBB_.emplace_back(pBlock_t(bb, bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */)));
		}
		else {
			//there are blocks with higher index, insert to avoid sorting
			vBB_.insert(pL.second, pBlock_t(bb, bblock::MASK_BIT(bit - WMUL(bb) /* WMOD(bit) */)));
		}
	}

	return *this;  
}

inline
BitSetSp& 
BitSetSp::reset_bit (int bit){

	vBB_.clear();

	auto bb = WDIV(bit);		
	vBB_.push_back(pBlock_t(bb,  bblock::MASK_BIT(bit - WMUL(bb) /*WMOD(bit)*/)));

	return *this;
}	

///////////////////
// Bit scanning

inline
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

inline
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

inline
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

inline
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

	
	const int MAX=rhs.vBB_.size() - 1;
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
BitSetSp&  BitSetSp::AND_block(int firstBlock, const BitSetSp& rhs ){

	//determine the closes block to firstBlock
	auto posL		= BBObject::noBit;
	auto itL		= this->find_block(firstBlock, posL);			//*this
	auto posR		= BBObject::noBit;
	auto itR		= rhs.find_block(firstBlock, posR);				//rhs
	
	while(itL!= vBB_.end() && itR != rhs.vBB_.end())
	{
		//update before either of the bitstrings has reached its end
		if(itL->idx_ < itR->idx_)
		{
			/////////////////////
			itL->bb_ = 0;
			/////////////////////
			++itL;

		}else if(itL->idx_ > itR->idx_)
		{
			++itR;

		}else{

			//blocks with same index

			///////////////////////////////////
			itL->bb_ &= itR->bb_;
			///////////////////////////////////

			++itL;
			++itR;
		}
	}

	//fill with zeros from last block in *this onwards if rhs has been examined
	if (itR == rhs.vBB_.end()) { 
				
		for (; itL != vBB_.end(); ++itL) {

			///////////////////////
			itL->bb_ = ZERO;
			////////////////////////
		}
	}
		
	return *this;
}

inline
BitSetSp&  BitSetSp::set_block(int firstBlock, const BitSetSp& rhs ){
		
	//determine the closes block to firstBlock
	auto  pairTHIS = this->find_block_ext(firstBlock);		//*this
	auto  pairR = rhs.find_block_ext(firstBlock);			 //rhs	
	

	while (pairTHIS.second < vBB_.end() && pairR.second < rhs.vBB_.end())
	{
		//update before either of the bitstrings has reached its end
		if (pairTHIS.second->idx_ < pairR.second->idx_)
		{
			++pairTHIS.second;
		}
		else if (pairR.second->idx_ < pairTHIS.second->idx_)
		{
			//OR - add block from rhs
			++pairR.second;
		}
		else {

			//blocks with same index

			///////////////////////////////////
			pairTHIS.second->bb_ |= pairR.second->bb_;
			///////////////////////////////////

			++pairTHIS.second;
			++pairR.second;
		}
	}

	//rhs unfinished with index below the last block of *this
	if (pairTHIS.second == vBB_.end()) {

		for (; pairR.second < rhs.vBB_.end(); ++pairR.second) {

			//////////////////////////////////
			vBB_.emplace_back(*pairR.second);
			///////////////////////////////////
		}

	}
	
	return *this;
 }


inline 
BitSetSp& BitSetSp::erase_block(int firstBlock, int lastBlock, const BitSetSp& rhs)
{
	//special case until the end of the bitset
	if (lastBlock == -1) {
		return erase_block(firstBlock, rhs);
	}

	//////////////////////////////////////////////////////////////////////////////////
	assert(firstBlock >= 0 && firstBlock <= lastBlock && lastBlock < rhs.capacity());
	//////////////////////////////////////////////////////////////////////////////////

	//determine the closest block in the range for both bitstrings
	auto pL = find_block_ext(firstBlock);
	auto pR = rhs.find_block_ext(firstBlock);

	while (	pL.second != vBB_.end()			&&	
			pR.second != rhs.vBB_.end()		&&
			pL.second->idx_ <= lastBlock	&&	
			pR.second->idx_ <= lastBlock		)
	{

		//update before either of the bitstrings has reached its end
		if (pL.second->idx_ < pR.second->idx_)
		{
			++pL.second;
		}
		else if (pL.second->idx_ > pR.second->idx_)
		{
			++pR.second;
		}
		else {  //both indexes must be equal

			//////////////////////////////////
			pL.second->bb_ &= ~pR.second->bb_;			//set minus operation
			//////////////////////////////////

			++pL.second;
			++pR.second;
		}

	}//end while

	return *this;
}

inline
BitSetSp& BitSetSp::erase_block(int firstBlock, const BitSetSp& rhs) {


	//determine the closest block in the range for both bitstrings
	auto pL = find_block_ext(firstBlock);
	auto pR = rhs.find_block_ext(firstBlock);

	//iteration
	while (  pL.second != vBB_.end()		&& 
			 pR.second != rhs.vBB_.end()		) 
	{
		
		//update before either of the bitstrings has reached its end
		if (pL.second->idx_ < pR.second->idx_) 
		{
			++pL.second;
		}
		else if (pL.second->idx_ > pR.second->idx_ ) 
		{
			++pR.second;
		}
		else {  //both indexes must be equal

			//////////////////////////////////
			pL.second->bb_ &= ~pR.second->bb_;			//set minus operation
			//////////////////////////////////

			++pL.second;
			++pR.second;
		}

	}

	return *this;
}

inline
BitSetSp& BitSetSp::erase_bit (int firstBit, int lastBit){

	//////////////////////////////////////////////
	assert(firstBit >= 0 && firstBit <= lastBit);
	//////////////////////////////////////////////
	
	auto bbh = WDIV(lastBit);
	auto bbl = WDIV(firstBit);

	/////////////////////////
	assert(bbh < capacity());
	/////////////////////////

	auto offsetl = firstBit - WMUL(bbl);
	auto offseth = lastBit  - WMUL(bbh);

	//determines the block in bbl or the closest one with greater index 
	auto it = lower_bound( vBB_.begin(), vBB_.end(), pBlock_t(bbl), pBlock_less() );

	//special case - no bits to erase in the closed range
	if (it == vBB_.end()) {
		return *this;
	}

	//lower block
	if (it->idx_ == bbl)				//lower block exists
	{

		if (bbh == bbl)					//case singleton range
		{
			it->bb_ &= bblock::MASK_0(offsetl, offseth);

			//////////////
			return *this;
			/////////////
		}

		//update lower block
		it->bb_ &= bblock::MASK_0_HIGH(offsetl);

		//next block for processing
		++it;
	}

	//rest of pBlocks
	for (; it != vBB_.end(); ++it) {

		if (it->idx_ >= bbh)			//exit condition
		{
			if (it->idx_ == bbh)
			{
				//extra processing if the end block exists

				if (bbh == bbl) {

					it->bb_ &= bblock::MASK_0(offsetl, offseth);

				}
				else {
					//add last block and trim
					it->bb_ &= bblock::MASK_0_LOW(offseth);
				}

			}

			//////////////
			return *this;
			/////////////
		}

		//Deletes intermediate block
		it->bb_ = ZERO;

	}//end for - pBlock iterations
	
	return *this;
}

inline
BitSetSp& BitSetSp::reset_bit (int lastBit, const BitSetSp& rhs){

	auto bbh = WDIV(lastBit);

	////////////////////////
	assert(bbh < nBB_);
	///////////////////////

	vBB_.clear();
	
	//finds block bbh, or closest with higher index than bbh in rhs
	auto pR = rhs.find_block_ext(bbh);

	//special case - no bits to set in the closed range
	if (pR.second != rhs.cend()) {

		//copy up to and excluding bbh
		std::copy(rhs.cbegin(), pR.second, insert_iterator<vPB>(vBB_, vBB_.begin()));

		//deal with last block bbh
		if (pR.first) {
			vBB_.emplace_back(pBlock_t(bbh, pR.second->bb_ & bblock::MASK_1_LOW(lastBit - WMUL(bbh))));

		}
	}
		
	return *this;
}

inline
BitSetSp&  BitSetSp::reset_bit (int firstBit,  int lastBit,  const BitSetSp& rhs){

	//////////////////////////////
	//special case - range starts from the beginning, called specialized case
	if (firstBit == 0) {
		return reset_bit(lastBit, rhs);	
	}
	///////////////////////////////

	auto bbl = WDIV(firstBit);
	auto bbh = WDIV(lastBit);
	
	////////////////////////////////////////////////////////////////////////////////////
	assert( (bbl >= 0) && (bbl <= bbh) && (bbh < nBB_) && (bbh < rhs.capacity()) );
	///////////////////////////////////////////////////////////////////////////////////

	vBB_.clear();

	//finds bbl or closest block with greater indx
	auto it = lower_bound(rhs.cbegin(), rhs.cend(), pBlock_t(bbl), pBlock_less());

	//special case - no bits to set in the closed range
	if (it == rhs.cend()) {
		return *this;
	}

	//first block exists
	auto offsetl = firstBit - WMUL(bbl);
	auto offseth = lastBit - WMUL(bbh);
	if (it->idx_ == bbl)			
	{
		//special case, the range is a singleton, exit condition
		if (bbh == bbl)			
		{
			vBB_.emplace_back(pBlock_t(bbl, it->bb_ & bblock::MASK_1(offsetl, offseth)));
			
			return *this;
		}
		else {

			//add lower block
			vBB_.emplace_back(pBlock_t(bbl, it->bb_ & bblock::MASK_1_HIGH(offsetl)));

			//next block
			++it;
		}
	} 

	//main loop copies blocks (bbl, bbh]
	//alternative implementation - find lower bound for bbh and copy (as in reset(lastBit))
	while (it != rhs.cend() && it->idx_ < bbh  ) {
		vBB_.emplace_back(*it);
		++it;
	}	

	//upate due to bbh if it exists
	if (it != rhs.cend() && it->idx_ == bbh) {

		//add and trim last block
		vBB_.push_back(pBlock_t(bbh, it->bb_ & bblock::MASK_1_LOW(offseth)));

	}

	return *this;
}

inline
bool operator == (const BitSetSp& lhs, const BitSetSp& rhs){

	return(	(lhs.nBB_ == rhs.nBB_) &&
			(lhs.vBB_ == rhs.vBB_)		);
}


template<bool Policy_iterPos>
inline
BitSetSp::vPB_cit
BitSetSp::find_block(int blockID, int& pos) const
{

	////////////////////////////////////////////////////////////////////////////////////////////
	auto it = lower_bound(vBB_.cbegin(), vBB_.cend(), pBlock_t(blockID), pBlock_less());
	////////////////////////////////////////////////////////////////////////////////////////////

	if (it != vBB_.end() && (it->idx_ == blockID || Policy_iterPos)) {
		pos = it - vBB_.begin();
	}
	else {
		pos = BBObject::noBit;
	}

	return it;
}

template<bool Policy_iterPos>
inline
BitSetSp::vPB_it
BitSetSp::find_block(int blockID, int& pos)
{

	////////////////////////////////////////////////////////////////////////////////////////////
	auto it = lower_bound(vBB_.begin(), vBB_.end(), pBlock_t(blockID), pBlock_less());
	////////////////////////////////////////////////////////////////////////////////////////////

	if (it != vBB_.end() && (it->idx_ == blockID || Policy_iterPos)) {
		pos = it - vBB_.begin();
	}
	else {
		pos = BBObject::noBit;
	}

	return it;
}

//////////////////////////
//
// DEPRECATED
//
/////////////////////////

//inline
//BitSetSp&  BitSetSp::erase_block_pos (int first_pos_of_block, const BitSetSp& rhs ){
///////////////////////
//// erases bits from a starting block in *this (given as the position in the bitstring collection, not its index) till the end of the bitstring, 
//
//	int i2 = 0;							//all blocks in rhs are considered
//	const int MAX = rhs.vBB_.size()-1;
//	
//	//optimization which works if rhs has less 1-bits than *this
//	for (int i1 = first_pos_of_block; i1 < vBB_.size(); ++i1){
//		for (; i2 < MAX && rhs.vBB_[i2].idx_ < vBB_[i1].idx_; ++i2) {
//
//			//update before either of the bitstrings has reached its end
//			if (vBB_[i1].idx_ == rhs.vBB_[i2].idx_) {
//				vBB_[i1].bb_ &= ~rhs.vBB_[i2].bb_;
//			}
//		}
//	}
//
//	return *this;
//}


#endif