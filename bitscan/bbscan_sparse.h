/**
 * @file bbscan_sparse.h
 * @brief header for sparse class equivalent to the BBScan class
 *		  manages sparse bit strings with efficient bitscanning capabilities
 * @author pss
 * @details: created 2012, last_update 25/02/2025
 *
 * TODO refactoring and testing 25/02/2025
 **/

#ifndef __BBSCAN_SPARSE_H__
#define __BBSCAN_SPARSE_H__

#include "bitscan/bbset_sparse.h"
using namespace std;

/////////////////////////////////
//
// Class BBScanSp
// (Uses a number of optimizations for sparse bit scanning)
//
///////////////////////////////////

class BBScanSp: public BitSetSp{

public:

	template <class U>
	friend struct BBObject::Scan;
	template <class U>
	friend struct BBObject::ScanDest;
	template <class U>
	friend struct BBObject::ScanRev;
	template <class U>
	friend struct BBObject::ScanDestRev;

public:

	//aliases for bitscanning 
	using scan = typename BBObject::Scan<BBScanSp>;
	using scanR = typename BBObject::ScanRev<BBScanSp>;
	using scanD = typename BBObject::ScanDest<BBScanSp>;
	using scanDR = typename BBObject::ScanDestRev<BBScanSp>;

public:	

//////////////////////////////
//construction / destruction

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

	/**
	* @brief Configures the initial block and bit position for bitscanning
	*		 according to one of the 4 scan types passed as argument
	* @param sct: type of scan
	* @returns 0 if successful, -1 otherwise, or if the bitset is empty
	* @details : sparse bitsets may have no blocks, in which case the scan is not possible and
	*		     the function returns -1
	**/
	int init_scan					(scan_types sct);
	
	/**
	* @brief Configures the initial block and bit position for bitscanning
	*		 starting from the bit 'firstBit' onwards, excluding 'firstBit'
	*		 according to one of the 4 scan types passed as argument 
	*		 (currently ONLY for the NON-DESTRUCTIVE cases).
	*		 If firstBit is -1 (BBObject::noBit), the scan starts from the beginning.
	* @param firstBit: starting bit
	* @param sct: type of scan
	* @returns 0 if successful, -1 otherwise or if the bitset is empty
	* @details : sparse bitsets may have no blocks, in which case the scan is not possible and
	*		     the function returns -1
	* 
	* TODO - extend to NON-DESTRUCTIVE cases
	**/
	int init_scan					(int firstBit, scan_types sct);
	
	////////////////
	// bitscan forward

	/**
	* @brief next bit in the bitstring, starting from the block
	*		 in the last call to next_bit.
	*		 Scan type: destructive
	*
	*		 I. caches the current block for the next call
	*		II. erases the current scanned bit
	*		III. First call requires initialization with init_scan(DESTRUCTIVE)
	*
	* @returns the next bit in the bitstring, BBObject::noBit if there are no more bits
	* @details created 2015, last update 25/02/2025
	**/
inline int next_bit_del				(); 
inline int next_bit_del				(BBScanSp& bitset);

	/**
	* @brief next bit in the bitstring, starting from the bit retrieved
	*		 in the last call to next_bit.
	*		 Scan type: non-destructive
	*
	*		 I. caches the current block for the next call
	*		II. DOES NOT erase the current scanned bit
	*		III. caches the scanned bit for the next call
	* 		IV. First call requires initialization with init_scan(NON-DESTRUCTIVE)
	*
	* @returns the next bit in the bitstring, BBObject::noBit if there are no more bits
	* @details Created 5/9/2014, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
inline int next_bit					();
inline int next_bit					(BBScanSp& bitset);
	
	/**
	* @brief for basic bitscanning - they are hidden by next_bit()
	**/
	using BitSetSp::next_bit;	
	
	////////////////
	// bitscan backwards

