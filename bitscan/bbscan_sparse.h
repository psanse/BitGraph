/**
 * @file bbscan_sparse.h
 * @brief header for sparse class equivalent to the BBScan class
 *		  manages sparse bit strings with efficient bitscanning capabilities
 * @author pss
 * @details: created 2015?, last_update 25/02/2025
 *
 * TODO refactoring and testing 25/02/2025
 **/

#ifndef __BBSCAN_SPARSE_H__
#define __BBSCAN_SPARSE_H__

#include "bbset_sparse.h"
using namespace std;

/////////////////////////////////
//
// Class BBScanSp
// (Uses a number of optimizations for sparse bit scanning)
//
///////////////////////////////////

class BBScanSp: public BitSetSp{
public:	

	//inherit constructors
	using BitSetSp::BitSetSp;

	//TODO...check copy and move assignments - should be forbidden

	~BBScanSp() = default;

///////////////
// setters and getters
	 void scan_block				(int blockID)	{ scan_.bbi_= blockID; }			//refers to the position in the collection (not in the bitstring)
	 void scan_bit					(int bit)		{ scan_.pos_= bit; }
	
	 int  scan_block				()	 const		{ return scan_.bbi_; }
	 int  scan_bit					()	 const		{ return scan_.pos_; }

 //////////////////////////////
 // Bitscanning (with cached info)

	//scan setup
	int init_scan					(scan_types);
	int init_scan_from				(int from, scan_types sct);				//currently only for the NON-DESTRUCTIVE case
	
	//bit scan forward (destructive)				
inline int next_bit_del				(); 												
inline int next_bit_del				(int& nBB);								//nBB: index of bitblock in the bitstring	(not in the collection)	
inline int next_bit_del				(int& nBB, BBScanSp& );					//EXPERIMENTAL! 
inline int next_bit_del_pos			(int& posBB);							//posBB: position of bitblock in the collection (not the index of the element)		

	//bit scan forward (non destructive)
virtual inline int next_bit			();
inline int next_bit					(int & nBB);							//nBB: index of bitblock in the bitstring	(not in the collection)				

	//bit scan backwards (non destructive)
virtual inline int prev_bit			(); 

	//bit scan backwards (destructive)
inline int prev_bit_del				(); 
inline int prev_bit_del				(int& nBB);

//////////////////
// data members

protected:
	 scan_t scan_;
};

///////////////////////
// Inline implementations, necessary in header file

