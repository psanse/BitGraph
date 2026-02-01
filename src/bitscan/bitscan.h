/**
 * @file bitscan.h
 * @brief facade aliases for consumer code of the BITSCAN lib, a C++ library for bitset optimization.
 * @details The BITSCAN library is currently released as part of the BITGRAPH C++. 
 * @details
 *  - date 2014
 *  - last modified 01/02/2026
 * @author pss
 **/

#include "bbsentinel.h"					//base of the non-sparse hierarchy
#include "bbscan_sparse.h"	
#include "bbutils.h"					//basic algorithms

namespace bitgraph {

	//non-sparse

	using simple_bitarray = Bitset;
	using bitarray = BBScan;
	using watched_bitarray = _impl::BBSentinel;

	//sparse
	
	using simple_sparse_bitarray = BitsetSp;
	using sparse_bitarray = BBScanSp;

}




