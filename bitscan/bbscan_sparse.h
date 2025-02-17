/**
 * @file bbscan_sparse.h
 * @brief header for sparse class equivalent to the BBScan class
 *		  manages sparse bit strings with bitscanning capabilities
 * @author pss
 * @details created ?, @last_update 15/02/2025
 *
 * TODO refactoring and testing 15/02/2025
 **/

#ifndef __BBSCAN_SPARSE_H__
#define __BBSCAN_SPARSE_H__

#include "bbset_sparse.h"
using namespace std;

/////////////////////////////////
//
// Class BBIntrinS
// (Uses a number of optimizations for bit scanning)
//
///////////////////////////////////

class BBIntrinS: public BitSetSp{
public:	
				
	 BBIntrinS						(){};										
explicit BBIntrinS					(int popsize /*1 based*/, bool reset=true):BitSetSp(popsize,reset)	{}	
	 BBIntrinS						(const BBIntrinS& bbN):BitSetSp(bbN)	{}

	 void set_bbindex				(int bbindex)	{m_scan.bbi_=bbindex;}			//refers to the position in the collection (not in the bitstring)
	 void set_posbit				(int posbit)	{m_scan.pos_=posbit;}	
	
//////////////////////////////
// bitscanning
inline int lsbn64					() const;
inline int msbn64					() const; 
inline	int msbn64					(int& nElem)			const;				
inline	int lsbn64					(int& nElem)			const; 	

	//scan setup
	int init_scan					(scan_types);
	int init_scan_from				(int from, scan_types sct);				//currently only for the NON-DESTRUCTIVE case
	
	//bit scan forward (destructive)				
inline int next_bit_del				(); 												
inline int next_bit_del				(int& nBB);								//nBB: index of bitblock in the bitstring	(not in the collection)	
inline int next_bit_del				(int& nBB, BBIntrinS& );				//EXPERIMENTAL! 
inline int next_bit_del_pos			(int& posBB);							//posBB: position of bitblock in the collection (not the index of the element)		

	//bit scan forward (non destructive)
virtual inline int next_bit			();
inline int next_bit					(int & nBB);							//nBB: index of bitblock in the bitstring	(not in the collection)				

	//bit scan backwards (non destructive)
virtual inline int prev_bit			(); 

	//bit scan backwards (destructive)
inline int prev_bit_del				(); 
inline int prev_bit_del				(int& nBB);
				
/////////////////
// Popcount
#ifdef POPCOUNT_INTRINSIC_64
virtual	 inline int popcn64			()	const;
virtual	 inline int popcn64			(int nBit)	const;							//population size from (and including) nBit
#endif

//////////////////
/// data members

public:
	 scan_t m_scan;
};

///////////////////////
//
// INLINE FUNCTIONS
// 
////////////////////////

#ifdef POPCOUNT_INTRINSIC_64
inline 
int BBIntrinS::popcn64() const{
	BITBOARD pc=0;
	for(int i=0; i<vBB_.size(); i++){
		pc+=__popcnt64(vBB_[i].bb_);
	}
return pc;
}


inline
int BBIntrinS::popcn64(int nBit) const{
//////////////////////////////
// population  size from (and including) nBit onwards
	BITBOARD pc=0;
	int nBB=WDIV(nBit);

	//find the biblock if it exists
	vPB_cit it = lower_bound(vBB_.begin(), vBB_.end(), pBlock_t(nBB), elem_less());
	if(it!=vBB_.end()){
		if(it->idx_==nBB){
			BITBOARD bb= it->bb_&~Tables::mask_low[WMOD(nBit)];
			pc+= __popcnt64(bb);
			it++;
		}

		//searches in the rest of elements with greater index than nBB
		for(; it!=vBB_.end(); ++it){
			pc+=  __popcnt64(it->bb_);
		}
	}
	
return pc;
}

#endif

inline int BBIntrinS::lsbn64() const{
	
	unsigned long posbb;
	for(int i = 0; i < vBB_.size(); ++i){
		if(_BitScanForward64(&posbb, vBB_[i].bb_))
			return(posbb+ WMUL(vBB_[i].idx_));	
	}

	return EMPTY_ELEM;
}

