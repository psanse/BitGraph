 /**
  * @file bbscan.h file
  * @brief header file of the BBScan class from the BITSCAN library.
  *		   Manages efficient bitscanning. An alternative is using the nested bitscanning classes in BBObject
  * @details Use external scanning feature for commodity. Use this class for specific tuning of bitscans
  * @details last_updated 13/02/2025
  * @author pss
  * 
  * TODO - Compare efficiency with nested bitscanning classes in BBObject (13/02/2025)
  **/

#ifndef __BBSCAN_H__
#define __BBSCAN_H__

#include "bbset.h"	

using namespace std;


/////////////////////////////////
//
// Class BBScan
// 
// (Efficient bit scanning)
// 
/////////////////////////////////

class BBScan: public BitSet{
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
	using scan = typename BBObject::Scan<BBScan>;
	using scanR = typename BBObject::ScanRev<BBScan>;
	using scanD = typename BBObject::ScanDest<BBScan>;
	using scanDR = typename BBObject::ScanDestRev<BBScan>;

//////////////////////////////
//construction / destruction
	
	//inherit constructors	
	using BitSet::BitSet;		
	
	//TODO...check copy and move assignments 
	
	~BBScan							()	 = default;

///////////////////////////////
//setters and getters
	
	void scan_block				(int bbindex)		{ scan_.bbi_ = bbindex;}	
	void scan_bit				(int posbit)		{ scan_.pos_ = posbit;}

	int  scan_block				()	 const			{ return scan_.bbi_; }	
	int  scan_bit				()	 const			{ return scan_.pos_; }
//////////////////////////////
// Bitscanning (with cached info)
	
	/**
	* @brief Configures the initial block and bit position for bitscanning
	*		 according to one of the 4 scan types passed as argument
	* @param sct: type of scan
	* @returns 0 if successful, -1 otherwise
	**/
virtual inline	int init_scan		(scan_types sct);

	/**
	* @brief Configures the initial block and bit position for bitscanning
	*		 starting from the bit 'firstBit' onwards, excluding 'firstBit'
	*		 according to one of the 4 scan types passed as argument.
	*		 If firstBit is -1 (BBObject::noBit), the scan starts from the beginning.
	* @param firstBit: starting bit
	* @param sct: type of scan
	* @returns 0 if successful, -1 otherwise
	* 
	* TODO - no firstBit information is configured for DESTRUCTIVE scan types (08/02/2025) 
	**/
inline	int init_scan				(int firstBit, scan_types sct);


	////////////////
	// bitscan forwards
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
	* @details Created   23/3/12, last update 09/02/2025
	**/
virtual inline  int next_bit_del	();


	/**
	* @brief next bit in the bitstring, starting from the block
	*		 in the last call to next_bit.
	*		 Returns in block the current bitblock of the returned bit and deletes from
	*		 the input bitset the current bit scanned (returned).
	*		 Erases the current scanned bit from the bitblock bitset passed as argument
	*		 Scan type: destructive
	*
	*		 I. caches the current block for the next call
	*		II. erases the current scanned bit
	* 		III. First call requires initialization with init_scan(DESTRUCTIVE)
	* 
	* @param bitset: a bitset from which the scanned bit is erased
	* @param block: output parameter with the current bitblock
	* @returns the next bit in the bitstring, BBObject::noBit if there are no more bits
	**/
virtual inline int next_bit_del		( BBScan& bitset);

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
	* @details Created   23/3/12, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
virtual	inline  int next_bit		();
	
	/**
	* @brief for basic bitscanning operatins - they are hidden by next_bit()
	**/
	using BitSet::next_bit;
	
	/**	
	* @brief next bit in the bitstring, starting from the bit retrieved
	*		 in the last call to next_bit.
	*		 Deletes from the input bitset the current bit scanned (returned).
	*		 Deletes the current scanned bit from the bitblock bitset passed as argument
	*		 Scan type: non-destructive
	*
	*		 I. caches the current block for the next call
	*		II. DOES NOT erase the current scanned bit
	*		III.caches the scanned bit for the next call
	* 		IV. First call requires initialization with init_scan(NON-DESTRUCTIVE)
	*
	* @param bitset: bitblock where the scanned bit is erased
	* @returns the next bit in the bitstring, BBObject::noBit if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
virtual inline	 int next_bit		( BBScan& bitset);

	////////////////
	// bitscan backwards
	
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
	* @details Created   23/3/12, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
virtual  inline int prev_bit		();
virtual	inline int prev_bit			(BBScan& bitset);


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
	* @details Created   23/3/12, last update 09/02/2025
	**/
 virtual inline	int prev_bit_del	();


