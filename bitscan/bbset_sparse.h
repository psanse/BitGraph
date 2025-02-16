 /**
  * @file bbset_sparse.h
  * @brief header for sparse class equivalent to the BitSet class
  * @author pss
  * @details created ?, @last_update 15/02/2025
  *
  * TODO refactoring and testing 15/02/2025 - follow the interface of the refactored BitSet
  **/

#ifndef __BBSET_SPARSE_H__
#define __BBSET_SPARSE_H__

#include "bbobject.h"
#include "bitblock.h"	
#include <vector>	
#include <algorithm>
#include <functional>
#include <iterator>

using namespace std;

constexpr int DEFAULT_CAPACITY = 2;								//initial reserve of bit blocks for any new sparse bitstring (possibly remove)

 
/////////////////////////////////
//
// class BitBoardS
// (Manages sparse bit strings)
//
///////////////////////////////////

class BitBoardS: public BBObject{
	
public:
	struct pBlock_t{
		int idx_;
		BITBOARD bb_;

		pBlock_t(int idx = BBObject::noBit, BITBOARD bb = 0):idx_(idx), bb_(bb){}

		bool operator ==	(const pBlock_t& e)	const	{ return (idx_ == e.idx_ && bb_ == e.bb_); }
		bool operator !=	(const pBlock_t& e)	const	{ return (idx_ != e.idx_ || bb_ != e.bb_); }
		bool operator !		()					const	{ return !bb_ ; }
		bool is_empty		()					const	{ return !bb_ ; }
		void clear			()							{ bb_ = 0;}
	};

	//aliases
	using velem = vector<pBlock_t>;
	using velem_it = vector<pBlock_t>::iterator;
	using velem_cit = vector<pBlock_t>::const_iterator;
		
	//functor for sorting - check if it is necessary
	struct elem_less {
		bool operator()(const pBlock_t& lhs, const pBlock_t& rhs) {
			return lhs.idx_ < rhs.idx_;
		}
	};

private:
	static int nElem;																						//to cache current bitblock position in the collection (not its idx_ ni the bitstring) 
public:	
	friend bool	operator ==			(const BitBoardS&, const BitBoardS&);
	friend bool operator!=			(const BitBoardS& lhs, const BitBoardS& rhs);

    friend inline BitBoardS&  AND	(const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);
	friend inline BitBoardS&  AND	(int first_block, const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);
	friend inline BitBoardS&  AND	(int first_block, int last_block, const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);
	friend BitBoardS&  OR			(const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);
	friend BitBoardS&  ERASE		(const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);			//removes rhs from lhs


	BitBoardS						():m_MAXBB(BBObject::noBit){}												//is this necessary?											
explicit BitBoardS					(int size, bool is_popsize=true );										//popsize is 1-based
	BitBoardS						(const BitBoardS& );	
virtual ~BitBoardS					(){clear();}	

	int init						(int size, bool is_popsize=true);					
	void clear						();
	void sort						();
	BitBoardS& operator =			(const BitBoardS& );	
/////////////////////
//setters and getters (will not allocate memory)
											