	/**
	* @brief next least-significant bit in the bitstring, starting from the bit retrieved
	*		 in the last call to next_bit.
	*		 Scan type: destructive, reverse
	*
	*		 I. caches the current block for the next call
	*		II. erases the current scanned bit
	* 		III. First call requires initialization with init_scan(DESTRUCTIVE, REVERSE)
	*
	* @returns the next lsb bit in the bitstring, BBObject::noBit if there are no more bits
	* @details Created 23/3/12, last update 09/02/2025
	**/
inline int prev_bit_del				();
inline int prev_bit_del				(BBScanSp& bitset);
	/**
	* @brief next least-significant bit in the bitstring, starting from the bit retrieved
	*		 in the last call to next_bit.
	*		 Scan type: non-destructive, reverse
	*
	*		 I. caches the current block for the next call
	*		II. caches the scanned bit for the next call
	* 		III. First call requires initialization with init_scan(NON-DESTRUCTIVE, REVERSE)
	*
	* @returns the next lsb bit in the bitstring, BBObject::noBit if there are no more bits
	* @details Created 5/9/2014, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
 inline int prev_bit				(); 
 inline int prev_bit				(BBScanSp& bitset);

	/////////////////
	//	DEPRECATED

//inline int next_bit_del				(int& nBB);								//nBB: index of bitblock in the bitstring	(not in the collection)	
//inline int next_bit_del				(int& nBB, BBScanSp& );					//EXPERIMENTAL! 
//inline int next_bit_del_pos			(int& posBB);							//posBB: position of bitblock in the collection (not the index of the element)		
//inline int next_bit					(int& nBB);								//nBB: index of bitblock in the bitstring	(not in the collection)				
//inline int prev_bit_del				(int& nBB);

//////////////////
// data members
protected:
	 scan_t scan_;
};

///////////////////////
// Inline implementations, necessary in header file

inline int BBScanSp::next_bit() {

	U32 posInBB;
			
	//search for next bit in the last block
	if(_BitScanForward64(&posInBB, vBB_[scan_.bbi_].bb_ & Tables::mask_high[scan_.pos_])){

		//stores the current bit for next call
		scan_.pos_ = posInBB;

		return (posInBB + WMUL(vBB_[scan_.bbi_].idx_));

	}else{	

		//Searches for next bit in the remaining blocks
		for(auto i= scan_.bbi_ + 1; i < vBB_.size(); ++i){
			if(_BitScanForward64(&posInBB,vBB_[i].bb_)){
				//stores the current block and bit for next call
				scan_.bbi_=i;
				scan_.pos_= posInBB;

				return (posInBB + WMUL(vBB_[i].idx_));
			}
		}
	}
	
	return BBObject::noBit;
	
}

inline int BBScanSp::next_bit(BBScanSp& bitset)
{
	U32 posInBB;

	//search for next bit in the last block
	if (_BitScanForward64(&posInBB, vBB_[scan_.bbi_].bb_ & Tables::mask_high[scan_.pos_])) {

		//stores the current bit for next call
		scan_.pos_ = posInBB;

		//delete the bit from the input bitset
		bitset.erase_bit(posInBB);

		return (posInBB + WMUL(vBB_[scan_.bbi_].idx_));

	}
	else {

		//Searches for next bit in the remaining blocks
		for (auto i = scan_.bbi_ + 1; i < vBB_.size(); ++i) {
			if (_BitScanForward64(&posInBB, vBB_[i].bb_)) {
				//stores the current block and bit for next call
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;

				//delete the bit from the input bitset
				bitset.erase_bit(posInBB);

				return (posInBB + WMUL(vBB_[i].idx_));
			}
		}
	}

	return BBObject::noBit;
}

inline int BBScanSp::prev_bit () {

	U32 posInBB;
				
	//searches for previous bit in the last scanned block
	if(_BitScanReverse64(&posInBB, vBB_[scan_.bbi_].bb_ & Tables::mask_low[scan_.pos_])){
		//stores the current bit for next call
		scan_.pos_ = posInBB;

		return (posInBB + WMUL(vBB_[scan_.bbi_].idx_));
	}else{	
		//Searches for previous bit in the remaining blocks
		for(auto i = scan_.bbi_ - 1; i >= 0; --i){

			if(_BitScanReverse64(&posInBB,vBB_[i].bb_)){

				//stores the current block and bit for next call
				scan_.bbi_=i;
				scan_.pos_= posInBB;

				return (posInBB + WMUL(vBB_[i].idx_));
			}
		}
	}
	
	return BBObject::noBit;

}

inline int BBScanSp::prev_bit(BBScanSp& bitset)
{
	U32 posInBB;

	//searches for previous bit in the last scanned block
	if (_BitScanReverse64(&posInBB, vBB_[scan_.bbi_].bb_ & Tables::mask_low[scan_.pos_])) {
		//stores the current bit for next call
		scan_.pos_ = posInBB;

		//delete the bit from the input bitset
		bitset.erase_bit(posInBB);

		return (posInBB + WMUL(vBB_[scan_.bbi_].idx_));
	}
	else {
		//Searches for previous bit in the remaining blocks
		for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i].bb_)) {

				//stores the current block and bit for next call
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;

				//delete the bit from the input bitset
				bitset.erase_bit(posInBB);

				return (posInBB + WMUL(vBB_[i].idx_));
			}
		}
	}

	return BBObject::noBit;
}


inline int BBScanSp::next_bit_del() {

	U32 posInBB;

	for(auto  i = scan_.bbi_; i < vBB_.size(); ++i)	{

		if(_BitScanForward64(&posInBB, vBB_[i].bb_)){

			//stores the current block
			scan_.bbi_=i;

			//deletes the current bit before returning
			vBB_[i].bb_&=~Tables::mask[posInBB];			
		
			return (posInBB + WMUL(vBB_[i].idx_));
		}

	}
	
	return BBObject::noBit;

}

inline int BBScanSp::next_bit_del(BBScanSp& bitset)
{
	U32 posInBB;

	for (auto i = scan_.bbi_; i < vBB_.size(); ++i) {

		if (_BitScanForward64(&posInBB, vBB_[i].bb_)) {

			//stores the current block
			scan_.bbi_ = i;

			//deletes the current bit before returning
			vBB_[i].bb_ &= ~Tables::mask[posInBB];

			//delete the bit from the input bitset
			bitset.erase_bit(posInBB);

			return (posInBB + WMUL(vBB_[i].idx_));
		}

	}

	return BBObject::noBit;
}

inline int BBScanSp::prev_bit_del() {

	U32 posInBB;

	for(int i = scan_.bbi_; i >= 0; --i){

		if(_BitScanReverse64(&posInBB, vBB_[i].bb_)){

			//stores the current block for the next call
			scan_.bbi_=i;

			//deletes the current bit from the bitset before returning
			vBB_[i].bb_&=~Tables::mask[posInBB];			
			
			return (posInBB + WMUL(vBB_[i].idx_));
		}
	}
	
	return BBObject::noBit;
}

inline int BBScanSp::prev_bit_del(BBScanSp& bitset)
{
	U32 posInBB;

	for (int i = scan_.bbi_; i >= 0; --i) {

		if (_BitScanReverse64(&posInBB, vBB_[i].bb_)) {

			//stores the current block for the next call
			scan_.bbi_ = i;

			//deletes the current bit from the bitset before returning
			vBB_[i].bb_ &= ~Tables::mask[posInBB];

			//delete the bit from the input bitset
			bitset.erase_bit(posInBB);

			return (posInBB + WMUL(vBB_[i].idx_));
		}
	}

	return BBObject::noBit;
}



inline
int BBScanSp::init_scan (scan_types sct){
	if (vBB_.empty()) { return -1; }			//necessary check since sparse bitstrings have empty semantics (i.e. sparse graphs)

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
		LOG_ERROR("unknown scan type - BBScanSp::::init_scan");
		return -1;
	}

	return 0;	
}

inline
int BBScanSp::init_scan (int firstBit, scan_types sct){

	//necessary check 
	if (vBB_.empty()) {	return -1;	}

	//special case - first bitscan
	if (firstBit == BBObject::noBit) {
		return init_scan(sct);
	}
		

	//determine the index of the starting block (not its ID)
	auto bbL = WDIV(firstBit);
	
	////////////////////////////////////////////////////
	pair<bool, int> p = find_block_pos(bbL);
	///////////////////////////////////////////////////

	//no blocks with index greater or equal to bbL, nothing to scan
	if (p.second == BBObject::noBit) {
		return -1;
	}

	switch (sct) {
	case NON_DESTRUCTIVE:
	case NON_DESTRUCTIVE_REVERSE:
		scan_block(p.second);
		(p.first) ? scan_bit(firstBit - WMUL(bbL)) : scan_bit(MASK_LIM);
		break;
		/*case DESTRUCTIVE:
		case DESTRUCTIVE_REVERSE:
			scan_block(p.second);
			break;*/
	default:
		LOG_ERROR("unknown scan type - BBScan::init_scan");
		return -1;
	}
		
	//nothing to scan or error
	return 0;
}


