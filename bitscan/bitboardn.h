/**  
 * @file bitboardn.h file 
 * @brief header file of the BitBoardN class from the BITSCAN library.
 *		  Manages bitstrings of any size as an array of bitblocks (64-bit numbers)
 * @author pss
 * @created 2014
 * @last_update 01/02/2025
 * 
 * TODO - refactoring and testing (31/01/2025)
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
 **/

#ifndef __BITBOARDN__H_
#define __BITBOARDN__H_

#include "bbobject.h"
#include "bitboard.h"	
#include <vector>	
#include "utils/common.h"			//stack

//uncomment NDEBUG in config.h to enable assertions
#include <cassert>

//alias
using vint = std::vector<int>;

/////////////////////////////////
//
// BitBoardN class 
//
// Manages bit strings greater than WORD_SIZE 
// @details Does not use HW dependent instructions (intrinsics), nor does it cache information for very fast bitscanning
//
///////////////////////////////////
class BitBoardN:public BBObject{
public:	

/////////////////////////////
// independent operators / services (no allocation or copies)

	friend bool operator==			(const BitBoardN& lhs, const BitBoardN& rhs);
	friend bool operator!=			(const BitBoardN& lhs, const BitBoardN& rhs);

	friend BitBoardN&  AND			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);							
	friend BitBoardN&  AND			(int first_block, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);
	friend BitBoardN&  AND			(int first_block, int last_block, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);
	friend BitBoardN&  AND			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res, int last_vertex, bool lazy);			//up to and excluding last_vertex
	friend int*	       AND			(const BitBoardN& lhs, const BitBoardN& rhs, int last_vertex, int res[], int& size);				//returns the operation as a set of vertices	

	friend BitBoardN&  OR			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);
	friend BitBoardN&  OR			(int from, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);	
	friend BitBoardN&  OR			(int v, bool from /* to */, const BitBoardN& lhs, const BitBoardN& rhs, BitBoardN& res);

	friend BitBoardN&  ERASE		(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);										//removes rhs from lhs
	friend int FIRST_SHARED			(const BitBoardN& lhs, const BitBoardN& rhs);
	friend int FIRST_SHARED			(int first_block, const BitBoardN& lhs, const BitBoardN& rhs);	//first elem in common

////////////
//construction / destruction 

	 BitBoardN						(): m_nBB(EMPTY_ELEM),m_aBB(NULL)		{};	
	 BitBoardN						(const vint& v, int popsize);
explicit  BitBoardN					(int popsize /*1 based*/, bool reset=true);	
explicit  BitBoardN					(const vint& v);
	
	 	 
	 //TODO - move and copy semantics...
	 BitBoardN						(const BitBoardN& bbN);
	 BitBoardN& operator =			(const BitBoardN&);

virtual	~BitBoardN					();

////////////
//memory allocation 
	void init						(int popsize, bool reset = true);										
	void init						(int popsize, const vint& );
	
/////////////////////
//setters and getters (will not allocate memory)
	
	BITBOARD* get_bitstring			()									{ return m_aBB; }
	const BITBOARD* get_bitstring	()			const					{ return m_aBB; }
	int number_of_bitblocks			()			const					{ return m_nBB; }
const BITBOARD get_bitblock			(int block) const					{ return m_aBB[block]; }
	BITBOARD& get_bitblock			(int block)							{ return m_aBB[block]; }

//////////////////////////////
// Bitscanning

	//find least/most signinficant bit
inline virtual int msbn64			()	const;		//lookup
inline virtual int lsbn64			()	const; 		//de Bruijn	/ lookup								

	//for looping (does not use state info)	
inline int next_bit					(int nBit)	const;					//de Bruijn
inline int next_bit_if_del			(int nBit)	const;					//de Bruijn
inline int previous_bit				(int nbit)	const;					//lookup 
	
/////////////////
// Popcount

virtual	inline int popcn64			()						const;		//lookup 
virtual	inline int popcn64			(int nBit/* 0 based*/)	const;

/////////////////////
//Setting / Deletion 
			
		/**
		* @brief sets bit in position idx to 1 in the bitstring
		* @param  idx: index of the bit to set (nBit >= 0)
		**/
		
		template<bool EraseAll = false>
		BitBoardN&  set_bit			(int nBit);

		/**
		* @brief sets the bits in the closed range [low, high] to 1 in the bitstring
		* @params low, high: range to be set (>= 0)
		* @date 22/9/14
		* @last_update 01/02/25
		**/
		template<bool EraseAll = false>
inline  BitBoardN&	 set_bit		(int low, int high);											

		/**
		* @brief sets all bits to 1
		**/