	int number_of_bitblocks			()						const {return m_aBB.size();}
	BITBOARD bitblock				(int idx_)				const {return m_aBB[idx_].bb_;}			//index in the collection			
	BITBOARD find_bitboard			(int block)		const;											//O(log) operation
	pair<bool, int>	 find_pos		(int block)		const;											//O(log) operation
	pair<bool, velem_it> find_block	(int block, bool is_lb=true);	
	pair<bool, velem_cit>find_block (int block, bool is_lb=true)	const;	
	velem_it  begin					(){return m_aBB.begin();}
	velem_it  end					(){return m_aBB.end();}
	velem_cit  begin				()	const {return m_aBB.cbegin();}
	velem_cit  end					()  const {return m_aBB.cend();}
	const velem& get_elem_set		() const {return m_aBB;}
	velem&	get_elem_set			()  {return m_aBB;}
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
//Set/Delete Bits (nbit is always 0 based)
		int   init_bit				(int nbit);	
		int   init_bit				(int lbit, int rbit);	
		int   init_bit				(int last_bit, const BitBoardS& bb_add);							//copies up to last_bit included
		int   init_bit				(int lbit, int rbit, const BitBoardS& bb_add);						//copies up to last_bit included
inline	int   set_bit				(int nbit);															//ordered insertion by bit block index
		int	  set_bit				(int lbit, int rbit);												//CLOSED range
BitBoardS&    set_bit				(const BitBoardS& bb_add);											//OR

BitBoardS&  set_block				(int first_block, const BitBoardS& bb_add);							//OR:closed range
BitBoardS&  set_block				(int first_block, int last_block, const BitBoardS& rhs);			//OR:closed range
		
inline	void  erase_bit				(int nbit);	
inline	velem_it  erase_bit			(int nbit, velem_it from_it);
		int	  erase_bit				(int lbit, int rbit);
		int	  clear_bit				(int lbit, int rbit);											//deallocates blocks
		void  shrink_to_fit			(){m_aBB.shrink_to_fit();}
		void  erase_bit				() {m_aBB.clear();}												//clears all bit blocks
BitBoardS&    erase_bit				(const BitBoardS&);				

BitBoardS&    erase_block			(int first_block, const BitBoardS& rhs );
BitBoardS&    erase_block			(int first_block, int last_block, const BitBoardS& rhs );
BitBoardS&    erase_block_pos		(int first_pos_of_block, const BitBoardS& rhs );

////////////////////////
//Operators 
 BitBoardS& operator &=				(const BitBoardS& );					
 BitBoardS& operator |=				(const BitBoardS& );
 BitBoardS& AND_EQ					(int first_block, const BitBoardS& rhs );						//in range
 BitBoardS& OR_EQ					(int first_block, const BitBoardS& rhs );						//in range
  		
/////////////////////////////
//Boolean functions
inline	bool is_bit					(int nbit)				const;									//nbit is 0 based
inline	bool is_empty				()						const;									//lax: considers empty blocks for emptyness
		bool is_disjoint			(const BitBoardS& bb)   const;
		bool is_disjoint			(int first_block, int last_block, const BitBoardS& bb)   const;
/////////////////////
//I/O 
	ostream& print					(ostream& = cout, bool show_pc = true, bool endl = true ) const override;

/////////////////////
//Conversions
	string to_string				();	
	void to_vector					(std::vector<int>& )	const;

////////////////////////
//data members
protected:
	velem m_aBB;					//a vector of sorted pairs of a non-empty bitblock and its index in a non-sparse bitstring
	int m_MAXBB;					//maximum number of bitblocks
};


//////////////////////////
//
// BOOLEAN FUNCTIONS
//
//////////////////////////

bool BitBoardS::is_bit(int nbit)	const{
//////////////////////////////
// RETURNS: TRUE if the bit is 1 in the position nbit, FALSE if opposite case or ERROR
//
// REMARKS: could be implemented in terms of find_bitboard

	//lower_bound implementation
	int idx=WDIV(nbit);
	velem_cit it=lower_bound(m_aBB.begin(), m_aBB.end(), pBlock_t(idx), elem_less());
	if(it!=m_aBB.end()){
		if((*it).idx_==idx)
			return ((*it).bb_ & Tables::mask[WMOD(nbit)]);
	}
	return false;
}


bool BitBoardS::is_empty ()	const{
///////////////////////
// 
// REMARKS:	The bit string may be empty either because it is known that there are no blocks (size=0)
//			or because the blocks contain no 1-bit (we admit this option explicitly for efficiency)

	if(m_aBB.empty()) return true;

	for(int i=0; i<m_aBB.size(); i++){
		if(m_aBB[i].bb_) return false;
	}
	
	
	//find if all elements are 0 (check efficiency)
	/*velem_cit it=find_if (m_aBB.begin(), m_aBB.end(), mem_fun_ref(&elem::is_not_empty)); 
	return (it==m_aBB.end());*/

return true;
}

