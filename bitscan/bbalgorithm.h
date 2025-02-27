 /**
  * @file bbalgorithm.h
  * @brief header for the algorithms and data structures for bitstrings and bitblocks
  *		   includes namespace bbalg at the end with a few algorithms
  * @author pss
  * @details created 2017, last_update 27/02/2025
  * @details no *.cpp file - only headers
  **/

#ifndef  _BBALG_H_
#define  _BBALG_H_

#include "utils/common.h"
#include "utils/logger.h"
#include "bitscan/bbset.h"
#include <array>
#include <functional>

using namespace std;

//aliases
using bbo = BBObject;


///////////////////////
//
// namespace bbAlg for stateless functions for BitSets
//
///////////////////////

namespace bbalg {
	std::vector<int> to_vector(const BitSet& bbn);
	BITBOARD gen_random_bitblock(double p);

	template<class BitSet_t, class array_t>
	int first_k_bits(int k, BitSet_t& bb, array_t& lv);
}

//////////////////////
// 
// bbSize_t class
// 
// A very simple wrapper for any type of bitset of the BBObject hierarchy with CACHED size 
// 
///////////////////////
template <class BitSet_t>
struct bbSize_t{

//construction / destruction
	bbSize_t			(int MAX_SIZE) : pc_(0), bb_(MAX_SIZE)	{}
	bbSize_t			():	pc_ (0)								{}														
	
	//move and copy semantics allowed

//allocation
	
	/**
	* @brief Resets the original bitset and allocated new capacity
	**/
	void reset		(int MAX_SIZE)			{ bb_.reset(MAX_SIZE); pc_ = 0; }

	/**
	* @brief Equivalent to reset. Preserved for backward compatibility
	**/
	void init		(int MAX_SIZE)			{ bb_.reset(MAX_SIZE); pc_ = 0; }		
	
//setters and getters
	BITBOARD  size	()	const				{ return pc_; }

//bit twiddling
	void set_bit	(int bit)				{ bb_.set_bit(bit); ++pc_;}		
	int  erase_bit	(int bit)				{ bb_.erase_bit(bit); return(--pc_); }
	
	/**
	* @brief clears all 1-bits. If lazy is true, the bitset is not modified.
	**/
	void erase_bit	(bool lazy = false)		{ if(!lazy) {bb_.erase_bit();} pc_ = 0;}			
	

//useful operations
	int lsb			()						{ if (pc_ > 0) { return bb_.lsb(); } else return BBObject::noBit; }
	int msb			()						{ if (pc_ > 0) { return bb_.msb(); } else return BBObject::noBit; }

	/**
	* @brief pops the most significant bit. If the bitset is empty, returns BBObject::noBit
	**/
	int pop_msb		()						{ if (pc_ > 0) { int bit = bb_.msb(); bb_.erase_bit(bit); pc_--; return bit; } \
											  else return BBObject::noBit;	}
	/**
	* @brief pops the least significant bit. If the bitset is empty, returns BBObject::noBit
	**/
	int pop_lsb		()						{ if (pc_ > 0) { int bit = bb_.lsb(); bb_.erase_bit(bit); pc_--; return bit; } \
											  else return BBObject::noBit; }

	int sync_pc		()						{ pc_ = bb_.size(); return pc_; }

//bool
	bool is_empty	() const				{return (pc_ == 0);}
	bool is_sync_pc	() const				{return (pc_ == bb_.size());}	

//operators
	friend bool operator ==	(const bbSize_t& lhs, const bbSize_t& rhs)			{ return (lhs.pc_ == rhs.pc_) && (lhs.bb_ == rhs.bb_);}
	friend bool operator !=	(const bbSize_t& lhs, const bbSize_t& rhs)			{ return !(lhs == rhs); }

//I/O
	ostream& print	(ostream& o = cout, bool show_pc = true, bool eofl = true)	const;

/////////////////
// data members

	BITBOARD pc_;																	//number of 1-bits
	BitSet_t bb_;																	//any type of the BBObject hierarchy	
};

template <class BitSet_t>
ostream& bbSize_t<BitSet_t>::print(ostream& o = cout, bool show_pc = true, bool eofl = true) const {

	bb_.print(o, true, false);
	if (show_pc) { o << "[" << pc_ << "]"; }
	if (eofl) { o << endl; }
	return o;
}

//////////////////////
// 
// bbStack_t class	 
// 
// @brief: a very simple wrapper for a composite vector / bitset with stack interface
// 
// @details: created 28/13/17, refactored 27/01/2025
// 
//////////////////////

template <class BitSet_t>
struct bbStack_t{

	enum print_t {STACK = 0, BITSET};

//construction / destruction
	bbStack_t			(int MAX_SIZE) : bb_(MAX_SIZE) {}
	bbStack_t			()	{}
	