	/**
	* @brief next least-significant bit in the bitstring, starting from the bit retrieved
	*		 in the last call to next_bit.
	*		 Returns in block the current bitblock of the returned bit and deletes from
	*		 the input bitset the current bit scanned (returned).
	*		 Erases the current scanned bit from the bitblock bitset passed as argument
	*		 Scan type: destructive, reverse
	*
	*		 I. caches the current block for the next call
	*		II. erases the current scanned bit
	* 		IV. First call requires initialization with init_scan(DESTRUCTIVE, REVERSE)
	* 
	* @param block: output bitblock of the retrieved bit
	* @param bitset: bitset from which the retrieved bit is erased
	* @returns the next lsb bit in the bitstring, BBObject::noBit if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	**/
 virtual inline int prev_bit_del	( BBScan& bitset);


//////////////////
/// data members
protected:
	 scan_t scan_;
};

///////////////////////
//
// INLINE Implementation, must be in header file



int BBScan::next_bit_del() {

	 U32 posInBB;

	for(auto i = scan_.bbi_; i < nBB_; ++i)	{

		if(_BitScanForward64(&posInBB, vBB_[i])){
			//stores the current block
			scan_.bbi_ = i;

			//deletes the current bit before returning
			vBB_[i] &= ~Tables::mask[posInBB];			

			return (posInBB + WMUL(i));
		}

	}
	
	return BBObject::noBit;  
}




int BBScan::next_bit_del( BBScan& bbN_del) {

	U32 posInBB;

	for(auto i = scan_.bbi_; i < nBB_; ++i){

		if(_BitScanForward64(&posInBB,vBB_[i])){
			//stores the current block and copies to output
			scan_.bbi_=i;
			
			//deletes the current bit before returning
			vBB_[i] &= ~Tables::mask[posInBB];				

			//erases from the bitset passed the scanned bit
			bbN_del.vBB_[i] &= ~Tables::mask[posInBB];
			
			return (posInBB+WMUL(i));
		}
	}
	
	return BBObject::noBit;  
}


int BBScan::next_bit() {

	U32 posInBB;
			
	//Search for next bit in the last scanned block
	if( _BitScanForward64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_]) ){

		//stores the current bit for next call
		scan_.pos_ = posInBB;									//current block has not changed, so not stored

		return (posInBB + WMUL(scan_.bbi_));

	}else{	

		//Searches for next bit in the remaining blocks
		for(auto i = scan_.bbi_ + 1; i < nBB_; ++i){
			if( _BitScanForward64(&posInBB,vBB_[i]) ){

				//stores the current block and bit for next call
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;

				return (posInBB+ WMUL(i));
			}
		}
	}
	
	return BBObject::noBit;
}



int BBScan::next_bit( BBScan& bitset) {

	U32 posInBB;

	//Search for next bit in the last scanned block
	if (_BitScanForward64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_high[scan_.pos_])) {
		
		//stores the current bit for next call
		scan_.pos_ = posInBB;									//current block has not changed, so not stored	

		//outputs the current block
		//block = scan_.bbi_;

		//deletes the bit from the input bitset
		bitset.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

		return (posInBB + WMUL(scan_.bbi_));
	}
	else {											
		//Searches for next bit in the remaining blocks
		for (auto i = scan_.bbi_ + 1; i < nBB_; i++) {
			if (_BitScanForward64(&posInBB, vBB_[i])) {

				//stores the current block and bit for next call
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;

				//outputs the current block
				//block = i;

				//deletes the bit from the input bitset
				bitset.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}
	}

	return BBObject::noBit;
}


