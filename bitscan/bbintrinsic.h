 /**
  * @file bbintrinsic.h file
  * @brief header file of the BBIntrin class from the BITSCAN library.
  *		  Manages efficient bitscanning 
  * @author pss
  **/

#ifndef __BBINTRINSIC_H__
#define __BBINTRINSIC_H__

#include "bitset.h"	

using namespace std;


/////////////////////////////////
//
// Class BBIntrin
// 
// (Efficient bit scanning)
// 
// TODO - rename 08/02/2025
//
///////////////////////////////////

class BBIntrin: public BitSet{
public:

	template<typename U>
	friend class DirectRevScan;

	template<typename U>
	friend class DirectScan;

	template<typename U>
	friend class DestructiveScan;

//constructors / destructors
	
	using BitSet::BitSet;			//inherit constructors	

	virtual ~BBIntrin				() = default;

///////////////////////////////
//setters and getters
	
	void set_scan_block				(int bbindex)		{ scan_.bbi = bbindex;}	
	void set_scan_bit				(int posbit)		{ scan_.pos = posbit;}	
 	
//////////////////////////////
// Bitscanning (with cached info)

	/**
	* @brief least significant bit in the bitstring
	* @details 30/3/12, last update 08/02/2025
	* @details implemented with intrinsic functions
	**/
inline int lsbn64					() const override;

	/**
	* @brief most significant bit in the bitstring
	* @details 30/3/12, last update 08/02/2025
	* @details implemented with intrinsic functions
	**/
inline int msbn64					() const override; 
	
	/**
	* @brief Configures the initial block and bit position for bitscanning
	*		 according to one of the 4 scan types passed as argument
	* @param sc: type of scan
	* @returns 0 if successful, -1 otherwise
	**/
virtual	int init_scan				(scan_types sc);	

	/**
	* @brief Configures the initial block and bit position for bitscanning
	*		 starting from the bit 'firstBit' onwards, excluding 'firstBit'
	*		 according to one of the 4 scan types passed as argument.
	*		 If firstBit is -1 (EMPTY_ELEM), the scan starts from the beginning.
	* @param firstBit: starting bit
	* @param sc: type of scan
	* @returns 0 if successful, -1 otherwise
	* 
	* TODO - no firstBit information is configured for DESTRUCTIVE scan types (08/02/2025) 
	**/
	int init_scan					(int firstBit, scan_types);


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
	* @returns the next bit in the bitstring, EMPTY_ELEM if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	**/
virtual inline int next_bit_del		(); 	

	/**
	* @brief next bit in the bitstring, starting from the block
	*		 in the last call to next_bit. 
	*		 Returns in block the current bitblock  of the returned bit.
	*		 Scan type: destructive
	*
	*		 I. caches the current block for the next call
	*		II. erases the current scanned bit
	* 		III. First call requires initialization with init_scan(DESTRUCTIVE)
	* 
	* @param block: output parameter with the current bitblock
	* @returns the next bit in the bitstring, EMPTY_ELEM if there are no more bits
	**/
virtual inline int next_bit_del		(int& block ); 

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
	* @param bitset: bitblock where the scanned bit is erased
	* @param block: output parameter with the current bitblock
	* @returns the next bit in the bitstring, EMPTY_ELEM if there are no more bits
	**/
virtual inline int next_bit_del		(int& nBB,  BBIntrin& bitset); 	 

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
	* @returns the next bit in the bitstring, EMPTY_ELEM if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
virtual inline int next_bit			();