inline int BBScanSp::next_bit() {
////////////////////////////
// date:5/9/2014
// non destructive bitscan for sparse bitstrings using intrinsics
// caches index in the collection and pos inside the bitblock
//
// comments
// 1-require previous assignment scan_.bbi=0 and scan_.pos=mask_lim

	unsigned long posbb;
			
	//search for next bit in the last block
	if(_BitScanForward64(&posbb, vBB_[scan_.bbi_].bb_ & Tables::mask_high[scan_.pos_])){
		scan_.pos_ =posbb;
		return (posbb + WMUL(vBB_[scan_.bbi_].idx_));
	}else{											//search in the remaining blocks
		for(int i=scan_.bbi_+1; i<vBB_.size(); i++){
			if(_BitScanForward64(&posbb,vBB_[i].bb_)){
				scan_.bbi_=i;
				scan_.pos_=posbb;
				return (posbb+ WMUL(vBB_[i].idx_));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBScanSp::next_bit(int& block_index) {
////////////////////////////
// date:5/9/2014
// non destructive bitscan for sparse bitstrings using intrinsics
// caches index in the collection and pos inside the bitblock
//
// comments
// 1-require previous assignment scan_.bbi=0 and scan_.pos_=mask_lim

	unsigned long posbb;
			
	//search for next bit in the last block
	if(_BitScanForward64(&posbb, vBB_[scan_.bbi_].bb_ & Tables::mask_high[scan_.pos_])){
		scan_.pos_ =posbb;
		block_index= vBB_[scan_.bbi_].idx_;
		return (posbb + WMUL(vBB_[scan_.bbi_].idx_));
	}else{											//search in the remaining blocks
		for(int i=scan_.bbi_+1; i<vBB_.size(); i++){
			if(_BitScanForward64(&posbb,vBB_[i].bb_)){
				scan_.bbi_=i;
				scan_.pos_=posbb;
				block_index=vBB_[i].idx_;
				return (posbb+ WMUL(vBB_[i].idx_));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBScanSp::prev_bit	() {
////////////////////////////
// date:5/9/2014
// Non destructive bitscan for sparse bitstrings using intrinsics
// caches index in the collection and pos inside the bitblock
//
// comments
// 1-require previous assignment scan_.bbi=number of bitblocks-1 and scan_.pos_=WORD_SIZE

	unsigned long posbb;
				
	//search int the last table
	if(_BitScanReverse64(&posbb, vBB_[scan_.bbi_].bb_ & Tables::mask_low[scan_.pos_])){
		scan_.pos_ =posbb;
		return (posbb + WMUL(vBB_[scan_.bbi_].idx_));
	}else{											//not found in the last table. search in the rest
		for(int i=scan_.bbi_-1; i>=0; i--){
			if(_BitScanReverse64(&posbb,vBB_[i].bb_)){
				scan_.bbi_=i;
				scan_.pos_=posbb;
				return (posbb+ WMUL(vBB_[i].idx_));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBScanSp::next_bit_del() {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan (scans and deletes each 1-bit) for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment scan_.bbi=0 

	unsigned long posbb;

	for(int i=scan_.bbi_; i<vBB_.size(); i++)	{
		if(_BitScanForward64(&posbb,vBB_[i].bb_)){
			scan_.bbi_=i;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb + WMUL(vBB_[i].idx_));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBScanSp::next_bit_del(int& block_index) {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment scan_.bbi=0 

	unsigned long posbb;

	for(int i=scan_.bbi_; i<vBB_.size(); i++)	{
		if(_BitScanForward64(&posbb,vBB_[i].bb_)){
			scan_.bbi_=i;
			block_index=vBB_[i].idx_;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb + WMUL(vBB_[i].idx_));
		}
	}
	
return EMPTY_ELEM;  
}


inline int BBScanSp::next_bit_del(int& block_index, BBScanSp& bbN_del) {
	////////////////////////////
	//
	// date: 06/07/2019 (EXPERIMENTAL)
	// Destructive bitscan for sparse bitstrings using intrinsics
	//
	// COMMENTS
	// 1-added for compatibility with BBIntrin
	// 1-Requires previous assignment scan_.bbi=0 

	unsigned long posbb;

	for (int i = scan_.bbi_; i < vBB_.size(); i++) {
		if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
			scan_.bbi_ = i;
			block_index = vBB_[i].idx_;
			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
			bbN_del.vBB_[i].bb_ &= ~Tables::mask[posbb];
			return (posbb + WMUL(vBB_[i].idx_));
		}
	}

	return EMPTY_ELEM;
}

inline int BBScanSp::next_bit_del_pos (int& posBB){
////////////////////////////
//
// date: 29/10/14
// Destructive bitscan which returns the position of the bitblock scanned in the collection
// (not the index attribute)
//
// COMMENTS
// 1-Requires previous assignment scan_.bbi=0 

	unsigned long posbb;

	for(int i=scan_.bbi_; i<vBB_.size(); i++)	{
		if(_BitScanForward64(&posbb,vBB_[i].bb_)){
			posBB=scan_.bbi_=i;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb + WMUL(vBB_[i].idx_));
		}
	}
	
return EMPTY_ELEM;  
}


inline int BBScanSp::prev_bit_del() {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment scan_.bbi=number of bitblocks-1

	unsigned long posbb;

	for(int i=scan_.bbi_; i>=0; i--){
		if(_BitScanReverse64(&posbb,vBB_[i].bb_)){
			scan_.bbi_=i;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb+WMUL(vBB_[i].idx_));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBScanSp::prev_bit_del(int & bb_index) {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment scan_.bbi=number of bitblocks-1

	unsigned long posbb;

	for(int i=scan_.bbi_; i>=0; i--){
		if(_BitScanReverse64(&posbb,vBB_[i].bb_)){
			scan_.bbi_=i;
			bb_index=vBB_[i].idx_;
			vBB_[i].bb_&=~Tables::mask[posbb];			//deleting before the return
			return (posbb+WMUL(vBB_[i].idx_));
		}
	}

	return BBObject::noBit;
}

inline
int BBScanSp::init_scan (scan_types sct){
	if (vBB_.empty()) { return BBObject::noBit; }			//necessary check since sparse bitstrings have empty semantics (i.e. sparse graphs)

	switch(sct){
	case NON_DESTRUCTIVE:
		scan_block(0);
		scan_bit(MASK_LIM);
		break;
	case NON_DESTRUCTIVE_REVERSE:
		scan_block(vBB_.size()-1);
		scan_bit(WORD_SIZE);
		break;
	case DESTRUCTIVE:
		scan_block(0);
		break;
	case DESTRUCTIVE_REVERSE:
		scan_block(vBB_.size() - 1);
		break;
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}

inline
int BBScanSp::init_scan_from (int from, scan_types sct){
////////////////////////
// scans starting at from until the end of the bitarray
//
// REMARKS: at the moment, only working for the NON-DESTRUCTIVE case

	pair<bool, int> p= find_block_pos(WDIV(from));
	if(p.second == BBObject::noBit) return BBObject::noBit;
	switch(sct){
	case NON_DESTRUCTIVE:
	case NON_DESTRUCTIVE_REVERSE:
		scan_block(p.second); 
		(p.first)? scan_bit(WMOD(from)) : scan_bit(MASK_LIM);
		break;
	/*case DESTRUCTIVE:
	case DESTRUCTIVE_REVERSE:
		scan_block(p.second);
		break;*/
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}


#endif 