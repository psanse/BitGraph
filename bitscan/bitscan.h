/**
 * @file bitscan.h
 * @brief Main header file for the BITSCAN library
 * @details The BITSCAN library is a C++ library for bit set optimization.
 * @version 1.0
 * @created 2014
 * @last_update 29/01/2025
 *
 * Copyright (C) 2014
 * Author: Pablo San Segundo
 * Intelligent Control Research Group (CSIC-UPM) 
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies, in source code or
*  in binaries. For precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#include "bbalg.h"
#include "bbsentinel.h"			
#include "bbintrinsic_sparse.h"	

//useful aliases
using bitblock = BitBoard;
using bitarray = BBIntrin;
using watched_bitarray = BBSentinel;
using sparse_bitarray = BBIntrinS;
using simple_bitarray = BitBoardN;
using simple_sparse_bitarray = BitBoardS;
using bbo = BBObject;