	/**
	* @brief next bit in the bitstring, starting from the bit retrieved
	*		 in the last call to next_bit.
	*		 Returns in block the current bitblock  of the returned bit.
	*		 Scan type: non-destructive
	*
	*		 I. caches the current block for the next call
	*		II. DOES NOT erase the current scanned bit
	*		III. caches the scanned bit for the next call
	* 		IV. First call requires initialization with init_scan(NON-DESTRUCTIVE)
	* 
	* @param block: output bitblock of the retrieved bit
	* @returns the next bit in the bitstring, EMPTY_ELEM if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
virtual inline int next_bit			(int& block);

	/**
	* @brief next bit in the bitstring, starting from the bit retrieved
	*		 in the last call to next_bit.
	*		 Returns in block the current bitblock of the returned bit and deletes from
	*		 the input bitset the current bit scanned (returned).
	*		 Erases the current scanned bit from the bitblock bitset passed as argument
	*		 Scan type: non-destructive
	*
	*		 I. caches the current block for the next call
	*		II. DOES NOT erase the current scanned bit
	*		III.caches the scanned bit for the next call
	* 		IV. First call requires initialization with init_scan(NON-DESTRUCTIVE)
	*
	* @param bitset: bitblock where the scanned bit is erased
	* @param block: output parameter with the current bitblock
	* @returns the next bit in the bitstring, EMPTY_ELEM if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
virtual	inline int next_bit			(int& block, BBIntrin& bitset);

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
	* @returns the next lsb bit in the bitstring, EMPTY_ELEM if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
virtual inline int prev_bit			(); 

	/**
	* @brief next least-significant bit in the bitstring, starting from the bit retrieved
	*		 in the last call to next_bit.
	*		 Returns in block the current bitblock  of the returned bit.	
	*		 Scan type: non-destructive, reverse
	*
	*		 I. caches the current block for the next call
	*		II. caches the scanned bit for the next call
	* 		III. First call requires initialization with init_scan(NON-DESTRUCTIVE, REVERSE)
	* 
	* @param block: output bitblock of the retrieved bit
	* @returns the next lsb bit in the bitstring, EMPTY_ELEM if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	* @details Since the scan does not delete the scanned bit from the bitstring,
	*		   it has to cache the last scanned bit for the next call
	**/
virtual inline int prev_bit			(int& block);

	/**
	* @brief next least-significant bit in the bitstring, starting from the bit retrieved
	*		 in the last call to next_bit.
	*		 Scan type: destructive, reverse
	*
	*		 I. caches the current block for the next call
	*		II. erases the current scanned bit
	* 		III. First call requires initialization with init_scan(DESTRUCTIVE, REVERSE)
	*
	* @returns the next lsb bit in the bitstring, EMPTY_ELEM if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	**/
 virtual inline int prev_bit_del	(); 

	 /**
	 * @brief next least-significant bit in the bitstring, starting from the bit retrieved
	 *		 in the last call to next_bit.
	 *		 Returns in block the current bitblock  of the returned bit.
	 *		 Scan type: destructive, reverse
	 *
	 *		 I. caches the current block for the next call
	 *		II. erases the current scanned bit
	 * 		III. First call requires initialization with init_scan(DESTRUCTIVE, REVERSE)
	 *
	 * @param block: output bitblock of the retrieved bit
	 * @returns the next lsb bit in the bitstring, EMPTY_ELEM if there are no more bits
	 * @details Created   23/3/12, last update 09/02/2025
	 **/
inline int prev_bit_del				(int& block);

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
	* @returns the next lsb bit in the bitstring, EMPTY_ELEM if there are no more bits
	* @details Created   23/3/12, last update 09/02/2025
	**/
inline int prev_bit_del				(int& block,  BBIntrin& bitset ); 

//////////
//conversions (requiring bitscanning)

	/**
	* @brief Converts the bitstring to a C array of integers
	* @param lv: output array
	* @param size: output size of the array (number of bits in the bitstring)
	* @param rev: if true, the bits are stored in reverse order
	* @returns the C-array of integers
	* @details Created  23/3/12, last update 09/02/2025
	* @details Uses non-destructive bitscanning
	* @detials overrides the virtual function in the base class using the intrinsic bitscanning
	**/
inline	int* to_C_array				(int* lv, std::size_t& size, bool rev = false) override;

#ifdef POPCOUNT_64

/////////////////
// Popcount with intrinsifc primitives 

