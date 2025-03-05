/**
 * @file bitscan.h
 * @brief Main header file for the BITSCAN library
 * @version 1.0
 * @details The BITSCAN library is a C++ library for bit set optimization. *
 * @details created 2014, last update 12/02/2025
 * @author pss
 **/

#include "bitscan/bbsentinel.h"			//base of the non-sparse hierarchy
#include "bitscan/bbscan_sparse.h"	

//useful aliases

//base of the hierarchy
using bbo = BBObject;

//non-sparse
using simple_bitarray = BitSet;
using bitarray = BBScan;
using watched_bitarray = BBSentinel;

//sparse
using simple_sparse_bitarray = BitSetSp;
using sparse_bitarray = BBScanSp;




