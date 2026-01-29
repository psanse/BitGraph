/**
 * @file bitscan.h
 * @brief Main header file for the BITSCAN part of the BITGRAPH lib
 * @version 1.0
 * @details The BITSCAN library is a C++ library for bit set optimization. *
 * @details created 2014, last update 12/02/2025
 * @author pss
 **/

#include "bbsentinel.h"					//base of the non-sparse hierarchy
#include "bbscan_sparse.h"	
#include "bbutils.h"					//basic algorithms

 //useful aliases

namespace bitgraph {

	//base of the hierarchy
	using bbo = BBObject;

	//non-sparse
	using simple_bitarray = Bitset;
	using bitarray = BBScan;
	using watched_bitarray = _impl::BBSentinel;

	//sparse
	using simple_sparse_bitarray = BitSetSp;
	using sparse_bitarray = BBScanSp;

}