inline int BBIntrinS::lsbn64(int& nElem) const{
////////////////////
// lsbn + returns in nElem the index in the collection if the bit string is not EMPTY

	unsigned long posbb;
	for(int i = 0; i < vBB_.size(); ++i){
		if(_BitScanForward64(&posbb, vBB_[i].bb_)){
			nElem = i;
			return(posbb + WMUL(vBB_[i].idx_));
		}
	}

	return EMPTY_ELEM;
}


inline int BBIntrinS::msbn64() const{
////////////////////
// Returns the index in the collection if bitstring is not EMPTY
	unsigned long posBB;
	for(int i=vBB_.size()-1; i>=0; i--){
		//Siempre me queda la duda mas de si es mas eficiente comparar con 0
		if(_BitScanReverse64(&posBB,vBB_[i].bb_))
				return (posBB+WMUL(vBB_[i].idx_));
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrinS::msbn64(int& nElem) const{
////////////////////
// Returns the index in the collection if bitstring is not EMPTY
	unsigned long posBB;
	for(int i=vBB_.size()-1; i>=0; i--){
		//Siempre me queda la duda mas de si es mas eficiente comparar con 0
		if(_BitScanReverse64(&posBB,vBB_[i].bb_)){
			nElem=i;
			return (posBB+WMUL(vBB_[i].idx_));
		}
	}
	
return EMPTY_ELEM;  
}


inline int BBIntrinS::next_bit() {
////////////////////////////
// date:5/9/2014
// non destructive bitscan for sparse bitstrings using intrinsics
// caches index in the collection and pos inside the bitblock
//
// comments
// 1-require previous assignment m_scan_bbi=0 and m_scan.pos=mask_lim

	unsigned long posbb;
			
	//search for next bit in the last block
	if(_BitScanForward64(&posbb, vBB_[m_scan.bbi_].bb_ & Tables::mask_high[m_scan.pos_])){
		m_scan.pos_ =posbb;
		return (posbb + WMUL(vBB_[m_scan.bbi_].idx_));
	}else{											//search in the remaining blocks
		for(int i=m_scan.bbi_+1; i<vBB_.size(); i++){
			if(_BitScanForward64(&posbb,vBB_[i].bb_)){
				m_scan.bbi_=i;
				m_scan.pos_=posbb;
				return (posbb+ WMUL(vBB_[i].idx_));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBIntrinS::next_bit(int& block_index) {
////////////////////////////
// date:5/9/2014
// non destructive bitscan for sparse bitstrings using intrinsics
// caches index in the collection and pos inside the bitblock
//
// comments
// 1-require previous assignment m_scan_bbi=0 and m_scan.pos_=mask_lim

	unsigned long posbb;
			
	//search for next bit in the last block
	if(_BitScanForward64(&posbb, vBB_[m_scan.bbi_].bb_ & Tables::mask_high[m_scan.pos_])){
		m_scan.pos_ =posbb;
		block_index= vBB_[m_scan.bbi_].idx_;
		return (posbb + WMUL(vBB_[m_scan.bbi_].idx_));
	}else{											//search in the remaining blocks
		for(int i=m_scan.bbi_+1; i<vBB_.size(); i++){
			if(_BitScanForward64(&posbb,vBB_[i].bb_)){
				m_scan.bbi_=i;
				m_scan.pos_=posbb;
				block_index=vBB_[i].idx_;
				return (posbb+ WMUL(vBB_[i].idx_));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBIntrinS::prev_bit	() {
////////////////////////////
// date:5/9/2014
// Non destructive bitscan for sparse bitstrings using intrinsics
// caches index in the collection and pos inside the bitblock
//
// comments
// 1-require previous assignment m_scan_bbi=number of bitblocks-1 and m_scan.pos_=WORD_SIZE

	unsigned long posbb;
				
	//search int the last table
	if(_BitScanReverse64(&posbb, vBB_[m_scan.bbi_].bb_ & Tables::mask_low[m_scan.pos_])){
		m_scan.pos_ =posbb;
		return (posbb + WMUL(vBB_[m_scan.bbi_].idx_));
	}else{											//not found in the last table. search in the rest
		for(int i=m_scan.bbi_-1; i>=0; i--){
			if(_BitScanReverse64(&posbb,vBB_[i].bb_)){
				m_scan.bbi_=i;
				m_scan.pos_=posbb;
				return (posbb+ WMUL(vBB_[i].idx_));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBIntrinS::next_bit_del() {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan (scans and deletes each 1-bit) for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=0 

	unsigned long posbb;

	for(int i=m_scan.bbi_; i<vBB_.size(); i++)	{
		if(_BitScanForward64(&posbb,vBB_[i].bb_)){
			m_scan.bbi_=i;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb + WMUL(vBB_[i].idx_));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrinS::next_bit_del(int& block_index) {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=0 

	unsigned long posbb;

	for(int i=m_scan.bbi_; i<vBB_.size(); i++)	{
		if(_BitScanForward64(&posbb,vBB_[i].bb_)){
			m_scan.bbi_=i;
			block_index=vBB_[i].idx_;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb + WMUL(vBB_[i].idx_));
		}
	}
	
return EMPTY_ELEM;  
}


inline int BBIntrinS::next_bit_del(int& block_index, BBIntrinS& bbN_del) {
	////////////////////////////
	//
	// date: 06/07/2019 (EXPERIMENTAL)
	// Destructive bitscan for sparse bitstrings using intrinsics
	//
	// COMMENTS
	// 1-added for compatibility with BBIntrin
	// 1-Requires previous assignment m_scan_bbi=0 

	unsigned long posbb;

	for (int i = m_scan.bbi_; i < vBB_.size(); i++) {
		if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
			m_scan.bbi_ = i;
			block_index = vBB_[i].idx_;
			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
			bbN_del.vBB_[i].bb_ &= ~Tables::mask[posbb];
			return (posbb + WMUL(vBB_[i].idx_));
		}
	}

	return EMPTY_ELEM;
}

inline int BBIntrinS::next_bit_del_pos (int& posBB){
////////////////////////////
//
// date: 29/10/14
// Destructive bitscan which returns the position of the bitblock scanned in the collection
// (not the index attribute)
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=0 

	unsigned long posbb;

	for(int i=m_scan.bbi_; i<vBB_.size(); i++)	{
		if(_BitScanForward64(&posbb,vBB_[i].bb_)){
			posBB=m_scan.bbi_=i;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb + WMUL(vBB_[i].idx_));
		}
	}
	
return EMPTY_ELEM;  
}


inline int BBIntrinS::prev_bit_del() {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=number of bitblocks-1

	unsigned long posbb;

	for(int i=m_scan.bbi_; i>=0; i--){
		if(_BitScanReverse64(&posbb,vBB_[i].bb_)){
			m_scan.bbi_=i;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb+WMUL(vBB_[i].idx_));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrinS::prev_bit_del(int & bb_index) {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=number of bitblocks-1

	unsigned long posbb;

	for(int i=m_scan.bbi_; i>=0; i--){
		if(_BitScanReverse64(&posbb,vBB_[i].bb_)){
			m_scan.bbi_=i;
			bb_index=vBB_[i].idx_;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb+WMUL(vBB_[i].idx_));
		}
	}
return EMPTY_ELEM;  
}

inline
int BBIntrinS::init_scan (scan_types sct){
	if(vBB_.empty()) return EMPTY_ELEM;				//necessary check since sparse bitstrings have empty semantics (i.e. sparse graphs)

	switch(sct){
	case NON_DESTRUCTIVE:
		set_bbindex(0);
		set_posbit(MASK_LIM);
		break;
	case NON_DESTRUCTIVE_REVERSE:
		set_bbindex(vBB_.size()-1);
		set_posbit(WORD_SIZE);
		break;
	case DESTRUCTIVE:
		set_bbindex(0); 
		break;
	case DESTRUCTIVE_REVERSE:
		set_bbindex(vBB_.size() - 1);
		break;
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}

inline
int BBIntrinS::init_scan_from (int from, scan_types sct){
////////////////////////
// scans starting at from until the end of the bitarray
//
// REMARKS: at the moment, only working for the NON-DESTRUCTIVE case

	pair<bool, int> p= find_pos(WDIV(from));
	if(p.second==EMPTY_ELEM) return EMPTY_ELEM;
	switch(sct){
	case NON_DESTRUCTIVE:
	case NON_DESTRUCTIVE_REVERSE:
		set_bbindex(p.second); 
		(p.first)? set_posbit(WMOD(from)) : set_posbit(MASK_LIM);
		break;
	/*case DESTRUCTIVE:
	case DESTRUCTIVE_REVERSE:
		set_bbindex(p.second);
		break;*/
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}


#endif 