////////////////////////////////
//
// DEPRECATED
//
//////////////////////////////////
//inline 
//int BBScanSp::next_bit_del(int& block_index) {
//	////////////////////////////
//	//
//	// date: 23/3/12
//	// Destructive bitscan for sparse bitstrings using intrinsics
//	//
//	// COMMENTS
//	// 1-Requires previous assignment scan_.bbi=0 
//
//	unsigned long posbb;
//
//	for (int i = scan_.bbi_; i < vBB_.size(); i++) {
//		if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
//			scan_.bbi_ = i;
//			block_index = vBB_[i].idx_;
//			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
//			return (posbb + WMUL(vBB_[i].idx_));
//		}
//	}
//
//	return EMPTY_ELEM;
//}
//
//
//inline int BBScanSp::next_bit_del(int& block_index, BBScanSp& bbN_del) {
//	////////////////////////////
//	//
//	// date: 06/07/2019 (EXPERIMENTAL)
//	// Destructive bitscan for sparse bitstrings using intrinsics
//	//
//	// COMMENTS
//	// 1-added for compatibility with BBIntrin
//	// 1-Requires previous assignment scan_.bbi=0 
//
//	unsigned long posbb;
//
//	for (int i = scan_.bbi_; i < vBB_.size(); i++) {
//		if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
//			scan_.bbi_ = i;
//			block_index = vBB_[i].idx_;
//			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
//			bbN_del.vBB_[i].bb_ &= ~Tables::mask[posbb];
//			return (posbb + WMUL(vBB_[i].idx_));
//		}
//	}
//
//	return EMPTY_ELEM;
//}
//
//inline int BBScanSp::next_bit_del_pos(int& posBB) {
//	////////////////////////////
//	//
//	// date: 29/10/14
//	// Destructive bitscan which returns the position of the bitblock scanned in the collection
//	// (not the index attribute)
//	//
//	// COMMENTS
//	// 1-Requires previous assignment scan_.bbi=0 
//
//	unsigned long posbb;
//
//	for (int i = scan_.bbi_; i < vBB_.size(); i++) {
//		if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
//			posBB = scan_.bbi_ = i;
//			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
//			return (posbb + WMUL(vBB_[i].idx_));
//		}
//	}
//
//	return EMPTY_ELEM;
//}