int BBScan::prev_bit		() {

	U32 posInBB;

	//Searches for previous bit in the last scanned block
	if( _BitScanReverse64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_low[scan_.pos_]) ){

		//stores the current bit for next call
		scan_.pos_ = posInBB;									//current block has not changed, so not stored			

		return (posInBB + WMUL(scan_.bbi_));

	}else{		

		//Searches for previous bit in the remaining blocks
		for(auto i = scan_.bbi_ - 1; i >= 0; --i){

			if(_BitScanReverse64(&posInBB,vBB_[i])){

				//stores the current block and bit for next call
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;

				return (posInBB + WMUL(i));
			}
		}
	}
	
	return BBObject::noBit;
}


int BBScan::prev_bit(BBScan& bitset)
{

	U32 posInBB;

	//Searches for previous bit in the last scanned block
	if (_BitScanReverse64(&posInBB, vBB_[scan_.bbi_] & Tables::mask_low[scan_.pos_])) {

		//stores the current bit for next call
		scan_.pos_ = posInBB;									//current block has not changed, so not stored			
		
		//deletes the bit from the input bitset
		bitset.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

		return (posInBB + WMUL(scan_.bbi_));

	}
	else {

		//Searches for previous bit in the remaining blocks
		for (auto i = scan_.bbi_ - 1; i >= 0; --i) {

			if (_BitScanReverse64(&posInBB, vBB_[i])) {

				//stores the current block and bit for next call
				scan_.bbi_ = i;
				scan_.pos_ = posInBB;

				//deletes the bit from the input bitset
				bitset.vBB_[scan_.bbi_] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}
	}

	return BBObject::noBit;
}



int BBScan::prev_bit_del() {
 
	U32 posInBB;

	for(auto i = scan_.bbi_; i >= 0; --i){

		if(_BitScanReverse64(&posInBB,vBB_[i])){

			//stores the current block for the next call
			scan_.bbi_=i;

			//deletes the current bit from the bitset before returning
			vBB_[i] &= ~Tables::mask[posInBB];		

			return (posInBB+WMUL(i));
		}
	}
	return BBObject::noBit;  
}



int BBScan::prev_bit_del( BBScan& bitset) {

	U32 posInBB;

	for (auto i = scan_.bbi_; i >= 0; --i) {

		if (_BitScanReverse64(&posInBB, vBB_[i])) {

			//stores the current block for the next call
			scan_.bbi_ = i;

			//deletes the current bit from the bitset before returning
			vBB_[i] &= ~Tables::mask[posInBB];			

			
			//outputs the current block
			//block = i;

			//erases the bit from the input bitset
			bitset.vBB_[i] &= ~Tables::mask[posInBB];

			return (posInBB + WMUL(i));
		}
	}

	return BBObject::noBit;
}


int BBScan::init_scan(scan_types sct){

	switch(sct){
	case NON_DESTRUCTIVE:
		scan_block	(0);
		scan_bit	(MASK_LIM);
		break;
	case NON_DESTRUCTIVE_REVERSE:
		scan_block	(nBB_ - 1);
		scan_bit	(WORD_SIZE);		//mask_low[WORD_SIZE] = ONE
		break;
	case DESTRUCTIVE:
		scan_block	(0); 
		break;
	case DESTRUCTIVE_REVERSE:
		scan_block	(nBB_ - 1);
		break;
	default:
		LOG_ERROR("unknown scan type - BBScan::::init_scan");
		return -1;
	}

	return 0;
}


int BBScan::init_scan (int firstBit, scan_types sct){

	//special case - first bitscan
	if (firstBit == BBObject::noBit) {
		return init_scan(sct);
	}

	
	int bbh = WDIV(firstBit);
	switch (sct) {
	case NON_DESTRUCTIVE:
	case NON_DESTRUCTIVE_REVERSE:
		scan_block(bbh);
		scan_bit(firstBit - WMUL(bbh) /* WMOD(firstBit) */);
		break;
	case DESTRUCTIVE:
	case DESTRUCTIVE_REVERSE:
		scan_block(bbh);
		break;
	default:
		LOG_ERROR("unknown scan type - BBScan::init_scan");
		return -1;
	}

	return 0;
}






#endif