	//move and copy semantics - copy and move semantics forbidden
	bbStack_t			(const bbStack_t& rhs)			= delete;
bbStack_t& operator =	(const bbStack_t& rhs) noexcept	= delete;
		
//allocation

	void reset		(int MAX_SIZE);

//setters and getters
	int  size			()							{ return stack_.size(); }

//stack interface (bits are added / removed according to the stack)
	void push			(int elem);
	int pop				();

//basic operations

	//clears bits from the bitset according to stack
	void erase_bit		();

	//synchro stack according to bitset (ordered from lsb to msb)
	void sync_stack		();		

	//synchro bitset according to stack
	void sync_bb		();																	

//boolean
	/* may occur if data is manipulated directly */
	//checks if the contents in the STACK is in BB - currently the opposite is not checked
	bool is_sync		();										
	
	bool is_empty		()			{return (pc_ ==0);}
	
//I/O
	ostream& print		(print_t t = STACK, ostream& o= cout, bool eofl = true);

/////////////////
// data members

	BitSet_t bb_;
	std::vector<int> stack_;

}; //end bbStack_t class

//////////////////////
// 
// bbCol_t class 
// 
// @brief  a very simple wrapper for a collection of bitsets of a fixed size
// @details: created 9/8/17 for MWCP upper bound computation, refactored 27/02/2025
// 
///////////////////////

template <class BitSet_t, int SIZE>
struct bbCol_t {

	using basic_type = BitSet_t;
	using type = bbCol_t<BitSet_t, SIZE>;

//contructors / destructors
	bbCol_t	(int popCount) {
		reset(popCount);		
	}

	//TODO move and copy semantics...

//allocation
	void reset(int popCount) {
		try {
			for (int i = 0; i < SIZE; i++) {
				bb_[i].reset(popCount);
			}
		}
		catch (exception& e) {
			LOG_ERROR(e.what());
			LOG_ERROR("bbCol_t::bbCol_t()");
			std::exit(-1);
		}
	}

//setters and getters
	int number_of_bitblocks	(int bitsetID)									{ return bb_[bitsetID].number_of_bitblocks(); }
	int size				(int bitsetID)									{ return bb_[bitsetID].size(); }
	int capacity			()												{ return SIZE; }

//basic operations
	BitSet& set_bit			(int bitsetID, int bit)								{return std::ref((bb_[bitsetID].set_bit(bit))); }

	/**
	* @brief sets the bit in the position pos of the bitset at bitsetID 
	*		 in the collection
	* @param bitsetID: the position of the bitset in the array
	* @param bit: the bit to set
	* @param is_first_bit: true if bit is now the first bit of the bitset
	* @returns a reference to the bitset modified 
	**/
	BitSet& set_bit			(int bitsetID, int bit, bool& is_first_bit);
	
	BitSet& erase_bit		(int bitsetID, int bit)								{ return std::ref((bb_[bitsetID].erase_bit(bit))); }
	
	/**
	* @brief clears all 1-bits in the bitset bb_[bitsetID]
	**/
	BitSet& erase_bit			(int bitsetID)									{ return std::ref((bb_[bitsetID].erase_bit())); }

//bool
	bool is_bit				(int pos, int bit)								{ return bb_[pos].is_bit(bit); }


//I/O
	ostream& print			(ostream& o = cout, bool show_pc = true, bool eofl = true)	 const;
	//TODO... others


/////////////
// data members
	static const int capacity_ = SIZE;
	std::array<BitSet_t, SIZE> bb_;
};

template <class BitSet_t, int SIZE>
inline
BitSet& bbCol_t<BitSet_t, SIZE>::set_bit(int bitsetID, int bit, bool& is_first_bit) {

	//adds bit
	bb_[bitsetID].set_bit(bit);

	//checks if the bit added is the first bit of the bitset
	is_first_bit = (bit == bb_[bitsetID].lsb());

	//returns a reference to the modified bitset
	return 	std::ref(bb_[bitsetID]);
}


template <class BitSet_t, int SIZE>
inline
ostream& bbCol_t<BitSet_t, SIZE>::print(ostream& o, bool show_pc, bool eofl)  const{
	for(auto i = 0; i < bb_.size(); ++i){
		if(!bb_[i].is_empty()){
			bb_[i].print(o,  showpc, true);
		}
	}
	if (eofl) { o << endl; }
	return o;
}

template <class BitSet_t>
inline
ostream& bbStack_t<BitSet_t>::print(print_t t, ostream& o, bool eofl){

	switch(t){
	case STACK:
		o << "[";
		for(auto i = 0; i < stack_.size(); ++i){
			o << stack_[i] << " ";
		}
		o << "]" << endl;
		break;
	case BITSET:
		bb_.print(o, true, false);		//size info and no end of line
		break;
	default:
		; //error
	}

	if (eofl) { o << endl; }
	return o;
}

