/*  
 * bbalg.h file from the BITSCAN library, a C++ library for bit set
 * optimization. BITSCAN has been used to implement BBMC, a very
 * succesful bit-parallel algorithm for exact maximum clique. 
 * (see license file for references)
 *
 * Copyright (C)
 * Author: Pablo San Segundo
 * Intelligent Control Research Group (CSIC-UPM) 
 *
 * Permission to use, modify and distribute this software is
 * granted provided that this copyright notice appears in all 
 * copies, in source code or in binaries. For precise terms 
 * see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any 
 * kind, express or implied, and with no claim as to its
 * suitability for any purpose.
 *
 */

#ifndef  __BBALG_H__
#define  __BBALG_H__

#include "utils/common.h"
#include "utils/logger.h"
#include "bitboardn.h"

using namespace std;

typedef BBObject  bbo;

//a basic ADT for managing popcounts
template <class bitstring_t>
struct bb_t{
//////////////////////
//a simple wrapper for any type of bitstring with popcount 
//last_update@12/08/19
///////////////////////
	int pc;																			//population count
	bitstring_t bb; 
////////////////////
//interface
	bb_t(int MAX_SIZE):pc(0){bb.init(MAX_SIZE);}									//all bits FALSE
	bb_t():pc(EMPTY_ELEM){}															//to create a collection
	void init(int MAX_SIZE){bb.init(MAX_SIZE); pc =0;}
	int  get_pc() const { return pc; }
		
	//bit twiddling
	void set_bit(int v) {bb.set_bit(v); pc++;}										//push-bitstring interface
	void erase_bit(bool lazy=false){ if(!lazy) {bb.erase_bit();} pc =0;}			//clears all bits
	int  erase_bit(int elem){bb.erase_bit(elem); pc--; return pc;}

	//useful func
	int sync_pc() { pc = bb.popcn64(); return pc; }
	int pop()	  { if (pc > 0) { int v = bb.msbn64(); bb.erase_bit(v); pc--; return v; } else return EMPTY_ELEM; }
	int front()   {	if (pc > 0) { return bb.lsbn64();	} else return EMPTY_ELEM;	}

	//bool
	bool is_empty() const {return (pc ==0);}
	bool test_pc()  const {return pc == bb.popcn64();}	

	//operators
	friend bool operator ==(const bb_t& lhs, const bb_t& rhs) { return (lhs.pc == rhs.pc) && (lhs.bb == rhs.bb);}
	friend bool operator !=(const bb_t& lhs, const bb_t& rhs) { return !(lhs == rhs); }

	//I/O
	ostream& print(ostream& o = cout) const { bb.print(o); o << "[" << pc << "]"; return o; }
};


template <class bitstring_t>
struct sbb_t{
////////////////////
// stack-state with bitstrings
//
// date_of_creation: 28/3/17

	enum type_t {STACK=0, BITSTRING};

	int* stack;
	int size;
	bitstring_t bb;

	sbb_t(int MAX_SIZE){init(MAX_SIZE);}	
	sbb_t():size(-1), stack(NULL){}		
	~sbb_t(){if(stack) delete stack; stack=NULL;}

	void init(int MAX_SIZE){bb.init(MAX_SIZE); size=0; stack=new int[MAX_SIZE]; for(int i=0; i<MAX_SIZE; i++) stack[i]=0;}
	void push(int elem) {if(!bb.is_bit(elem)){bb.set_bit(elem); stack[size++]=elem;}}
	int pop(){if(size>0) {int elem=stack[--size]; bb.erase_bit(elem); return elem;} else return -1;}
	
	void erase(){for(int i=0; i<size; i++) bb.erase_bit(stack[i]); size=0;}				//clears all bit O(SIZE) operation
	void update_stack();																//synchro according to bitset
	void update_bb();																	//synchro according to stack
	bool is_synchro();																	/* may occur if data is manipulated directly */
	bool is_empty(){return (size==0);}
	int  get_size(){return size;}

	ostream& print(type_t t=STACK, ostream& o= cout);
};

template <class bitstring_t>
struct bba_t{
/////////////////
//simple ADT for an array of bitstrings (without size information)
	int capacity;													/* remains fixed */
	bitstring_t* pbb;
	
	bba_t():capacity(0), pbb(NULL){}
	~bba_t(){clear();}
	void init(int capacity, int pc);
	void clear()						{if(pbb){ delete [] pbb; pbb=NULL; capacity=0;}}

	void set_bit(int pos, int bit)							{pbb[pos].set_bit(bit);}
	void set_bit(int pos, int bit, bool& is_first_bit)		{pbb[pos].set_bit(bit);  is_first_bit=(bit==pbb[pos].lsbn64());}
	void erase_bit(int pos, int bit)	{pbb[pos].erase_bit(bit);}
	void erase_bit(int pos)				{pbb[pos].erase_bit();}
	void erase_bit();					
	int popcn(int pos)					{return pbb[pos].popcn64();}
	bool is_bit(int pos, int bit)		{return pbb[pos].is_bit(bit);}

	int get_number_of_bitblocks()		{return pbb[0].number_of_bitblocks();}

	//I/O
	ostream& print(ostream& o=cout) const;		
};
template <class bitstring_t>
void bba_t<bitstring_t>::erase_bit(){
	for(int pos=0; pos<capacity; pos++){
		pbb[pos].erase_bit();
	}
}

template <class bitstring_t>
inline
ostream& bba_t<bitstring_t>::print(ostream& o) const{
	for(int i=0; i<capacity; i++){
		if(!pbb[i].is_empty()){
			pbb[i].print(o);
			o<<endl;
		}
	}
	return o;
}

template <class bitstring_t>
inline
void bba_t<bitstring_t>::init(int capacity, int pc){
	clear();
	try{
		this->capacity=capacity;
		pbb=new bitstring_t[capacity];
		for(int i=0; i<capacity; i++){
			pbb[i].init(pc);
		}
	}catch(exception& e){
		LOG_ERROR("bba_t<bitstring_t>::-init()");
		e.what();
	}
}


template <class bitstring_t>
inline
ostream& sbb_t<bitstring_t>::print(type_t t, ostream& o){
	switch(t){
	case STACK:
		o<<"[";
		for(int i=0; i<size; i++){
			o<<stack[i]<<" ";
		}
		o<<"]"<<endl;
		break;
	case BITSTRING:
		bb.print(o);
		break;
	default:
		; //error
	}

	return o;
}

template <class bitstring_t>
inline
bool sbb_t<bitstring_t>::is_synchro(){
//checks if the contents is the same in STACK and BB
	
	int pc=bb.popcn64();
	if(pc!=size) return false;

	for(int i=0; i<size; i++){
		if(!bb.is_bit(stack[i])) return false;
	}
	
	return true;
}

template <class bitstring_t>
inline
void sbb_t<bitstring_t>::update_bb(){
	bb.erase_bit();
	for(int i=0; i<size; i++){
		bb.set_bit(stack[i]);
	}
}

template <class bitstring_t>
inline
void sbb_t<bitstring_t>::update_stack(){
	size=0;
	bb.init_scan(BBObject::NON_DESTRUCTIVE);
	while(true){
		int v=bb.next_bit();
		if(v==EMPTY_ELEM) break;

		stack[size++]=v;
	}
}

inline std::vector<int> to_vector(const BitBoardN& bbn){
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

template<class bitstring_t, class array_t>
inline
int first_k_bits (int k, bitstring_t &bb, array_t &lv){
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