	/**
	* @brief returns the number of 1-bits in the bitstring
	* @details implemented with intrinsic functions
	**/
virtual	 inline int popcn64			()						const;

	/**
	* @brief returns the number of 1-bits in the bitstring in the closed range [firstBit, END]
	* @param firstBit: first bit to consider in the count, included
	* @details implemented with intrinsic functions
	**/
virtual	 inline int popcn64			(int firstBit)			const;

#endif

//////////////////
/// data members
	 scan_t scan_;
};

///////////////////////
//
// INLINE Implementation, must be in header file


#ifdef POPCOUNT_64

inline 
int BBIntrin::popcn64() const{

	BITBOARD pc = 0;

	for(auto i = 0; i < nBB_; ++i){
		pc += __popcnt64(vBB_[i]);
	}

	return pc;
}

inline
int BBIntrin::popcn64(int firstBit) const{
	
	BITBOARD pc = 0;
	
	auto bb = WDIV(firstBit);

	//population of the block of the bit
	pc += __popcnt64( vBB_[bb] & bblock::MASK_1_HIGH(firstBit - WMUL(bb) /* WMOD(bb)*/ ) );


	//population after the block of the bit
	for(int i= bb + 1; i < nBB_; ++i){
		pc += __popcnt64(vBB_[i]);
	}
		

	return pc;
}

#endif


inline
int BBIntrin::msbn64() const {

	U32 posInBB;

	for (auto i = nBB_ - 1; i >= 0; --i) {

		if (_BitScanReverse64(&posInBB, vBB_[i])) {
			return (posInBB + WMUL(i));
		}
	}

	return EMPTY_ELEM;
}


inline
int BBIntrin::lsbn64() const {

	U32 posInBB;

	for (auto i = 0; i < nBB_; ++i) {
		if (_BitScanForward64(&posInBB, vBB_[i])) {
			return(posInBB + WMUL(i));
		}
	}

	return EMPTY_ELEM;
}



inline
int BBIntrin::next_bit_del() {

	 U32 posInBB;

	for(auto i = scan_.bbi; i < nBB_; ++i)	{

		if(_BitScanForward64(&posInBB, vBB_[i])){
			//stores the current block
			scan_.bbi = i;

			//deletes the current bit before returning
			vBB_[i] &= ~Tables::mask[posInBB];			

			return (posInBB + WMUL(i));
		}

	}
	
	return EMPTY_ELEM;  
}

inline 
int BBIntrin::next_bit_del(int& block) {

	U32 posInBB;

	for(auto i = scan_.bbi; i < nBB_; ++i)	{

		if(_BitScanForward64(&posInBB,vBB_[i])){
			//stores the current block and copies to output
			scan_.bbi = i;
			block = i;

			//deletes the current bit before returning
			vBB_[i] &= ~Tables::mask[posInBB];			
			
			return (posInBB + WMUL(i));
		}
	}
	
	return EMPTY_ELEM;  
}

inline
int BBIntrin::next_bit_del(int& block, BBIntrin& bbN_del) {

	U32 posInBB;

	for(auto i = scan_.bbi; i < nBB_; ++i){

		if(_BitScanForward64(&posInBB,vBB_[i])){
			//stores the current block and copies to output
			scan_.bbi=i;
			block = i;

			//deletes the current bit before returning
			vBB_[i] &= ~Tables::mask[posInBB];				

			//erases from the bitset passed the scanned bit
			bbN_del.vBB_[i] &= ~Tables::mask[posInBB];
			
			return (posInBB+WMUL(i));
		}
	}
	
	return EMPTY_ELEM;  
}

