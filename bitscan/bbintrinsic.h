/*  
 * bbintrinsic.h file from the BITSCAN library, a C++ library for bit set
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

#pragma once

#include "bitset.h"	
#include <vector>			//I/O

using namespace std;


/////////////////////////////////
//
// Class BBIntrin
// (Uses a number of optimizations for bit scanning)
//
///////////////////////////////////

class BBIntrin: public BitSet{
public:	

//constructors / destructors
	
	using BitSet::BitSet;			//inherit constructors	

	virtual ~BBIntrin				() = default;
	
//setters and getters
	 void set_bbindex				(int bbindex)		{ m_scan.bbi=bbindex;}	
	 void set_posbit				(int posbit)		{ m_scan.pos=posbit;}	
 
	
//////////////////////////////
// bitscanning

inline virtual int lsbn64			() const;
inline virtual int msbn64			() const; 


	//bit scan forward (destructive)
virtual	int init_scan				(scan_types);	
virtual	int init_scan_from			(int from, scan_types);

virtual inline int next_bit_del		(); 												
virtual inline int next_bit_del		(int& nBB /* table index*/); 
virtual inline int next_bit_del		(int& nBB,  BBIntrin& bbN_del); 	 


	//bit scan forward (non destructive)
virtual inline int next_bit			();

	//bit scan backwards (non destructive)
virtual inline int prev_bit		(); 
virtual inline int prev_bit		(int& nBB);

	//bit scan backwards (destructive)
 virtual inline int prev_bit_del		(); 


virtual inline int next_bit			(int &); 
virtual	inline int next_bit			(int &,  BBIntrin& ); 


inline int prev_bit_del			(int& nBB);
inline int prev_bit_del			(int& nBB,  BBIntrin& del ); 

//////////
//conversions

inline	int* to_C_array				(int* lv, std::size_t& size, bool rev= false) override;

/////////////////
// Popcount - CHECK if required (08/02/2025)

#ifdef POPCOUNT_64
virtual	 inline int popcn64				()						const;
virtual	 inline int popcn64				(int nBit/*0 based*/)	const;
#endif

//////////////////
/// data members
	 scan_t m_scan;
};

///////////////////////
//
// INLINE FUNCTIONS
// 
////////////////////////
#ifdef POPCOUNT_64
inline int BBIntrin::popcn64() const{
	BITBOARD pc=0;
	for(int i=0; i<nBB_; i++){
		pc+=__popcnt64(vBB_[i]);
	}
return pc;
}


inline int BBIntrin::popcn64(int nBit) const{
/////////////////////////
// Population size from nBit(included) onwards
	
	BITBOARD pc=0;
	
	int nBB=WDIV(nBit);
	for(int i=nBB+1; i<nBB_; i++){
		pc+=__popcnt64(vBB_[i]);
	}

	//special case of nBit bit block
	BITBOARD bb=vBB_[nBB]&~Tables::mask_low[WMOD(nBit)];
	pc+=__popcnt64(bb);

return pc;
}

#endif