inline  BitBoardN&  set_bit			();
	 	
		/**
		* @brief Adds the bits from the bitstring bb_add in the population
		*		 range of the bitstring (bitblocks are copied).
		* 
		*		 I. The bitblock size of bb_add must be at least as large as the bitstring.
		*		
		* @details  Equivalent to OR operation / set union
		* @returns reference to the modified bitstring
		* 
		**/
inline	BitBoardN& set_bit			(const BitBoardN& bb_add);
		
		/**
		* @brief Adds the bits from the bitstring bb_add in the range [0, lastBit] 
		* @param lastBit : the last bit in the range to be copied
		* @returns reference to the modified bitstring
		**/
inline BitBoardN& set_bit			(int lastBit, const BitBoardN& bb_add);
	 
	   /**
	   * @brief Adds elements from a vector of non-negative integers lv as 1-bit
	   *	    up to the maximum capacity of the bitstring. Values greater than the
	   *		maximum population size are ignored.
	   * 
	   * @param lv: vector of non-negative integers
	   * @param EraseAll <template>: if true, deletes the rest of bits in the bitstring
	   * @returns reference to the modified bitstring
	   * @details negative elements will cause an assertion if NDEBUG is not defined, 
	   *		  else the behaviour is undefined.
	   **/
		template<bool EraseAll = false>	
	   BitBoardN& set_bit			(const vint& lv);

		/**
		* @brief sets bit number nBit to 0 in the bitstring
		* @param  nBit: index of the bit to set (>=0)
		* @returns reference to the modified bitstring
		**/
inline	BitBoardN& erase_bit		(int nBit);

		/**
		* @brief sets the bits in the closed range [low, high] to 0 in the bitstring
		* @params low, high: range to be set (>= 0)	
		* @created 22/9/14
		* @last_update 01/02/25
		**/
inline BitBoardN& erase_bit			(int low, int high);

		/**
		* @brief sets all bits to 0
		* @returns reference to the modified bitstring
		**/
inline BitBoardN& erase_bit			();

		/**
		* @brief Removes the bits from the bitstring bb_del inside the population
		*		 range.
		*
		*		 I. bb_del must have a maximum population
		*			greater or equal than the bitstring.
		*
		* @details  Equivalent to a set minus operation
		* @returns reference to the modified bitstring
		*
		**/
inline	BitBoardN& erase_bit		(const BitBoardN& bb_del);	

		/**
		* @brief Removes the 1-bits from both input bitstrings (their union) 
		*		inside the population range. The sizes of both input bitstrings
		*		must be greater than the destination bitstring (*this)
		* @param bb_del_lhs, bb_del_rhs : bitstrings to be removed
		* @returns reference to the modified bitstring
		* @created: 30/7/2017  for the MWCP
		* @last_update: 02/02/2025
		**/
inline	BitBoardN& erase_bit		(const BitBoardN& bb_del_lhs, const BitBoardN& bb_del_rhs);
						
/////////////////////
//BitBlock operations 

	/**
	* @brief Copies the 1-bits from the bitstring bb_add in the closed range [FirstBlock, LastBlock]
	*		 If LastBlock == -1, the range is [FirstBlock, m_nBB]
	* 
	*		 0 <= FirstBlock <= LastBLock < the number of bitblocks in the bitstring
	* 
	* @param bb_add: input bitstring to be copied
	* @param FirstBlock: the first bitblock to be modified
	* @param LastBLock: the last bitblock to be modified
	* @returns reference to the modified bitstring
	**/
	BitBoardN& set_block			(int FirstBlock, int LastBlock, const BitBoardN& bb_add);
	
	/**
	* @brief Deletes the 1-bits from the bitstring bb_del in the closed range [FirstBlock, LastBlock]
	*		 If LastBlock == -1, the range is the whole bitstring.
	*
	*		 0 <= FirstBlock <= LastBLock < the number of bitblocks in the bitstring
	*
	* @param bb_del: input bitstring whose 1-bits are to be removed	
	* @param FirstBlock: the first bitblock to be modified
	* @param LastBLock: the last bitblock to be modified
	* @returns reference to the modified bitstring
	**/
	inline	BitBoardN& erase_block	(int FirstBlock, int LastBlock, const BitBoardN& bb_del);

	/**
	* @brief Removes the 1-bits from both input bitstrings (their union)
	*		in the closed range of bitblocks [FirstBlock, LastBlock]
	*		If LastBlock == -1, the range is [FirstBlock, m_nBB]
	* 
	* @param FirstBlock: the first bitblock to be modified
	* @param bb_del_lhs, bb_del_rhs : bitstrings whose 1-bits are to be removed
	* @returns reference to the modified bitstring
	* @date: 02/02/2025 during a refactorization of BITSCAN
	*
	**/
	inline	BitBoardN& erase_block (int FirstBlock, int LastBlock, const BitBoardN& bb_del_lhs, const BitBoardN& bb_del_rhs);