template <class BitSet_t>
inline
bool bbStack_t<BitSet_t>::is_sync(){
		
	if (bb_.size() != stack_.size()) { return false; }
	
	//checks if exactly the population of bb_ is in the STACK  
	for(auto i = 0; i < stack_.size(); ++i){
		if (!bb_.is_bit(stack_[i])) {
			return false;
		}
	}	
	
	return true;
}

template <class BitSet_t>
inline
void bbStack_t<BitSet_t>::sync_bb(){

	bb_.erase_bit();
	for(auto i = 0; i < stack_.size(); i++){
		bb_.set_bit(stack_[i]);
	}
}

template <class BitSet_t>
inline
void bbStack_t<BitSet_t>::sync_stack(){

	//cleans stack
	stack_.clear();
	
	//bitscanning with nested data structure
	BitSet_t::scan sc(bb_);
	if (sc.init_scan() != -1) {
		int bit = BBObject::noBit;
		while ( (bit = bb_.next_bit()) != BBObject::noBit ) {
			stack_.emplace_back(bit);
		}
	}

}

template <class BitSet_t>
inline
void  bbStack_t<BitSet_t>::reset(int MAX_SIZE) {

	//cleans stack
	pc_ = 0;

	//allocates memory	
	try {
		/////////////////////////////
		bb_.reset(MAX_SIZE);
		stack_.clear();
		////////////////////////////
	}
	catch (exception& e) {
		LOG_ERROR(e.what());
		LOG_ERROR("bbStack_t<BitSet_t>::-reset()");
		std::exit(-1);
	}

}; //end struct


template <class BitSet_t>
inline
void bbStack_t<BitSet_t>::push(int bit) { 

	if (!bb_.is_bit(bit)) {
		bb_.set_bit(bit);
		stack_.emplace_back(bit);
	} 
}

template <class BitSet_t>
inline
int bbStack_t<BitSet_t>::pop() {
	 
	if (stack_.size() > 0) {
		 int bit = stack_.back();
		 v.pop_back();
		 bb_.erase_bit(bit);	
		 return bit;
	 }
	 else return BBObject::noBit;
}

template <class BitSet_t>
inline
void bbStack_t<BitSet_t>::erase_bit() {
	for (int i = 0; i < stack_.size(); i++) {
		bb_.erase_bit(stack_[i]);
	}	
}				

///////////////////////
//
// Stateless functions for BitSets
// (namespace bbalg)
//
//////////////////////

namespace bbalg {
	inline 
	std::vector<int> to_vector(const BitSet& bbn) {

		vector<int> res;

		int v = EMPTY_ELEM;
		while (1) {
			if ((v = bbn.next_bit(v)) == EMPTY_ELEM)
				break;
			res.push_back(v);
		}
		return res;
	}

	/**
	* @brief generates a random BITBOARD with density p of 1-bits
	**/
	inline 
	BITBOARD gen_random_bitblock (double p) {
		
		BITBOARD bb = 0;

		for (auto i = 0; i < WORD_SIZE; i++) {
			if (com::rand::uniform_dist(p)) {
				bb |= Tables::mask[i];
			}
		}
		return bb;
	}

	template<class BitSet_t, class array_t>
	inline 
	int first_k_bits(int k, BitSet_t& bb, array_t& lv) {
		///////////////////
		// Computes the first 3 nodes of color set 'col' in 'lv'
		//
		// RETURNS the number of bits read [0..k]
		//
		// TODO-change to BITSCAN and extend for k nodes

		bb.init_scan(bbo::NON_DESTRUCTIVE);
		int POINTER = 0;
		while (true) {
			lv[POINTER] = bb.next_bit();
			if (lv[POINTER] == EMPTY_ELEM || ++POINTER == k) break;
		}
		return POINTER;
	}

	//inline BITBOARD get_first_k_bits(BITBOARD bb,  BYTE k /*1-64*/){
	////////////////////////////////////
	//// Returns BITBOARD of first k bits to 1 or 0 if k-bits to 1 could not be found
	//
	//	BITBOARD bb_aux, res=0;
	//	int cont=0;
	//
	//	//control
	//	if(k<1 || k>64) return 0;
	//
	//	while(bb){
	//		bb_aux= bb & (-bb);	/*00..010..0*/
	//		res |= bb_aux;
	//		if(++cont==k) break;
	//	
	//	bb ^= bb_aux;
	//	}
	//
	//return (cont==k)? res: 0;
	//}
}



#endif

