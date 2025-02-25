/**
* @file bitboard.cpp
* @brief Implementation of the BitBoard class 
**/

#include "bitblock.h"
#include <iostream>

using namespace std;


namespace bblock {

	int popc64_lup(const BITBOARD bb_dato) {

		register union x {
			U16 c[4];
			BITBOARD b;
		} val;

		val.b = bb_dato;

		return (Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]]); //Suma de poblaciones 
	}


	int lsb64_lup(const BITBOARD bb) {
		U16 bb16;

		if (bb) {
			bb16 = (U16)bb;
			if (bb16) return (Tables::lsba[0][bb16]);
			bb16 = (U16)(bb >> 16);
			if (bb16) return (Tables::lsba[1][bb16]);
			bb16 = (U16)(bb >> 32);
			if (bb16) return (Tables::lsba[2][bb16]);
			bb16 = (U16)(bb >> 48);
			if (bb16) return (Tables::lsba[3][bb16]);
		}

		return EMPTY_ELEM;		//should not occur
	}


	int  lsb64_pc(const BITBOARD bb_dato) {

		if (bb_dato) {
			return popc64((bb_dato & -bb_dato) - 1);
		}
		return EMPTY_ELEM;
	}


	int lsb64_mod(const BITBOARD bb_dato) {

		if (bb_dato) {
			return(Tables::T_64[(bb_dato & (~bb_dato + 1)) % 67]);
		}

		return EMPTY_ELEM;
	}

	int  lsb64_lup_eff(const BITBOARD bb_dato) {


		register union x {
			U16 c[4];
			BITBOARD b;
		}val;
		val.b = bb_dato;	//union load

		//Control
		if (bb_dato) {
			if (val.c[0])
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

	int popc64_lup_1(const BITBOARD bb_dato) {
		return (Tables::pc[(U16)bb_dato] + Tables::pc[(U16)(bb_dato >> 16)] +
			Tables::pc[(U16)(bb_dato >> 32)] + Tables::pc[(U16)(bb_dato >> 48)]);
	}

	/////////////////////
	// 
	// MSB_64 (...)
	//
	// Comment:  No bit shuffling implememation seems close to LOOK UP for MSB(64 bits)
	//
	////////////////////////

	int msb64_lup(const BITBOARD bb) {

		register union x {
			U16 c[4];
			BITBOARD b;
		} val;
		val.b = bb;

		//if(bb==0) return -1;				//for sparse data

		if (val.b) {
			if (val.c[3]) return (Tables::msba[3][val.c[3]]);
			if (val.c[2]) return (Tables::msba[2][val.c[2]]);
			if (val.c[1]) return (Tables::msba[1][val.c[1]]);
			if (val.c[0]) return (Tables::msba[0][val.c[0]]);
		}

		/*	if (val.c[3])							//access table msb[65536](valores:0-15) in blocks of 16 bits
				return (msb[val.c[3]] + 48);
			else if (val.c[2])
					return (msb[val.c[2]] + 32);
			else if (val.c[1])
					return (msb[val.c[1]] + 16);
			else  return (msb[val.c[0]]);			//should be nonzero by (1)*/

		return EMPTY_ELEM;							//should not reach here
	}
		

	//////
	// I/O
	//////
	std::ostream& print( BITBOARD bb_data, std::ostream& o, bool endofl)
	{

		if (bb_data) {

			int nBit = EMPTY_ELEM;
			BITBOARD bb = bb_data;

			//bitscanning loop
			while ((nBit = lsb64_de_Bruijn(bb)) != EMPTY_ELEM)
			{
				o << nBit << " ";

				//removes the bit (XOR operation)
				bb ^= Tables::mask[nBit];
			}
		}

		//adds population count
		o << "[" << popc64(bb_data) << "]";
		
		if (endofl) { o << std::endl; }

		return o;

	}

	void copy	 (int firstBit, int lastBit, const BITBOARD& source,  BITBOARD& dest)
	{
		auto destOri = dest;
	
		//delete left of lastBit and right of firstBit
		dest = source & MASK_1(firstBit, lastBit);
		
		//add left of lastBit and right of firstBit
		dest |= destOri & Tables::mask_high [lastBit];
		dest |= destOri & Tables::mask_low[firstBit];
	}

	void copy_high (int bit, const BITBOARD& source, BITBOARD& dest)
	{
		auto destOri = dest;

		//copy the good part of the block (bit included)
		dest = source & MASK_1_HIGH(bit);

		//add right part of the block (excluding bit)
		dest |= destOri & Tables::mask_low[bit];

	}

	void copy_low(int bit, const BITBOARD& source, BITBOARD& dest)
	{
		auto destOri = dest;

		//copy the good part of the block (bit included)
		dest = source & MASK_1_LOW(bit);

		//add right part of the block (excluding bit)
		dest |= destOri & Tables::mask_high[bit];
	}

}//end namespace bblock