//inline int BBScanSp::next_bit(int& block_index) {
//	////////////////////////////
//	// date:5/9/2014
//	// non destructive bitscan for sparse bitstrings using intrinsics
//	// caches index in the collection and pos inside the bitblock
//	//
//	// comments
//	// 1-require previous assignment scan_.bbi=0 and scan_.pos_=mask_lim
//
//	unsigned long posbb;
//
//	//search for next bit in the last block
//	if (_BitScanForward64(&posbb, vBB_[scan_.bbi_].bb_ & Tables::mask_high[scan_.pos_])) {
//		scan_.pos_ = posbb;
//		block_index = vBB_[scan_.bbi_].idx_;
//		return (posbb + WMUL(vBB_[scan_.bbi_].idx_));
//	}
//	else {											//search in the remaining blocks
//		for (int i = scan_.bbi_ + 1; i < vBB_.size(); i++) {
//			if (_BitScanForward64(&posbb, vBB_[i].bb_)) {
//				scan_.bbi_ = i;
//				scan_.pos_ = posbb;
//				block_index = vBB_[i].idx_;
//				return (posbb + WMUL(vBB_[i].idx_));
//			}
//		}
//	}
//
//	return EMPTY_ELEM;
//}

//inline int BBScanSp::prev_bit_del(int& bb_index) {
//	////////////////////////////
//	//
//	// date: 23/3/12
//	// Destructive bitscan for sparse bitstrings using intrinsics
//	//
//	// COMMENTS
//	// 1-Requires previous assignment scan_.bbi=number of bitblocks-1
//
//	unsigned long posbb;
//
//	for (int i = scan_.bbi_; i >= 0; i--) {
//		if (_BitScanReverse64(&posbb, vBB_[i].bb_)) {
//			scan_.bbi_ = i;
//			bb_index = vBB_[i].idx_;
//			vBB_[i].bb_ &= ~Tables::mask[posbb];			//deleting before the return
//			return (posbb + WMUL(vBB_[i].idx_));
//		}
//	}
//
//	return BBObject::noBit;
//}


#endif 