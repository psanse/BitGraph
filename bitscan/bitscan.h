/**
 * @file bitscan.h
 * @brief Main header file for the BITSCAN library
 * @version 1.0
 * @details The BITSCAN library is a C++ library for bit set optimization. *
 * @details created 2014, last update 12/02/2025
 * @author pss
 **/

#include "bbalg.h"
#include "bbsentinel.h"			
#include "bbintrinsic_sparse.h"	

//useful aliases
using bitarray = BBScan;
using watched_bitarray = BBSentinel;
using sparse_bitarray = BBIntrinS;
using simple_bitarray = BitSet;
using simple_sparse_bitarray = BitBoardS;
using bbo = BBObject;



