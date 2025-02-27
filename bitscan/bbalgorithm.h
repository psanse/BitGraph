 /**
  * @file bbalgorithm.h
  * @brief header for the algorithms and data structures for bitstrings and bitblocks
  * @author pss
  * @details created 2017, last_update 27/02/2025
  * 
  * TODO refactor and test (15/02/2025)
  **/

#ifndef  _BBALG_H_
#define  _BBALG_H_

#include "utils/common.h"
#include "utils/logger.h"
#include "bitscan/bbset.h"

using namespace std;

//aliases
using bbo = BBObject;

//////////////////////
// 
// bbSize_t class
// 
// A simple wrapper for any type of bitset of the BBObject hierarchy with CACHED size 
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
// (created 28/13/17 - stack state semantics with bitsets)
// Not very clear the utility of such class - possibly change to composite with std::vector
// 
//////////////////////

template <class BitSet_t>
struct bbStack_t{

	enum print_t {STACK = 0, BITSTRING};

//construction / destruction
	bbStack_t			(int MAX_SIZE);
	bbStack_t			(): pc_(0), stack_(nullptr)	{}
	
	//move and copy semantics - copy and move semantics forbidden
	bbStack_t			(const bbStack_t& rhs)			= delete;
bbStack_t& operator =	(const bbStack_t& rhs) noexcept	= delete;

	~bbStack_t			()							{ delete stack_;}

//allocation

	void reset		(int MAX_SIZE);

//setters and getters
	int  size			()							{ return pc_; }

//stack interface (bits are taken according to the stack)
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
	BITBOARD pc_;		//number of bits in stack
	int* stack_;

}; //end struct bbStack_t


//////////////////////
// 
// bba_t class... (created 9/8/17 for MWCP upper bound computation)
// 
// Wrappers with raw pointers - CHECK if they are still needed (27/02/2025)
// 
// TODO - remove/refactor
// 
///////////////////////

template <class BitSet_t>
struct bba_t{
/////////////////
//simple ADT for an array of bitstrings (without size information)
	int capacity;													/* remains fixed */
	BitSet_t* pbb;
	
	bba_t():capacity(0), pbb(NULL){}
	~bba_t(){clear();}
	void init(int capacity, int pc);
	void clear()						{if(pbb){ delete [] pbb; pbb=NULL; capacity=0;}}

	void set_bit(int pos, int bit)							{pbb[pos].set_bit(bit);}
	void set_bit(int pos, int bit, bool& is_first_bit)		{pbb[pos].set_bit(bit);  is_first_bit=(bit==pbb[pos].lsbn64());}
	void erase_bit(int pos, int bit)	{pbb[pos].erase_bit(bit);}
	void erase_bit(int pos)				{pbb[pos].erase_bit();}
	void erase_bit();					
	int popcn(int pos)					{return pbb[pos].size();}
	bool is_bit(int pos, int bit)		{return pbb[pos].is_bit(bit);}

	int get_number_of_bitblocks()		{return pbb[0].number_of_bitblocks();}

	//I/O
	ostream& print(ostream& o=cout) const;		
};

template <class BitSet_t>
void bba_t<BitSet_t>::erase_bit(){
	for(int pos = 0; pos<capacity; pos++){
		pbb[pos].erase_bit();
	}
}

template <class BitSet_t>
inline
ostream& bba_t<BitSet_t>::print(ostream& o) const{
	for(int i=0; i<capacity; i++){
		if(!pbb[i].is_empty()){
			pbb[i].print(o);
			o<<endl;
		}
	}
	return o;
}

template <class BitSet_t>
inline
void bba_t<BitSet_t>::init(int capacity, int pc){
	clear();
	try{
		this->capacity=capacity;
		pbb=new BitSet_t[capacity];
		for(int i=0; i<capacity; i++){
			pbb[i].init(pc);
		}
	}catch(exception& e){
		LOG_ERROR("bba_t<BitSet_t>::-init()");
		e.what();
	}
}