////////////////////////
//operators
				
	BitBoardN& operator &=			(const BitBoardN& );													//bitset_intersection
	BitBoardN& operator |=			(const BitBoardN& );													//bitset_union
	BitBoardN& operator ^=			(const BitBoardN& );													//bitet symmetric_difference

	BitBoardN&  AND_EQ				(int first_block, const BitBoardN& rhs );								//AND: range
	BitBoardN&  OR_EQ				(int first_block, const BitBoardN& rhs );								//OR:  range
		
	BitBoardN& flip					();
inline	int	single_disjoint			(const BitBoardN& rhs, int& vertex)				const;					//non-disjoint by single element
inline	int	single_disjoint			(int first_block, int last_block, 
										const BitBoardN& rhs, int& vertex)			const;
inline	int	single_joint			(const BitBoardN& rhs, int& vertex)				const;					//non-joint by single element (this\rhs)
inline int double_joint				(const BitBoardN& rhs, int& v, int& w)			const;					//non_joint by one or two elements (this\rhs)
inline int  is_singleton			(int vlhs, int vrhs)							const;					//single vertex inside range
inline int  find_singleton			(int low, int high, int& singleton)				const;

inline int first_found				(const BitBoardN& rhs)							const;					//first vertex in common with rhs  (7/17)

/////////////////////////////
//Boolean functions

	inline bool is_bit				(int nbit)										const;
inline virtual bool is_empty		()												const;	
inline virtual bool is_empty		(int nBBL, int nBBH)							const;						
	inline bool is_singleton		()												const;					//only one element- /*TODO (15/3/17) -extend to a range of bits*/
	inline bool is_disjoint			(const BitBoardN& rhs)							const;
	inline bool is_disjoint			(int first_block, int last_block,
										const BitBoardN& rhs			)			const;
	inline bool is_disjoint			(const BitBoardN& a, const  BitBoardN& b)		const;					//no bit in common with both a and b (not available in sparse bitsets)

/////////////////////
// I/O 
	/**
	* @brief streams bb and its popcount to the output stream o
	* @details format example [...000111 (3)]
	* @param o: output stream
	* @param show_pc: if true, shows popcount
	* @returns output stream
	**/
	std::ostream& print				(std::ostream& o= std::cout, bool show_pc = true, bool endl = true) const override;
	
	/**
	* @brief converts bb and its popcount to a readable string
	* @details format example [...000111 (3)]
	* @returns string
	**/
	std::string to_string			();
	
///////////////////////
//conversions

	/**
	* @brief Converts the bitstring to a std::vector of non-negative integers.
	*		 The size of the vector is the number of bits in the bitstring. 
	* @param lv: output vector 
	**/
	void to_vector					(vint& lv)							const;

	/**
	* @brief Casts the bitstring to a vector of non-negative integers
	* @details calls to_vector
	**/
	operator						vint()								const;

	
	/**
	* @brief Converts the bitstring to a stack object s 
	* @details no allocation! stack MUST BE of adequate size to hold all 1-bits
	* @param s: output stack 
	**/
	void to_stack					(com::stack_t<int>& s)				const;						
	

	/**
	* @brief Converts the bitstring to a C array
	* @param lv: output C-array as pointer
	* @param size: output size of the array
	* @param rev: if true, the array is filled in reverse order
	* @returns pointer to the array, size of the array
	**/
virtual	int* to_C_array				(int* lv, std::size_t& size, bool rev = false);

////////////////////////
//data members

protected:
	BITBOARD* m_aBB;				//array of bitblocks - not using std::vector because of memory allignment
	int m_nBB;						//number of bitblocks (1-based)

}; //end BitBoardN class

//////////////////////////////////
// Inline functions implementations - necessary in header file

inline 
int BitBoardN::first_found	(const BitBoardN& rhs) const{
//returns first vertex in common with rhs that (7/17)	
//returns -1 if they are disjoint

	BITBOARD bb = 0;
	for(auto i = 0; i < this -> m_nBB; ++i){
		bb = this->m_aBB[i] & rhs.m_aBB[i];
		if(bb = (this->m_aBB[i] & rhs.m_aBB[i])){
			return bblock::lsb64_intrinsic(bb) + WMUL(i);
		}
	}
	return EMPTY_ELEM;
}