inline
int BBIntrin::next_bit() {

	U32 posInBB;
			
	//Search for next bit in the last scanned block
	if( _BitScanForward64(&posInBB, vBB_[scan_.bbi] & Tables::mask_high[scan_.pos]) ){

		//stores the current bit for next call
		scan_.pos = posInBB;									//current block has not changed, so not stored

		return (posInBB + WMUL(scan_.bbi));

	}else{	

		//Searches for next bit in the remaining blocks
		for(auto i = scan_.bbi + 1; i < nBB_; ++i){
			if( _BitScanForward64(&posInBB,vBB_[i]) ){

				//stores the current block and bit for next call
				scan_.bbi = i;
				scan_.pos = posInBB;

				return (posInBB+ WMUL(i));
			}
		}
	}
	
	return EMPTY_ELEM;
}

inline
int BBIntrin::next_bit(int& block) {

	U32 posInBB;

	//Search for next bit in the last scanned block
	if (_BitScanForward64(&posInBB, vBB_[scan_.bbi] & Tables::mask_high[scan_.pos])) {

		//stores the current bit for next call
		scan_.pos = posInBB;									//current block has not changed, so not stored

		//outputs the current block
		block = scan_.bbi;

		return (posInBB + WMUL(scan_.bbi));
	}
	else {		

		//Searches for next bit in the remaining blocks
		for (int i = scan_.bbi + 1; i < nBB_; i++) {

			if (_BitScanForward64(&posInBB, vBB_[i])) {

				//stores the current block and bit for next call
				scan_.bbi = i;
				scan_.pos = posInBB;

				//outputs the current block
				block = i;

				return (posInBB + WMUL(i));
			}
		}
	}

	return EMPTY_ELEM;
}

inline
int BBIntrin::next_bit(int& block, BBIntrin& bitset) {

	U32 posInBB;

	//Search for next bit in the last scanned block
	if (_BitScanForward64(&posInBB, vBB_[scan_.bbi] & Tables::mask_high[scan_.pos])) {
		
		//stores the current bit for next call
		scan_.pos = posInBB;									//current block has not changed, so not stored	

		//outputs the current block
		block = scan_.bbi;

		//deletes the bit from the input bitset
		bitset.vBB_[scan_.bbi] &= ~Tables::mask[posInBB];

		return (posInBB + WMUL(scan_.bbi));
	}
	else {											
		//Searches for next bit in the remaining blocks
		for (auto i = scan_.bbi + 1; i < nBB_; i++) {
			if (_BitScanForward64(&posInBB, vBB_[i])) {

				//stores the current block and bit for next call
				scan_.bbi = i;
				scan_.pos = posInBB;

				//outputs the current block
				block = i;

				//deletes the bit from the input bitset
				bitset.vBB_[i] &= ~Tables::mask[posInBB];

				return (posInBB + WMUL(i));
			}
		}
	}

	return EMPTY_ELEM;
}

inline
int BBIntrin::prev_bit		() {

	U32 posInBB;

	//Searches for previous bit in the last scanned block
	if( _BitScanReverse64(&posInBB, vBB_[scan_.bbi] & Tables::mask_low[scan_.pos]) ){

		//stores the current bit for next call
		scan_.pos = posInBB;									//current block has not changed, so not stored			

		return (posInBB + WMUL(scan_.bbi));

	}else{		

		//Searches for previous bit in the remaining blocks
		for(auto i = scan_.bbi - 1; i >= 0; --i){

			if(_BitScanReverse64(&posInBB,vBB_[i])){

				//stores the current block and bit for next call
				scan_.bbi = i;
				scan_.pos = posInBB;

				return (posInBB + WMUL(i));
			}
		}
	}
	
	return EMPTY_ELEM;
}

inline
int BBIntrin::prev_bit	(int& block) {

	U32 posInBB;
				
	//Searches for previous bit in the last scanned block
	if( _BitScanReverse64(&posInBB, vBB_[scan_.bbi] & Tables::mask_low[scan_.pos]) ){

		//stores the current bit for next call
		scan_.pos = posInBB;									//current block has not changed, so not stored

		//outputs the current block
		block = scan_.bbi;

		return (posInBB + WMUL(scan_.bbi));

	}else{											

		//Searches for previous bit in the remaining blocks
		for(auto i = scan_.bbi - 1; i >= 0; --i){

			if(_BitScanReverse64(&posInBB,vBB_[i])){

				//stores the current block and bit for next call
				scan_.bbi=i;
				scan_.pos=posInBB;

				//outputs the current block
				block = i;

				return (posInBB+ WMUL(i));
			}
		}
	}
	
	return EMPTY_ELEM;
}