inline
bool BitBoardS::is_disjoint	(const BitBoardS& rhs) const{
///////////////////
// true if there are no bits in common 
	int i1=0, i2=0;
	while(true){
		//exit condition I
		if(i1==m_aBB.size() || i2==rhs.m_aBB.size() ){		//size should be the same
					return true;
		}

		//update before either of the bitstrings has reached its end
		if(m_aBB[i1].idx_==rhs.m_aBB[i2].idx_){
			if(m_aBB[i1].bb_ & rhs.m_aBB[i2].bb_)
							return false;	//bit in common	
			++i1; ++i2;
		}else if(m_aBB[i1].idx_<rhs.m_aBB[i2].idx_){
			i1++;
		}else if(rhs.m_aBB[i2].idx_<m_aBB[i1].idx_){
			i2++;
		}
	}

return true;
}

inline 
bool BitBoardS::is_disjoint	(int first_block, int last_block, const BitBoardS& rhs)   const{
///////////////////
// true if there are no bits in common in the closed range [first_block, last_block]
//
// REMARKS: 
// 1) no assertions on valid ranges

	int i1=0, i2=0;

	//updates initial element indexes it first_block is defined
	if(first_block>0){

		pair<bool, int> p1=this->find_pos(first_block);
		pair<bool, int> p2=rhs.find_pos(first_block);

		//checks whether both sparse bitstrings have at least one block greater or equal to first_block
		if(p1.second==BBObject::noBit || p2.second==BBObject::noBit) return true;
		i1=p1.second; i2=p2.second;
	}

	//main loop
	int nElem=this->m_aBB.size(); int nElem_rhs=rhs.m_aBB.size();
	while(! ((i1>=nElem || this->m_aBB[i1].idx_>last_block ) || (i2>=nElem_rhs || rhs.m_aBB[i2].idx_>last_block )) ){

		//update before either of the bitstrings has reached its end
		if(this->m_aBB[i1].idx_<rhs.m_aBB[i2].idx_){
			i1++;
		}else if(rhs.m_aBB[i2].idx_<this->m_aBB[i1].idx_){
			i2++;
		}else{
			if(this->m_aBB[i1].bb_ & rhs.m_aBB[i2].bb_)
				return false;				
			i1++, i2++; 
		}

		/*if(lhs.m_aBB[i1].idx_==rhs.m_aBB[i2].idx_){
		BitBoardS::elem e(lhs.m_aBB[i1].idx_, lhs.m_aBB[i1].bb_ & rhs.m_aBB[i2].bb_);
		res.m_aBB.push_back(e);
		i1++, i2++; 
		}else if(lhs.m_aBB[i1].idx_<rhs.m_aBB[i2].idx_){
		i1++;
		}else if(rhs.m_aBB[i2].idx_<lhs.m_aBB[i1].idx_){
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

void BitBoardS::erase_bit(int nbit /*0 based*/){
//////////////
// clears bitblock information (does not remove bitblock if empty) 
// REMARKS: range must be sorted

	int idx = WDIV(nbit);

	//equal_range implementation 
	/*pair<velem_it, velem_it> p=equal_range(m_aBB.begin(), m_aBB.end(), elem(WDIV(nbit)), elem_less());
	if(distance(p.first,p.second)!=0){
		(*p.first).bb_&=~Tables::mask[WMOD(nbit)];
	}*/
	
	//lower_bound implementation
	velem_it it=lower_bound(m_aBB.begin(), m_aBB.end(), pBlock_t(idx), elem_less());
	if(it!=m_aBB.end()){
		//check if the element exists already
		if (it->idx_ == idx) {
			it->bb_ &= ~Tables::mask[WMOD(nbit)];
		}
	}
}

BitBoardS::velem_it  BitBoardS::erase_bit (int nbit, BitBoardS::velem_it from_it){
	
	int idx=WDIV(nbit);
		
	//lower_bound implementation
	velem_it it=lower_bound(from_it, m_aBB.end(), pBlock_t(idx), elem_less());
	if(it!=m_aBB.end()){
		//check if the element exists already
		if(it->idx_== idx)
			it->bb_&=~Tables::mask[WMOD(nbit)];
	}
return it;
}

int  BitBoardS::set_bit (int nbit ){
///////////////
//  General function for setting bits  
//
//  REMARKS:
//  1-Penalty cost for insertion
//  2-The other idea would be to allow unordered insertion and sort afterwards. This would be sensible for initial setting of bits operation (i.e. when reading from file)

	int idx = WDIV(nbit);

	//ASSERT
	if (idx >=m_MAXBB){
		cout<<"bit outside population limit"<<endl;
		cout<<"bit: "<<nbit<<" block: "<< idx <<" max block: "<<m_MAXBB<<endl;
		return -1;
	}

	//lower_bound implementation
	velem_it it=lower_bound(m_aBB.begin(), m_aBB.end(), pBlock_t(idx), elem_less());
	if(it!=m_aBB.end()){
		//check if the element exists already
		if(it->idx_== idx){
			it->bb_|=Tables::mask[WMOD(nbit)];
		}else 	//new inserted element
			m_aBB.insert(it, pBlock_t(idx,Tables::mask[WMOD(nbit)]));
	
	}else{
		//insertion at the end
		m_aBB.push_back(pBlock_t(idx,Tables::mask[WMOD(nbit)]));
	}
	
return 0;  //ok
}

inline
int BitBoardS::init_bit (int nBit){
///////////////////
// sets bit and clears the rest
	
	//**assert MAXSIZE: return -1

	m_aBB.clear();
	m_aBB.push_back(pBlock_t(WDIV(nBit), Tables::mask[WMOD(nBit)]));

return 0;
}	

////////////////
//
// Bit scanning
//
/////////////////

int BitBoardS::prev_bit	(int nBit){
/////////////////
// Uses cache of last index position for fast bit scanning
//

	if(nBit==BBObject::noBit)
			return msbn64(nElem);		//updates nElem with the corresponding bitblock
	
	int index = WDIV(nBit);
	int npos=bblock::msb64_lup(Tables::mask_low[WMOD(nBit) /*nBit-WMUL(index)*/] & m_aBB[nElem].bb_);
	if(npos!=BBObject::noBit)
		return (WMUL(index) + npos);
	
	for(int i=nElem-1; i>=0; i--){  //new bitblock
		if( m_aBB[i].bb_){
			nElem=i;
			return bblock::msb64_de_Bruijn(m_aBB[i].bb_) + WMUL(m_aBB[i].idx_);
		}
	}
return BBObject::noBit;
}


int BitBoardS::next_bit(int nBit){
/////////////////
// Uses cache of last index position for fast bit scanning
//	
	if(nBit==BBObject::noBit)
		return lsbn64(nElem);		//updates nElem with the corresponding bitblock
	
	int idx = WDIV(nBit);
	int npos=bblock::lsb64_de_Bruijn(Tables::mask_high[WMOD(nBit) /* WORD_SIZE * idx */] & m_aBB[nElem].bb_);
	if(npos!=BBObject::noBit)
		return (WMUL(idx) + npos);
	
	for(int i=nElem+1; i<m_aBB.size(); i++){
		//new bitblock
		if(m_aBB[i].bb_){
			nElem=i;
			return bblock::lsb64_de_Bruijn(m_aBB[i].bb_) + WMUL(m_aBB[i].idx_);
		}
	}
return BBObject::noBit;
}

int BitBoardS::msbn64	(int& nElem)	const{
///////////////////////
// Look up table implementation (best found so far)
//
// RETURNS element index of the bitblock

	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=m_aBB.size()-1; i>=0; i--){
		val.b=m_aBB[i].bb_;
		if(val.b){
			nElem=i;
			if(val.c[3]) return (Tables::msba[3][val.c[3]]+WMUL(m_aBB[i].idx_));
			if(val.c[2]) return (Tables::msba[2][val.c[2]]+WMUL(m_aBB[i].idx_));
			if(val.c[1]) return (Tables::msba[1][val.c[1]]+WMUL(m_aBB[i].idx_));
			if(val.c[0]) return (Tables::msba[0][val.c[0]]+WMUL(m_aBB[i].idx_));
		}
	}

return BBObject::noBit;		//should not reach here
}