inline int BitBoardN::msbn64() const{
///////////////////////
// Look up table implementation (best found so far)

	register union u {
		U16 c[4];
		BITBOARD b;
	} val;
	
	//reverse loop (most significant bit block early exit)
	for(auto i = m_nBB - 1; i >= 0; i--){
		val.b = m_aBB[i];
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]] + WMUL(i));
			if(val.c[2]) return (Tables::msba[2][val.c[2]] + WMUL(i));
			if(val.c[1]) return (Tables::msba[1][val.c[1]] + WMUL(i));
			if(val.c[0]) return (Tables::msba[0][val.c[0]] + WMUL(i));
		}
	}

	return EMPTY_ELEM;		//should not reach here
}


inline int BitBoardN::next_bit(int nBit/* 0 based*/) const{
////////////////////////////
//
// Returns next bit from nBit in the bitstring (to be used in a bitscan loop)
//
// NOTES: if nBit is FIRST_BITSCAN returns lsb

	int index, npos;

	if(nBit==EMPTY_ELEM)
				return lsbn64();
	else{
		index=WDIV(nBit);

		//looks in same BB as nBit
		npos=bblock::lsb64_de_Bruijn(Tables::mask_left[WMOD(nBit) /*-WORD_SIZE*index*/] & m_aBB[index] );
		if(npos>=0)
			return (WMUL(index)/*WORD_SIZE*index*/ + npos);

		//looks in remaining BBs
		for(int i=index+1; i<m_nBB; i++){
			if(m_aBB[i])
				return(bblock::lsb64_de_Bruijn(m_aBB[i])+WMUL(i)/*WORD_SIZE*i*/ );
		}
	}
	
return -1;	
}

inline int BitBoardN::next_bit_if_del(int nBit/* 0 based*/) const{
////////////////////////////
// Returns next bit assuming, when used in a loop, that the last bit
// scanned is deleted prior to the call
		
	if(nBit==EMPTY_ELEM)
		return lsbn64();
	else{
		for(int i = WDIV(nBit); i<m_nBB; i++){
			if(m_aBB[i]){
				return(bblock::lsb64_de_Bruijn(m_aBB[i])+WMUL(i) );
			}
		}
	}
	
return -1;	
}

inline int BitBoardN::previous_bit(int nBit/* 0 bsed*/) const{
////////////////////////////
// Gets the previous bit to nBit. If nBits is -10 is a MSB
//
// COMMENTS
// 1-There is no control of EMPTY_ELEM

	if(nBit==EMPTY_ELEM) 
			return msbn64();
	

	int index=WDIV(nBit);
	int npos;

	union u {
		U16 c[4];
		BITBOARD b;
	};
	u val;
		
	
	//BitBoard pos
	npos=bblock::msb64_lup( Tables::mask_right[WMOD(nBit) /*nBit-WMUL(index)*/] & m_aBB[index] );
	if(npos!=EMPTY_ELEM)
			return (WMUL(index) + npos);

	for(int i=index-1; i>=0; i--){
		val.b=m_aBB[i];
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]]+ WMUL(i));
			if(val.c[2]) return (Tables::msba[2][val.c[2]]+ WMUL(i));
			if(val.c[1]) return (Tables::msba[1][val.c[1]]+ WMUL(i));
			if(val.c[0]) return (Tables::msba[0][val.c[0]]+ WMUL(i));
		}
	}

return -1;		//should not reach here
}

inline bool BitBoardN::is_bit (int nbit/*0 based*/) const{
//////////////////////////////
// RETURNS: TRUE if the bit is 1 in the position nbit, FALSE if opposite case or ERROR

	return (m_aBB[WDIV(nbit)] & Tables::mask[WMOD(nbit)]);

}
 
inline bool BitBoardN::is_empty() const
{
	for (auto i = 0; i < m_nBB; ++i) {
		if (m_aBB[i]) {
			return false;
		}
	}

	return true;	
}

inline bool BitBoardN::is_empty	(int nBBL, int nBBH) const
{
	for (auto i = nBBL; i <= nBBH; ++i) {
		if (m_aBB[i]) {
			return false;
		}
	}

	return true;	
}

inline 
bool BitBoardN::is_disjoint	(const BitBoardN& rhs) const
{
	for (auto i = 0; i < m_nBB; ++i) {
		if (m_aBB[i] & rhs.m_aBB[i]) {
			return false;
		}
	}
	return true;
}

inline 
bool BitBoardN::is_disjoint	(const BitBoardN& lhs,  const BitBoardN& rhs)	const
{
	for(auto i = 0; i < m_nBB; ++i)
		if(m_aBB[i] & lhs.m_aBB[i] & rhs.m_aBB[i]) return false;
return true;
}

inline bool BitBoardN::is_disjoint (int first_block, int last_block,const BitBoardN& rhs)	const{
///////////////////
// no checking of block indexes
	for (auto i = first_block; i <= last_block; ++i) {
		if (m_aBB[i] & rhs.m_aBB[i]) {
			return false;
		}
	}
	return true;
}