inline
int BBIntrin::prev_bit_del() {
 
	U32 posInBB;

	for(auto i = scan_.bbi; i >= 0; --i){

		if(_BitScanReverse64(&posInBB,vBB_[i])){

			//stores the current block for the next call
			scan_.bbi=i;

			//deletes the current bit from the bitset before returning
			vBB_[i] &= ~Tables::mask[posInBB];		

			return (posInBB+WMUL(i));
		}
	}
	return EMPTY_ELEM;  
}

inline
int BBIntrin::prev_bit_del(int& block) {

	U32 posInBB;

	for(auto i = scan_.bbi; i >= 0; --i){

		if(_BitScanReverse64(&posInBB,vBB_[i])){

			//stores the current block for the next call
			scan_.bbi=i;

			//deletes the current bit from the bitset before returning
			vBB_[i]&=~Tables::mask[posInBB];			

			//outputs the current block
			block = i;

			return (posInBB + WMUL(i));
		}
	}
	
	return EMPTY_ELEM;  
}

inline
int BBIntrin::prev_bit_del(int& block, BBIntrin& bitset) {

	U32 posInBB;

	for (auto i = scan_.bbi; i >= 0; --i) {

		if (_BitScanReverse64(&posInBB, vBB_[i])) {

			//stores the current block for the next call
			scan_.bbi = i;

			//deletes the current bit from the bitset before returning
			vBB_[i] &= ~Tables::mask[posInBB];			

			
			//outputs the current block
			block = i;

			//erases the bit from the input bitset
			bitset.vBB_[i] &= ~Tables::mask[posInBB];

			return (posInBB + WMUL(i));
		}
	}

	return EMPTY_ELEM;
}

inline
int BBIntrin::init_scan(scan_types sct){

	switch(sct){
	case NON_DESTRUCTIVE:
		set_scan_block	(0);
		set_scan_bit	(MASK_LIM);
		break;
	case NON_DESTRUCTIVE_REVERSE:
		set_scan_block	(nBB_ - 1);
		set_scan_bit	(WORD_SIZE);		//mask_low[WORD_SIZE] = ONE
		break;
	case DESTRUCTIVE:
		set_scan_block	(0); 
		break;
	case DESTRUCTIVE_REVERSE:
		set_scan_block	(nBB_ - 1);
		break;
	default:
		LOG_ERROR("unknown scan type - BBIntrin::::init_scan");
		return -1;
	}

	return 0;
}

inline
int BBIntrin::init_scan (int firstBit, scan_types sct){

	if (firstBit == EMPTY_ELEM) {
		init_scan(sct);
	}
	else{
		int bbh = WDIV(firstBit);
		switch(sct){
		case NON_DESTRUCTIVE:
		case NON_DESTRUCTIVE_REVERSE:
			set_scan_block	(bbh);
			set_scan_bit	(firstBit - WMUL(bbh) /* WMOD(firstBit) */);
			break;
		case DESTRUCTIVE:
		case DESTRUCTIVE_REVERSE:
			set_scan_block	(bbh);
			break;
		default:
			LOG_ERROR("unknown scan type - BBIntrin::::init_scan");
			return -1;
		}
	}

	return 0;
}

inline
int* BBIntrin::to_C_array  (int* lv, std::size_t& nPop, bool rev)  {

	int bit = EMPTY_ELEM;
	nPop = 0;

	if (rev) {
		init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	}
	else {
		init_scan(BBObject::NON_DESTRUCTIVE);
	}
	
	//bitscanning loop
	while( (bit = next_bit())!= EMPTY_ELEM ){
		lv[nPop ++] = bit;
	}

	return lv;
}

#endif






