// BitBoard.cpp: implementation of the BitBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "bitboard.h"
#include <iostream>

using namespace std;



inline int BitBoard::lsb64_de_Bruijn(const BITBOARD bb_dato) {
	////////////////////////
	// Uses de Bruijn perfect hashing in two versions:
	// a) ISOLANI_LSB with hashing bb &(-bb)
	// b) All 1 bits to LSB with hashing bb^(bb-1)
	// 
	//	Option b) would seem to exploit the CPU HW better on average and is defined as default
	//  To change this option go to config.h file


#ifdef ISOLANI_LSB
	return (bb_dato == 0) ? EMPTY_ELEM : Tables::indexDeBruijn64_ISOL[((bb_dato & -bb_dato) * DEBRUIJN_MN_64_ISOL) >> DEBRUIJN_MN_64_SHIFT];
#else
	return (bb_dato == 0) ? EMPTY_ELEM : Tables::indexDeBruijn64_SEP[((bb_dato ^ (bb_dato - 1)) * DEBRUIJN_MN_64_SEP) >> DEBRUIJN_MN_64_SHIFT];
#endif
}

inline
int BitBoard::msb64_de_Bruijn(const BITBOARD bb_dato) {
	////////////////////////
	// date: 27/7/2014
	// Uses de Bruijn perfect hashing but first has to create 1-bits from the least significant to MSB
	//
	// More practical than LUPs of 65535 entries, but requires efficiency tests for a precise comparison

	if (bb_dato == 0) return EMPTY_ELEM;

	//creates all 1s up to MSB position
	BITBOARD bb = bb_dato;
	bb |= bb >> 1;
	bb |= bb >> 2;
	bb |= bb >> 4;
	bb |= bb >> 8;
	bb |= bb >> 16;
	bb |= bb >> 32;

	//applys same computation as for LSB-de Bruijn
	return Tables::indexDeBruijn64_SEP[(bb * DEBRUIJN_MN_64_SEP) >> DEBRUIJN_MN_64_SHIFT];
}


inline 
int BitBoard::popc64_lup(const BITBOARD bb_dato) {
	//////////////////////////////
	// Lookup implementation 
	//
	// COMMENTS
	// more compact than lookup without union but similar in time
	register union x {
		U16 c[4];
		BITBOARD b;
	} val;

	val.b = bb_dato; //Carga unisn

	return (Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]]); //Suma de poblaciones 
}
	
//ostream & operator<<(ostream& o, const BITBOARD& bb_data){
//	BITBOARD bb=bb_data;
//	if(bb){
//		int nBit=EMPTY_ELEM;
//		while(bb)
//		{
//			nBit=BitBoard::lsb64_de_Bruijn(bb);
//			if(nBit==EMPTY_ELEM) break;
//			o<<nBit<<" ";
//		bb^=Tables::mask[nBit];
//		}
//	}
//
//	o<<" ["<<BitBoard::popc64(bb_data)<<"]"<<endl;
//
//return o;
//}

 int BitBoard::lsb64_lup	(const BITBOARD bb){
	U16 bb16;

	if(bb){ 
		bb16=(U16)bb;
		if(bb16) return (Tables::lsba[0][bb16]);
		bb16=(U16)(bb>>16);
		if(bb16) return (Tables::lsba[1][bb16]);
		bb16=(U16)(bb>>32);
		if(bb16) return (Tables::lsba[2][bb16]);
		bb16=(U16)(bb>>48);
		if(bb16) return (Tables::lsba[3][bb16]);
	}

return EMPTY_ELEM;		//should not occur
}



 int  BitBoard::lsb64_pc(const BITBOARD bb_dato) {
/////////////////////
// isolani-1
// Comments: useful if you have anchitecturewith a fast popcount

	 if(bb_dato){
		 return BitBoard::popc64( (bb_dato & -bb_dato) - 1 );
	 }
return EMPTY_ELEM;
 }
 


int BitBoard::lsb64_mod(const BITBOARD bb_dato){
//////////////////
// Modulus implementation of perfect hashing
// Use T_64[67] to convert the class of BITBOARDS 000...1...000in the position of the bit 1
// COMMENTS: Modulus % x reduces efficiency 
	if(bb_dato){
		return(Tables::T_64[(bb_dato&(~bb_dato+1))%67]);
	}

return EMPTY_ELEM;
}

int  BitBoard::lsb64_lup_1(const BITBOARD bb_dato){
//////////////////////////
//Look up table implementation
//
// COMMENTS
// 1-Best implementation for 32 bits on x86 on average. Is is worse than LSB_32x only for sparse BB with
//	 1bits in the final BB

	union x	{
		U16 c[4];
		BITBOARD b;
	}val;				/*register*/
	val.b = bb_dato; //union load

	//Control
	if(bb_dato){
		if(val.c[0])
			return (Tables::lsb[val.c[0]]);
		else if (val.c[1])
			return (Tables::lsb[val.c[1]] + 16);
		else if (val.c[2])
			return (Tables::lsb[val.c[2]] + 32);
		else return (Tables::lsb[val.c[3]] + 48);		//nonzero by (1)
	}

return EMPTY_ELEM;		//Should not reach
}

//////////////////////////////
//
// PopCount
//
////////////////////////////

int BitBoard::popc64_lup_1(const BITBOARD bb_dato){  
//uses 16 bit look up table
	return (Tables::pc[(U16)bb_dato]+Tables::pc[(U16)(bb_dato>>16)]+Tables::pc[(U16)(bb_dato>>32)]+Tables::pc[(U16)(bb_dato>>48)]); 
}

/////////////////////
// 
// MSB_64 (...)
//
// No bit shuffling implememation seems close to LOOK UP for MSB(64 bits)
//
/////////////////////

int BitBoard::msb64_lup(const BITBOARD bb){
//////////////////////
// lookup table variant
	union x	{
		U16 c[4];
		BITBOARD b;
	} val;
	val.b = bb; 
	
	//if(bb==0) return -1;				//for sparse data

	if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]]);
			if(val.c[2]) return (Tables::msba[2][val.c[2]]);
			if(val.c[1]) return (Tables::msba[1][val.c[1]]);
			if(val.c[0]) return (Tables::msba[0][val.c[0]]);
	}

/*	if (val.c[3])							//access table msb[65536](valores:0-15) in blocks of 16 bits
		return (msb[val.c[3]] + 48);
	else if (val.c[2])
			return (msb[val.c[2]] + 32);
	else if (val.c[1])
			return (msb[val.c[1]] + 16);
	else  return (msb[val.c[0]]);			//should be nonzero by (1)*/
	
return EMPTY_ELEM;									//should not reach here
}


///////////////////////
//
// VIEW
//
///////////////////////

std::ostream& BitBoard::print(const BITBOARD bb_data, std::ostream& o) {
///////////////////////
// Shows bits and popcount

	BITBOARD bb=bb_data;
	if(bb){
		int nBit=EMPTY_ELEM;
		while(bb)
		{
			nBit=BitBoard::lsb64_de_Bruijn(bb);
			if(nBit==EMPTY_ELEM) break;
			o<<nBit<<" ";
		bb^=Tables::mask[nBit];
		}
	}

	o<<"["<<BitBoard::popc64(bb_data)<<"]"<<endl;

	return o;

}