inline
BitBoardN& BitBoardN::set_bit (int high, const BitBoardN& bb_add){

	int bbh = WDIV(high);	

	for(auto i = 0; i <= bbh; ++i){
		m_aBB[i] = bb_add.m_aBB[i];
	}

	//trim last bit block up to, and exluding, high
	m_aBB[bbh] &= bblock::MASK_1_RIGHT(high - WMUL(bbh));			//also m_aBB[bbh] &= bblock::MASK_1(0, high - WMUL(bbh)); 
	
	return *this;
}

inline
int  BitBoardN::is_singleton (int low, int high) const{
/////////////////////////////
// Determines if there is only one vertex inside the range (both ends included)
//
// RETURNS 0 if range is empty, 1 if singleton, -1 if more than one
	int nbbl=WDIV(low), nbbh=WDIV(high);
	int pc=0;
	
	//both ends
	if(nbbl==nbbh){
		BITBOARD bbl= m_aBB[nbbl] & bblock::MASK_1(WMOD(low),WMOD(high));
		pc = bblock::popc64(bbl);
	}else{
		BITBOARD bbl= m_aBB[nbbl] & bblock::MASK_1(WMOD(low),WORD_SIZE-1);
		pc = bblock::popc64(bbl);
		if(pc>1){
			return -1;
		}
		for(int i=nbbl+1; i<nbbh; i++){
			pc+=bblock::popc64(m_aBB[i]);
			if(pc>1){
				return -1;
			}
		}
		BITBOARD bbh= m_aBB[nbbh] & bblock::MASK_1(0,WMOD(high));
		pc+=bblock::popc64(bbh);
	}

	if(pc==0) return 0;
	else if(pc==1) return 1;
	return -1;					//more than one vertex
}

inline
int  BitBoardN::find_singleton (int low, int high, int& singleton) const{
/////////////////////////////
// Determines if there is only one vertex inside the range (both ends included)
// and returns the singleton (or EMPTY_ELEM)
//
// RETURNS 0 if range is empty, 1 if singleton (including singleton vertex), -1 
//			if more than one

	int nbbl=WDIV(low), nbbh=WDIV(high), posl=WMOD(low), posh=WMOD(high),pc=0;
	bool first_vertex_flag=true;
	singleton=EMPTY_ELEM;
	
	//both ends
	if(nbbl==nbbh){
		BITBOARD bbl= m_aBB[nbbl] & bblock::MASK_1(posl,posh);
		pc=bblock::popc64(bbl);
		if(pc==1){
			singleton=bblock::lsb64_intrinsic(bbl)+WMUL(nbbl);
			return 1;
		}
	}else{
		BITBOARD bbl= m_aBB[nbbl] & bblock::MASK_1(posl,WORD_SIZE-1);
		pc=bblock::popc64(bbl);
		if(pc>1){
			return -1;
		}else if(pc==1){
			first_vertex_flag=false;
			singleton=bblock::lsb64_intrinsic(bbl)+WMUL(nbbl);
		}
		for(int i=nbbl+1; i<nbbh; i++){
			pc+=bblock::popc64(m_aBB[i]);
			if(pc>1){
				return -1;
			}else if(first_vertex_flag && (pc==1)){
				singleton=bblock::lsb64_intrinsic(m_aBB[i])+WMUL(i);
				first_vertex_flag=false;
			}
		}
		BITBOARD bbh= m_aBB[nbbh] &~ Tables::mask_left[posh];	
		pc+=bblock::popc64(bbh);
		if(first_vertex_flag && (pc==1))
			singleton=bblock::lsb64_intrinsic(bbh)+WMUL(nbbh);
	}

	if(pc==0) return 0;
	else if(pc==1) return 1;
	return -1;					//more than one vertex
}


//inline void  BitBoardN::copy_up_to_block (int last_block, const BitBoardN& bb_add){
////////////////
//// copies up to last_bit included and clears t
//// OBSERVATIONS: No out-of-bounds check for last_bit (will throw exception)
//
//	for(int i=0; i<=last_block; i++){
//		m_aBB[i]=bb_add.m_aBB[i];
//	}
//}
template<bool EraseAll>
inline BitBoardN& BitBoardN::set_bit	(int nbit ){
	
	if (EraseAll) {
		erase_bit();
	}

	m_aBB[WDIV(nbit)] |= Tables::mask[WMOD(nbit)];
	return *this;
}	