inline int BBIntrin::next_bit(int &nBB_new)  {
////////////////////////////
// Date:23/3/2012
// BitScan not destructive
// Version that use static data of the last bit position and the last table
//
// COMMENTS
// 1-Require previous assignment BitSet::scan=0 and BBIntrin::scanv=MASK_LIM

	unsigned long posInBB;
			
	//Search int the last table
	if(_BitScanForward64(&posInBB, vBB_[m_scan.bbi] & Tables::mask_high[m_scan.pos])){
		m_scan.pos =posInBB;
		nBB_new=m_scan.bbi;
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//Not found in the last table. Search in the rest
		for(int i=m_scan.bbi+1; i<nBB_; i++){
			if(_BitScanForward64(&posInBB,vBB_[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				nBB_new=i;
				return (posInBB+ WMUL(i));
			}
		}
	}
return EMPTY_ELEM;
}

inline int BBIntrin::next_bit(int &nBB_new,  BBIntrin& bbN_del ) {
////////////////////////////
// Date:30/3/2012
// BitScan not destructive
// Version that use static data of the last bit position and the last table
//
// COMMENTS
// 1-Require previous assignment BitSet::scan=0 and BitSet::scanv=MASK_LIM

	unsigned long posInBB;
			
	//Search int the last table
	if(_BitScanForward64(&posInBB, vBB_[m_scan.bbi] & Tables::mask_high[m_scan.pos])){
		m_scan.pos =posInBB;
		nBB_new=m_scan.bbi;
		bbN_del.vBB_[m_scan.bbi]&=~Tables::mask[posInBB];
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//Not found in the last table. Search in the rest
		for(int i=m_scan.bbi+1; i<nBB_; i++){
			if(_BitScanForward64(&posInBB,vBB_[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				nBB_new=i;
				bbN_del.vBB_[i]&=~Tables::mask[posInBB];
				return (posInBB+ WMUL(i));
			}
		}
	}
return EMPTY_ELEM;
}


inline int BBIntrin::msbn64() const{
////////////////////////////
//
// Date: 30/3/12
// Return the last bit
// 
// COMMENTS: New variable static scan which stores index of every BB

	 unsigned long posInBB;

	for(int i=nBB_-1; i>=0; i--){
		//Siempre me queda la duda mas de si es mas eficiente comparar con 0
		if(_BitScanReverse64(&posInBB,vBB_[i]))
			return (posInBB+WMUL(i));
	}
	
return EMPTY_ELEM;  
}

	
inline int BBIntrin::lsbn64() const{
	unsigned long posBB;
	for(int i=0; i<nBB_; i++){
		if(_BitScanForward64(&posBB, vBB_[i]))
			return(posBB+ WMUL(i));	
	}
return EMPTY_ELEM;
}



inline int BBIntrin::next_bit_del() {
////////////////////////////
//
// Date: 23/3/12
// BitScan with Delete (D- erase the bit scanned) and Intrinsic
// 
// COMMENTS: New variable static scan which stores index of every BB

	 unsigned long posInBB;

	for(int i=m_scan.bbi; i<nBB_; i++)	{
		if(_BitScanForward64(&posInBB,vBB_[i])){
			m_scan.bbi=i;
			vBB_[i]&=~Tables::mask[posInBB];			//Deletes the current bit before returning
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}


inline int BBIntrin::next_bit_del(int& nBB) {
//////////////
// Also return the number of table
	unsigned long posInBB;

	for(int i=m_scan.bbi; i<nBB_; i++)	{
		if(_BitScanForward64(&posInBB,vBB_[i])){
			m_scan.bbi=i;
			vBB_[i]&=~Tables::mask[posInBB];			//Deleting before the return
			nBB=i;
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrin::next_bit_del(int& nBB, BBIntrin& bbN_del) {
//////////////
// BitScan DI it also erase the returned bit of the table passed
	unsigned long posInBB;

	for(int i=m_scan.bbi; i<nBB_; i++){
		if(_BitScanForward64(&posInBB,vBB_[i])){
			m_scan.bbi=i;
			vBB_[i]&=~Tables::mask[posInBB];					//Deleting before the return
			bbN_del.vBB_[i]&=~Tables::mask[posInBB];
			nBB=i;
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrin::next_bit() {
////////////////////////////
// Date:23/3/2012
// BitScan not destructive
// Version that use static data of the last bit position and the last table
//
// COMMENTS
// 1-Require previous assignment m_scan_bbl=0 and m_scan.pos=MASK_LIM

	unsigned long posInBB;
			
	//Search for next bit in the last block
	if(_BitScanForward64(&posInBB, vBB_[m_scan.bbi] & Tables::mask_high[m_scan.pos])){
		m_scan.pos =posInBB;
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//Search in the remaining blocks
		for(int i=m_scan.bbi+1; i<nBB_; i++){
			if(_BitScanForward64(&posInBB,vBB_[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				return (posInBB+ WMUL(i));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBIntrin::prev_bit		() {
////////////////////////////
// Date:13/4/2012
// BitScan not destructive in reverse order (end-->begin)
//
// COMMENTS
// 1-Require previous assignment BitSet::scan=0 and BitSet::scanv=MASK_LIM

	unsigned long posInBB;
				
	//Search int the last table
	if(_BitScanReverse64(&posInBB, vBB_[m_scan.bbi] & Tables::mask_low[m_scan.pos])){
		m_scan.pos =posInBB;
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//Not found in the last table. Search in the rest
		for(int i=m_scan.bbi-1; i>=0; i--){
			if(_BitScanReverse64(&posInBB,vBB_[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				return (posInBB+ WMUL(i));
			}
		}
	}
	
return EMPTY_ELEM;
}

inline int BBIntrin::prev_bit	(int& nBB) {
////////////////////////////
// Date:13/4/2012
// BitScan not destructive in reverse order (end-->begin)
//
// COMMENTS
// 1-Require previous assignment BitSet::scan=0 and BitSet::scanv=MASK_LIM

	unsigned long posInBB;
				
	//Search int the last table
	if(_BitScanReverse64(&posInBB, vBB_[m_scan.bbi] & Tables::mask_low[m_scan.pos])){
		m_scan.pos =posInBB;
		nBB=m_scan.bbi;
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//Not found in the last table. Search in the rest
		for(int i=m_scan.bbi-1; i>=0; i--){
			if(_BitScanReverse64(&posInBB,vBB_[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				nBB=i;
				return (posInBB+ WMUL(i));
			}
		}
	}
	
return EMPTY_ELEM;
}

inline int BBIntrin::prev_bit_del() {
//////////////
// BitScan DI 
	unsigned long posInBB;

	for(int i=m_scan.bbi; i>=0; i--){
		if(_BitScanReverse64(&posInBB,vBB_[i])){
			m_scan.bbi=i;
			vBB_[i]&=~Tables::mask[posInBB];			//Deleting before the return
			return (posInBB+WMUL(i));
		}
	}
return EMPTY_ELEM;  
}

inline int BBIntrin::prev_bit_del(int& nBB, BBIntrin& del) {
//////////////
// BitScan DI it also erase the returned bit of the table passed
	unsigned long posInBB;

	for(int i=m_scan.bbi; i>=0; i--){
		if(_BitScanReverse64(&posInBB,vBB_[i])){
			m_scan.bbi=i;
			vBB_[i]&=~Tables::mask[posInBB];			//Deleting before the return
			del.vBB_[i]&=~Tables::mask[posInBB];
			nBB=i;
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrin::prev_bit_del(int& nBB) {
//////////////
// BitScan DI 
	unsigned long posInBB;

	for(int i=m_scan.bbi; i>=0; i--){
		if(_BitScanReverse64(&posInBB,vBB_[i])){
			m_scan.bbi=i;
			vBB_[i]&=~Tables::mask[posInBB];			//Deleting before the return
			nBB=i;
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}

inline
int BBIntrin::init_scan(scan_types sct){
	switch(sct){
	case NON_DESTRUCTIVE:
		set_bbindex(0);
		set_posbit(MASK_LIM);
		break;
	case NON_DESTRUCTIVE_REVERSE:
		set_bbindex(nBB_-1);
		set_posbit(WORD_SIZE);		//mask_low[WORD_SIZE]=ONE
		break;
	case DESTRUCTIVE:
		set_bbindex(0); 
		break;
	case DESTRUCTIVE_REVERSE:
		set_bbindex(nBB_-1);
		break;
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}

inline
int BBIntrin::init_scan_from (int from, scan_types sct){
/////////////////
// scans from 'from' onwards, excluding from
//
// COMMENTS: if you want to scan from the beginning, use from=EMPTY_ELEM
// 
	if(from==EMPTY_ELEM) 
			init_scan(sct);
	else{
		switch(sct){
		case NON_DESTRUCTIVE:
		case NON_DESTRUCTIVE_REVERSE:
			set_bbindex(WDIV(from));
			set_posbit(WMOD(from));
			break;
		case DESTRUCTIVE:
		case DESTRUCTIVE_REVERSE:
			set_bbindex(WDIV(from)); 
			break;
		default:
			cerr<<"bad scan type"<<endl;
			return -1;
		}
	}
return 0;
}

inline
int* BBIntrin::to_C_array(int* lv, std::size_t& size, bool rev){
	int v = EMPTY_ELEM;
	size = 0;

	if (rev) {
		init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	}
	else {
		init_scan(BBObject::NON_DESTRUCTIVE);
	}
	
	while( (v = next_bit())!= EMPTY_ELEM ){
		lv[size++] = v;
	}
	return lv;
}

//inline
//int* BBIntrin::to_old_vector_reverse (int* lv, int& size){
//	int v; size=0;
//	init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
//	while(true){
//		if( (v=prev_bit())!=EMPTY_ELEM ){
//			lv[size++]=v;
//		}else break;
//	}
//	return lv;
//}