template <class BitSet_t>
inline
ostream& bbStack_t<BitSet_t>::print(print_t t, ostream& o, bool eofl){

	switch(t){
	case STACK:
		o << "[";
		for(auto i = 0; i < pc_; ++i){
			o << stack_[i] << " ";
		}
		o << "]" << endl;
		break;
	case BITSTRING:
		bb_.print(o, true, false);		//eofl= true by default
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
		
	int pcBitSet = bb_.size();
	if (pcBitSet != pc_) { return false; }
	
	//checks if exactly the population of bb_ is in the STACK  
	for(auto i = 0; i < pc_; ++i){
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
	for(auto i = 0; i < pc_; i++){
		bb_.set_bit(stack_[i]);
	}
}

template <class BitSet_t>
inline
void bbStack_t<BitSet_t>::sync_stack(){

	//cleans stack
	pc_ = 0;
	
	//bitscanning with nested data structure
	BitSet_t::scan sc(bb_);
	if (sc.init_scan() != -1) {
		int bit = BBObject::noBit;
		while ( (bit = bb_.next_bit()) != BBObject::noBit ) {
			stack_[pc_++] = bit;
		}
	}

}

inline std::vector<int> to_vector(const BitSet& bbn){
	vector<int> res;

	int v=EMPTY_ELEM;
	while(1){
		if((v=bbn.next_bit(v))==EMPTY_ELEM)
			break;
		res.push_back(v);
	}
return res;
}

inline BITBOARD gen_random_bitboard(double p){
//generates a random BITBOARD with density p of 1-bits
	BITBOARD bb=0;
	for(int i=0; i<WORD_SIZE; i++){
		if(com::rand::uniform_dist(p)){
			bb|=Tables::mask[i];
		}
	}
return bb;
}


template<class BitSet_t, class array_t>
inline
int first_k_bits (int k, BitSet_t &bb, array_t &lv){
///////////////////
// Computes the first 3 nodes of color set 'col' in 'lv'
//
// RETURNS the number of bits read [0..k]
//
// TODO-change to BITSCAN and extend for k nodes

	bb.init_scan(bbo::NON_DESTRUCTIVE);
	int POINTER=0;
	while(true){
		lv[POINTER]=bb.next_bit();
		if(lv[POINTER]==EMPTY_ELEM || ++POINTER==k) break;
	}
	return POINTER;
}

template <class BitSet_t>
bbStack_t<BitSet_t>::bbStack_t(int MAX_SIZE) :
	pc_(0),
	stack_(nullptr) 
{
	//allocates memory	
	try {
		/////////////////////////////
		bb_.reset(MAX_SIZE);
		stack_ = new int[MAX_SIZE];
		////////////////////////////
	}
	catch (exception& e) {
		LOG_ERROR(e.what());
		LOG_ERROR("bbStack_t<BitSet_t>::-reset()");
		std::exit(-1);
	}

	//initialize stack  
	//(I believe this is not necessary, since the policy is to initialize all basic data types to 0)
	for (int i = 0; i < MAX_SIZE; i++) {
		stack_[i] = 0;
	}
}

template <class BitSet_t>
void  bbStack_t<BitSet_t>::reset(int MAX_SIZE) {

	//cleans stack
	pc_ = 0;

	//allocates memory	
	try {
		/////////////////////////////
		bb_.reset(MAX_SIZE);
		delete stack_;
		stack_ = new int[MAX_SIZE];
		////////////////////////////
	}
	catch (exception& e) {
		LOG_ERROR(e.what());
		LOG_ERROR("bbStack_t<BitSet_t>::-reset()");
		std::exit(-1);
	}

	//initialize stack  
	//(I believe this is not necessary, since the policy is to initialize all basic data types to 0)
	for (int i = 0; i < MAX_SIZE; i++) {
		stack_[i] = 0;
	}

}; //end struct



template <class BitSet_t>
void bbStack_t<BitSet_t>::push(int bit) { 

	if (!bb_.is_bit(bit)) {
		bb_.set_bit(bit);
		stack_[pc_++] = bit;
	} 
}

template <class BitSet_t>
int bbStack_t<BitSet_t>::pop() {
	 
	if (pc_ > 0) {
		 int bit = stack_[--pc_]; 
		 bb_.erase_bit(bit);
		 return bit;
	 }
	 else return BBObject::noBit;
}

template <class BitSet_t>
void bbStack_t<BitSet_t>::erase_bit() {
	for (int i = 0; i < pc_; i++) {
		bb_.erase_bit(stack_[i]);
	}
	pc_ = 0;
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



#endif