template<bool EraseAll>
inline 
BitBoardN&  BitBoardN::set_bit (int low, int high){

	//////////////////////////////
	assert(low < high || low > 0);
	//////////////////////////////

	int bbl= WDIV(low);
	int bbh= WDIV(high); 
	

	//clears all bits if required
	if (EraseAll) {
		erase_bit();
	}	

	if(bbl == bbh)
	{			
		m_aBB[bbh] |= bblock::MASK_1(low - WMUL(bbl), high - WMUL(bbh));			//low - WMUL(bbl) = WMOD(bbl) but less expensive (CHECK 01/02/25)
	}
	else
	{
		//set to one the intermediate blocks
		for (int i = bbl + 1; i < bbh; ++i) {
			m_aBB[i] = ONE;
		}
		
		//sets the first and last blocks
		m_aBB[bbh] |= bblock::MASK_1_RIGHT	(high - WMUL(bbh));
		m_aBB[bbl] |= bblock::MASK_1_LEFT	(low - WMUL(bbl));

	}

	return *this;
}

inline
BitBoardN& BitBoardN::set_bit (){

	for (auto i = 0; i < m_nBB; ++i) {
		m_aBB[i] = ONE;
	}
	return *this;
}

inline
BitBoardN&  BitBoardN::set_bit (const BitBoardN& bb_add){

	/////////////////////////////////
	assert(m_nBB <= bb_add.m_nBB);
	/////////////////////////////////

	for (auto i = 0; i < m_nBB; ++i) {
		m_aBB[i] |= bb_add.m_aBB[i];
	}

	return *this;
}