int BitBoardS::lsbn64 (int& nElem)		const	{
/////////////////
// different implementations of lsbn depending on configuration
//
// RETURNS element index of the bitblock
	
#ifdef DE_BRUIJN
	for(int i=0; i<m_aBB.size(); i++){
		if(m_aBB[i].bb_){
			nElem=i;
#ifdef ISOLANI_LSB
			return(Tables::indexDeBruijn64_ISOL[((m_aBB[i].bb_ & -m_aBB[i].bb_) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(m_aBB[i].idx_));	
#else
			return(Tables::indexDeBruijn64_SEP[((m_aBB[i].bb_ ^ (m_aBB[i].bb_ - 1)) * bblock::DEBRUIJN_MN_64_SEP/*magic num*/) >> bblock::DEBRUIJN_MN_64_SHIFT] + WMUL(m_aBB[i].idx_));
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
		val.b=m_aBB[i].bb_;
		nElem=i;
		if(val.b){
			if(val.c[0]) return (Tables::lsba[0][val.c[0]]+WMUL(m_aBB[i].idx_));
			if(val.c[1]) return (Tables::lsba[1][val.c[1]]+WMUL(m_aBB[i].idx_));
			if(val.c[2]) return (Tables::lsba[2][val.c[2]]+WMUL(m_aBB[i].idx_));
			if(val.c[3]) return (Tables::lsba[3][val.c[3]]+WMUL(m_aBB[i].idx_));
		}
	}

#endif
return BBObject::noBit;	
}

inline
int BitBoardS::popcn64() const{
	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	for(int i=0; i<m_aBB.size(); i++){
		val.b = m_aBB[i].bb_; 
		npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

return npc;
}

inline
int BitBoardS::popcn64(int nBit) const{
	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	int nBB=WDIV(nBit);

	//find the biblock if it exists
	velem_cit it=lower_bound(m_aBB.begin(), m_aBB.end(), pBlock_t(nBB), elem_less());
	if(it!=m_aBB.end()){
		if(it->idx_==nBB){
			val.b= it->bb_&~Tables::mask_low[WMOD(nBit)];
			npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
			it++;
		}
		
		//searches in the rest of elements with greater index than nBB
		for(; it!=m_aBB.end(); ++it){
			val.b = it->bb_; //Loads union
			npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
		}
	}
	
return npc;
}

inline
BitBoardS& AND (const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res){
///////////////////////////
// AND between sparse sets
//		
	int i2=0;
	res.erase_bit();					//experimental (and simplest solution)
	const int MAX=rhs.m_aBB.size()-1;

	//empty check of rhs required, the way it is implemented
	if(MAX==BBObject::noBit) return res;
	
	//optimization which works if lhs has less 1-bits than rhs
	int lhs_SIZE=lhs.m_aBB.size();
	for (int i1 = 0; i1 < lhs_SIZE;i1++){
		for(; i2<MAX && rhs.m_aBB[i2].idx_<lhs.m_aBB[i1].idx_; i2++){}
		
		//update before either of the bitstrings has reached its end
		if(lhs.m_aBB[i1].idx_ == rhs.m_aBB[i2].idx_){
				res.m_aBB.push_back(BitBoardS::pBlock_t(lhs.m_aBB[i1].idx_, lhs.m_aBB[i1].bb_ & rhs.m_aBB[i2].bb_));
		}
	}

	//general purpose code assuming no a priori knowledge about density in lhs and rhs
	//int i1=0, i2=0;
	//while(i1!=lhs.m_aBB.size() && i2!=rhs.m_aBB.size() ){
	//	//update before either of the bitstrings has reached its end
	//	if(lhs.m_aBB[i1].idx_<rhs.m_aBB[i2].idx_){
	//		i1++;
	//	}else if(rhs.m_aBB[i2].idx_<lhs.m_aBB[i1].idx_){
	//		i2++;
	//	}else{
	//		BitBoardS::elem e(lhs.m_aBB[i1].idx_, lhs.m_aBB[i1].bb_ & rhs.m_aBB[i2].bb_);
	//		res.m_aBB.push_back(e);
	//		i1++, i2++; 
	//	}
	//	
	//}
return res;
}

inline
BitBoardS& AND (int first_block, const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res){
///////////////////////////
// AND between sparse sets from first_block (included) onwards
//
			
	res.erase_bit();
	pair<bool, int> p1=lhs.find_pos(first_block);
	pair<bool, int> p2=rhs.find_pos(first_block);
	if(p1.second!=BBObject::noBit && p2.second!=BBObject::noBit){
		int i1=p1.second, i2=p2.second;
		while( i1!=lhs.m_aBB.size() && i2!=rhs.m_aBB.size() ){
			
			//update before either of the bitstrings has reached its end
			if(lhs.m_aBB[i1].idx_<rhs.m_aBB[i2].idx_){
				i1++;
			}else if(rhs.m_aBB[i2].idx_<lhs.m_aBB[i1].idx_){
				i2++;
			}else{
				BitBoardS::pBlock_t e(lhs.m_aBB[i1].idx_, lhs.m_aBB[i1].bb_ & rhs.m_aBB[i2].bb_);
				res.m_aBB.push_back(e);
				i1++, i2++; 
			}


		/*	if(lhs.m_aBB[i1].idx_==rhs.m_aBB[i2].idx_){
				BitBoardS::elem e(lhs.m_aBB[i1].idx_, lhs.m_aBB[i1].bb_ & rhs.m_aBB[i2].bb_);
				res.m_aBB.push_back(e);
				i1++, i2++; 
			}else if(lhs.m_aBB[i1].idx_<rhs.m_aBB[i2].idx_){
				i1++;
			}else if(rhs.m_aBB[i2].idx_<lhs.m_aBB[i1].idx_){
				i2++;
			}*/
		}
	}
return res;
}


inline
BitBoardS& AND (int first_block, int last_block, const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res){
///////////////////////////
// AND between sparse sets in closed range
// last update: 21/3/15 -BUG corrected concerning last_block and first_block value optimization  
//
// REMARKS: no assertions on valid ranges
		
	res.erase_bit();
	int i1=0, i2=0;

	//updates initial element indexes it first_block is defined
	if(first_block>0){

		pair<bool, int> p1=lhs.find_pos(first_block);
		pair<bool, int> p2=rhs.find_pos(first_block);

		//checks whether both sparse bitstrings have at least one block greater or equal to first_block
		if(p1.second==BBObject::noBit || p2.second==BBObject::noBit) return res;
		i1=p1.second; i2=p2.second;
	}


	//main loop	
	int nElem_lhs=lhs.m_aBB.size(); int nElem_rhs=rhs.m_aBB.size();
	while(! ((i1>=nElem_lhs|| lhs.m_aBB[i1].idx_>last_block) || (i2>=nElem_rhs || rhs.m_aBB[i2].idx_>last_block)) ){
			
		//update before either of the bitstrings has reached its end
		if(lhs.m_aBB[i1].idx_<rhs.m_aBB[i2].idx_){
			i1++;
		}else if(rhs.m_aBB[i2].idx_<lhs.m_aBB[i1].idx_){
			i2++;
		}else{
			BitBoardS::pBlock_t e(lhs.m_aBB[i1].idx_, lhs.m_aBB[i1].bb_ & rhs.m_aBB[i2].bb_);
			res.m_aBB.push_back(e);
			i1++, i2++; 
		}

		/*if(lhs.m_aBB[i1].idx_==rhs.m_aBB[i2].idx_){
			BitBoardS::elem e(lhs.m_aBB[i1].idx_, lhs.m_aBB[i1].bb_ & rhs.m_aBB[i2].bb_);
			res.m_aBB.push_back(e);
			i1++, i2++; 
		}else if(lhs.m_aBB[i1].idx_<rhs.m_aBB[i2].idx_){
			i1++;
		}else if(rhs.m_aBB[i2].idx_<lhs.m_aBB[i1].idx_){
			i2++;
		}*/
	}

return res;
}

inline
BitBoardS&  ERASE (const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res){
/////////////////
// removes rhs from lhs
// date of creation: 17/12/15

	
	const int MAX=rhs.m_aBB.size()-1;
	if(MAX==BBObject::noBit){ return (res=lhs);  }		//copy before returning
	res.erase_bit();

	//this works better if lhs is as sparse as possible (iterating first over rhs is illogical here becuase the operation is not symmetrical)
	int i2=0;
	int lhs_SIZE=lhs.m_aBB.size();
	for (int i1 = 0; i1 < lhs_SIZE; i1++){
		for(; i2<MAX && rhs.m_aBB[i2].idx_<lhs.m_aBB[i1].idx_; i2++){}
		
		//update before either of the bitstrings has reached its end
		if(lhs.m_aBB[i1].idx_==rhs.m_aBB[i2].idx_){
				res.m_aBB.push_back(BitBoardS::pBlock_t(lhs.m_aBB[i1].idx_, lhs.m_aBB[i1].bb_ &~ rhs.m_aBB[i2].bb_));
		}else{
			res.m_aBB.push_back(BitBoardS::pBlock_t(lhs.m_aBB[i1].idx_, lhs.m_aBB[i1].bb_));
		}
	}
return res;
}

inline
BitBoardS&  BitBoardS::erase_block (int first_block, const BitBoardS& rhs ){
////////////////////
// removes 1-bits from current object (equialent to set_difference) from first_block (included) onwards			

	pair<bool, BitBoardS::velem_it> p1=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2=rhs.find_block(first_block);
	
	//optimization based on the size of rhs being greater
	//for (int i1 = 0; i1 < lhs.m_aBB.size();i1++){

	//iteration
	while( ! ( p1.second==m_aBB.end() ||  p2.second==rhs.m_aBB.end() ) ){
		////exit condition I
		//if(p1.second==m_aBB.end() || p2.second==rhs.m_aBB.end() ){		//should be the same
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
BitBoardS&  BitBoardS::AND_EQ(int first_block, const BitBoardS& rhs ){
//////////////////////
// left intersection (AND). bits in rhs remain starting from closed range [first_block, END[

	pair<bool, BitBoardS::velem_it> p1=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2=rhs.find_block(first_block);
	
	//optimization based on the size of rhs being greater
	//for (int i1 = 0; i1 < lhs.m_aBB.size();i1++){...}

	//iteration
	while( true ){
		//exit condition 
		if(p1.second==m_aBB.end() ){		//size should be the same
					return *this;
		}else if( p2.second==rhs.m_aBB.end()){  //fill with zeros from last block in rhs onwards
			for(; p1.second!=m_aBB.end(); ++p1.second)
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
BitBoardS&  BitBoardS::OR_EQ(int first_block, const BitBoardS& rhs ){
//////////////////////
// left union (OR). Bits in rhs are added starting from closed range [first_block, END[

	pair<bool, BitBoardS::velem_it> p1=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2=rhs.find_block(first_block);
	
	//iteration
	while(true){
		//exit condition 
		if(p1.second==m_aBB.end() || p2.second==rhs.m_aBB.end() ){				//size should be the same
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
BitBoardS&  BitBoardS::erase_block_pos (int first_pos_of_block, const BitBoardS& rhs ){
/////////////////////
// erases bits from a starting block in *this (given as the position in the bitstring collection, not its index) till the end of the bitstring, 

	int i2=0;							//all blocks in rhs are considered
	const int MAX=rhs.m_aBB.size()-1;
	
	//optimization which works if rhs has less 1-bits than this
	for (int i1 = first_pos_of_block; i1 <m_aBB.size(); i1++){
		for(; i2<MAX && rhs.m_aBB[i2].idx_< m_aBB[i1].idx_; i2++){}
		
		//update before either of the bitstrings has reached its end
		if(m_aBB[i1].idx_==rhs.m_aBB[i2].idx_){
				m_aBB[i1].bb_&=~rhs.m_aBB[i2].bb_;
		}
	}

return *this;
}


inline
BitBoardS&  BitBoardS::erase_block (int first_block, int last_block, const BitBoardS& rhs ){
////////////////////
// removes 1-bits from current object (equialent to set_difference) from CLOSED RANGE of blocks	

	pair<bool, BitBoardS::velem_it> p1=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2=rhs.find_block(first_block);
	if(p1.second==m_aBB.end() || p2.second==rhs.m_aBB.end() )
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
		if(p1.second==m_aBB.end() || p1.second->idx_>last_block || p2.second==rhs.m_aBB.end() || p2.second->idx_>last_block ){
			break;
		}
	}while(true);
	
return *this;
}


inline
int	 BitBoardS::erase_bit (int low, int high){
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
	velem_it itl=lower_bound(m_aBB.begin(), m_aBB.end(), pBlock_t(bbl), elem_less());
	if(itl!=m_aBB.end()){
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
		for(; itl!=m_aBB.end(); ++itl){
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
int BitBoardS::init_bit (int high, const BitBoardS& bb_add){
//////////////////////////////////
// fast copying of bb_add up to and including high bit
	
	m_aBB.clear();
	int	bbh=WDIV(high); 
	pair<bool, BitBoardS::velem_cit> p=bb_add.find_block(bbh);
	if(p.second==bb_add.end())
		copy(bb_add.begin(), bb_add.end(),insert_iterator<velem>(m_aBB,m_aBB.begin()));
	else{
		if(p.first){
			copy(bb_add.begin(), p.second, insert_iterator<velem>(m_aBB,m_aBB.begin()));
			m_aBB.push_back(pBlock_t(bbh, p.second->bb_ & ~Tables::mask_high[high-WMUL(bbh)]));
		}else{
			copy(bb_add.begin(), ++p.second, insert_iterator<velem>(m_aBB,m_aBB.begin()));
		}
	}
return 0;
}

inline
int  BitBoardS::init_bit (int low, int high,  const BitBoardS& bb_add){
/////////////
// fast copying of bb_add in the corresponding CLOSED range
// date of creation: 19/12/15 (bug corrected in 6/01/2016)

	//***ASSERT

	m_aBB.clear();
	int	bbl=WDIV(low);
	int	bbh=WDIV(high);


	//finds low bitblock and updates forward
	velem_cit itl=lower_bound(bb_add.begin(), bb_add.end(), pBlock_t(bbl), elem_less());
	if(itl!=bb_add.end()){
		if(itl->idx_==bbl){	//lower block exists
			if(bbh==bbl){		//case update in the same bitblock
				m_aBB.push_back(pBlock_t( bbh, itl->bb_ & bblock::MASK_1(low-WMUL(bbl), high-WMUL(bbh)) ));
				return 0;
			}else{
				//add lower block
				m_aBB.push_back(pBlock_t(bbl, itl->bb_ &~ Tables::mask_low[low-WMUL(bbl)] ));
				++itl;
			}
		}

		//copied the rest if elements
		for(; itl!=bb_add.end(); ++itl){
			if(itl->idx_>=bbh){		//exit condition
				if(itl->idx_==bbh){	
					m_aBB.push_back(pBlock_t(bbh, itl->bb_&~Tables::mask_high[high-WMUL(bbh)]));
				}
			return 0;
			}
			//copies the element as is
			m_aBB.push_back(*itl);
		}
	}

return 0;		//should not reach here
}

inline
bool operator == (const BitBoardS& lhs, const BitBoardS& rhs){
/////////////////////
// Simple equality check which considers exact copy of bit strings 
// REMARKS: does not take into account information, i.e. bit blocks=0

	return(lhs.m_aBB==rhs.m_aBB);
}

inline
bool operator!=	(const BitBoardS& lhs, const BitBoardS& rhs){
	return ! operator==(lhs, rhs);
}

#endif