inline
BitBoardN&  BitBoardN::set_block (int FirstBlock, int LastBlock, const BitBoardN& bb_add){

	///////////////////////////////////////////////////////////////////////////////
	assert((FirstBlock >= 0) && (LastBlock < m_nBB) && (FirstBlock <= LastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block;
	(LastBlock == -1) ? last_block = m_nBB - 1 : last_block = LastBlock;	

	for (auto i = FirstBlock; i <= last_block; ++i) {
		m_aBB[i] |= bb_add.m_aBB[i];
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_bit (){

	for (auto i = 0; i < m_nBB; ++i) {
		m_aBB[i] = ZERO;
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_bit(int nBit) {

	m_aBB[WDIV(nBit)] &= ~Tables::mask[WMOD(nBit)]; 
	return *this;
}

inline 
BitBoardN&  BitBoardN::erase_bit (int low, int high){

	/////////////////////////////////////////////
	assert(low < high || low > 0 );
	/////////////////////////////////////////////

	int bbl = WDIV(low);
	int bbh = WDIV(high);


	if (bbl == bbh)
	{
		m_aBB[bbh] &= bblock::MASK_0(low - WMUL(bbl), high - WMUL(bbh));			//low - WMUL(bbl) = WMOD(bbl) but less expensive (CHECK 01/02/25)
	}
	else
	{
		//set to one the intermediate blocks
		for (int i = bbl + 1; i < bbh; ++i) {
			m_aBB[i] = ZERO;
		}
				
		m_aBB[bbh] &= bblock::MASK_0_RIGHT	(high - WMUL(bbh));					//Tables::mask_left
		m_aBB[bbl] &= bblock::MASK_0_LEFT	(low - WMUL(bbl));					//Tables::mask_right	
	}

	return *this;

}

inline int BitBoardN::lsbn64() const{
/////////////////
// different implementations of lsbn depending on configuration

#ifdef DE_BRUIJN
	for(int i=0; i<m_nBB; i++){
		if(m_aBB[i])
#ifdef ISOLANI_LSB
			return(Tables::indexDeBruijn64_ISOL[((m_aBB[i] & -m_aBB[i]) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(i));	
#else
			return(Tables::indexDeBruijn64_SEP[((m_aBB[i]^ (m_aBB[i]-1)) * bblock::DEBRUIJN_MN_64_SEP/*magic num*/) >> bblock::DEBRUIJN_MN_64_SHIFT]+ WMUL(i));
#endif
	}
#elif LOOKUP
	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=0; i<m_nBB; i++){
		val.b=m_aBB[i];
		if(val.b){
			if(val.c[0]) return (Tables::lsba[0][val.c[0]]+WMUL(i));
			if(val.c[1]) return (Tables::lsba[1][val.c[1]]+WMUL(i));
			if(val.c[2]) return (Tables::lsba[2][val.c[2]]+WMUL(i));
			if(val.c[3]) return (Tables::lsba[3][val.c[3]]+WMUL(i));
		}
	}

#endif
return EMPTY_ELEM;	
}


inline bool BitBoardN::is_singleton()const{
/////////////////////////////
// optimized for dense graphs
	int pc=0;
	for(int i=0; i<m_nBB; i++){
		if((pc+= bblock::popc64(m_aBB[i]))>1) return false; 
	}
	if(pc) return true;
return false;
}


int BitBoardN::popcn64() const{
	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	for(int i=0; i<m_nBB; i++){
		val.b = m_aBB[i]; //Loads union
		npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

return npc;
}

int BitBoardN::popcn64(int nBit) const{
/////////////////////////
// Population size from nBit(included) onwards

	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	int nBB=WDIV(nBit);

	for(int i=nBB+1; i<m_nBB; i++){
		val.b = m_aBB[i]; //Loads union
		npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

	//special case of nBit bit block
	val.b = m_aBB[nBB]&~Tables::mask_right[WMOD(nBit)];		//Loads union
	npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	
return npc;
}


inline int BitBoardN::single_disjoint (const BitBoardN& rhs, int& vertex) const{
/////////////////////
// PARAMS 
// vertex: Single vertex which is in lhs (this) and not in rhs (this) and rhs 
// 
// RETURN value: 0 if disjoint, 1 if single_disjoint, EMPTY_ELEM otherwise 


	int pc=0;
	vertex=EMPTY_ELEM;
	bool first_time=true;
	
	for(int i=0; i<m_nBB; i++){
		pc+=bblock::popc64(this->m_aBB[i] & rhs.m_aBB[i]);
		if(pc>1){
			vertex=EMPTY_ELEM;
			return EMPTY_ELEM;
		}else if(pc==1 && first_time ){  //store vertex position
			vertex=bblock::lsb64_intrinsic(this->m_aBB[i] & rhs.m_aBB[i])+ WMUL(i);
			first_time=false;
		}
	}
	
	return pc;		//disjoint
}


inline	int	BitBoardN::single_disjoint (int first_block, int last_block, const BitBoardN& rhs, int& vertex) const{
//////////////////////
// single_disjoint including both end blocks (*** TODO: perhaps semi-open range?)
// date_of_creation: 14/8/16 
//
// Application: single_disjoint operation with bitstrings of different size (max clique)
// 
// REMARKS: does not check block range

	int pc=0;
	vertex=EMPTY_ELEM;
	bool first_time=true;
	
	for(int i=first_block; i<=last_block; i++){
		pc+=bblock::popc64(this->m_aBB[i] & rhs.m_aBB[i]);
		if(pc>1){
			vertex=EMPTY_ELEM;
			return EMPTY_ELEM;
		}else if(pc==1 && first_time ){  //store vertex position
			vertex=bblock::lsb64_intrinsic(this->m_aBB[i] & rhs.m_aBB[i] )+ WMUL(i);
			first_time=false;
		}
	}
	
	return pc;		//disjoint
}


inline int BitBoardN::single_joint (const BitBoardN& rhs, int& vertex) const{
/////////////////////
// date: 27/7/16 
// PARAMS 
// vertex: Single vertex from lhs (this) which does not belong to rhs or EMPTY_ELEM (return value MUST BE 1)
// note that this operation studies the cardinality of a set-difference operator, and is thus not 
// conmutative.
// 
// 
// RETURN value: 0 if joint, 1 if single_joint, EMPTY_ELEM otherwise 
	
	int pc=0;
	vertex=EMPTY_ELEM;
	bool first_time=true;
	
	for(int i=0; i<m_nBB; i++){
		pc+=bblock::popc64(this->m_aBB[i] &~ rhs.m_aBB[i]);
		if(pc>1){
			vertex=EMPTY_ELEM;
			return EMPTY_ELEM;
		}else if(pc==1 && first_time ){  //store vertex position
			vertex=bblock::lsb64_intrinsic(this->m_aBB[i] &~ rhs.m_aBB[i] )+ WMUL(i);
			first_time=false;
		}
	}
	
	return pc;		//measure of jointness
}

inline int BitBoardN::double_joint (const BitBoardN& rhs, int& v, int& w) const{
/////////////////////
// date: 27/7/16 
// PARAMS 
// v, w: vertices from lhs (this) which do not belong to rhs or EMPTY_ELEM (return value MUST BE 1 or 2)
// note that this operation studies the cardinality of a set-difference operator, and is thus not 
// conmutative.
// 
// 
// RETURN value: 0 if joint, 1 if single_joint (v is the vertex, w=EMPTY_ELEM), 2 if double_joint (v, w are the vertices, EMPTY_ELEM otherwise 
	
	int pc=0;
	v=EMPTY_ELEM; w=EMPTY_ELEM;
	bool first_time=true; bool second_time=true;
	
	for(int i=0; i<m_nBB; i++){
		pc+=bblock::popc64(this->m_aBB[i] &~ rhs.m_aBB[i]);
		if(pc>2){
			v=EMPTY_ELEM; w=EMPTY_ELEM;
			return EMPTY_ELEM;
		}else if(pc==1 && first_time ){  //store vertex position
			v=bblock::lsb64_intrinsic(this->m_aBB[i] &~ rhs.m_aBB[i] )+ WMUL(i);
			first_time=false;
		}else if(pc==2 && second_time ){  //store vertex position
			BITBOARD bb=this->m_aBB[i] &~ rhs.m_aBB[i];
			if(first_time){
				v=bblock::lsb64_intrinsic(bb);
				w=bblock::lsb64_intrinsic(bb^Tables::mask[v])+WMUL(i);
				v+=WMUL(i);
			}else{
				w=bblock::lsb64_intrinsic(this->m_aBB[i] &~ rhs.m_aBB[i] )+ WMUL(i);
			}
			second_time=false;
		}
	}
	
	return pc;		//measure of jointness
}

inline
bool operator==	(const BitBoardN& lhs, const BitBoardN& rhs){
	
	for(int i=0; i<lhs.m_nBB; i++)
		if( lhs.m_aBB[i]!=rhs.m_aBB[i] ) return false;

return true;
}

inline
bool operator!=	(const BitBoardN& lhs, const BitBoardN& rhs){
	 return ! operator==(lhs, rhs);
}

inline
BitBoardN& BitBoardN::erase_bit (const BitBoardN& bbn){

	for (auto i = 0; i < m_nBB; ++i) {
		m_aBB[i] &= ~bbn.m_aBB[i];
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_bit (const BitBoardN& bb_lhs, const BitBoardN& bb_rhs ){

	for (auto i = 0; i < m_nBB; i++) {
		m_aBB[i] &= ~(bb_lhs.m_aBB[i] | bb_rhs.m_aBB[i]);
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_block (int FirstBlock, int LastBlock, const BitBoardN& bb_lhs, const BitBoardN& bb_rhs ){

	///////////////////////////////////////////////////////////////////////////////
	assert((FirstBlock >= 0) && (LastBlock < m_nBB) && (FirstBlock <= LastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block;
	(LastBlock == -1) ? last_block = m_nBB - 1 : last_block = LastBlock;

	for (auto i = FirstBlock; i <= LastBlock; ++i) {
		m_aBB[i] &= ~(bb_lhs.m_aBB[i] | bb_rhs.m_aBB[i]);
	}

	return *this;
}

inline
BitBoardN& BitBoardN::erase_block(int FirstBlock, int LastBlock, const BitBoardN& bb_del){

	///////////////////////////////////////////////////////////////////////////////
	assert((FirstBlock >= 0) && (LastBlock < m_nBB) && (FirstBlock <= LastBlock));
	///////////////////////////////////////////////////////////////////////////////

	int last_block;
	(LastBlock == -1) ? last_block = m_nBB - 1 : last_block = LastBlock;

	for (auto i = FirstBlock; i <= last_block; ++i) {
		m_aBB[i] &= ~bb_del.m_aBB[i];
	}

	return *this;
}


inline
int FIRST_SHARED (const BitBoardN& lhs, const BitBoardN& rhs){
/////////////////////
// RETURNS first bit common to lhs and rhs, EMPTY ELEM if sets are disjoint
	
	for(int i=0; i<lhs.m_nBB; i++){
		BITBOARD bb=lhs.m_aBB[i] & rhs.m_aBB[i];
		if(bb){
			return bblock::lsb64_intrinsic(bb)+WMUL(i);
		}
	}
	
	return EMPTY_ELEM;		//disjoint
}

inline
int FIRST_SHARED(int first_block, const BitBoardN& lhs, const BitBoardN& rhs) {
	/////////////////////
	// RETURNS first bit common to lhs and rhs, EMPTY ELEM if sets are disjoint

	for (int i = first_block; i < lhs.m_nBB; i++) {
		BITBOARD bb = lhs.m_aBB[i] & rhs.m_aBB[i];
		if (bb) {
			return bblock::lsb64_intrinsic(bb) + WMUL(i);
		}
	}

	return EMPTY_ELEM;		//disjoint
}

template<bool EraseAll>
inline
BitBoardN& BitBoardN::set_bit(const vint& lv) {

	//cleans the bitstring if required
	if (EraseAll) {
		erase_bit();
	}

	//copies elements up to the maximum capacity of the bitstring
	auto POPSIZE = WMUL(m_nBB);
	for (auto i = 0; i < lv.size(); ++i) {

		/////////////////////
		assert(lv[i] >= 0);
		////////////////////

		if (lv[i] < POPSIZE /* 1-based*/) {
			set_bit(lv[i]);
		}
	}

	return *this;

}



#endif