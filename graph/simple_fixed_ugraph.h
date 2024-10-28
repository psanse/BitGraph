/*  
 * fixed_UgraphF.h file from the GRAPH library, manages non sparse graphs of a fixed size
*  in compile time (with application to maximal clique enumeration)
 * GRAPH stores the adjacency matrix un full but each row is encoded as a bitstring. 
 * (see license file (legal.txt) for references)
 *
 * Copyright (C)
 * Author: Pablo San Segundo
 * Intelligent Control Research Group CAR(UPM-CSIC) 
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

#ifndef __SIMPLE_FIXED_UGRAPH_H__
#define __SIMPLE_FIXED_UGRAPH_H__

#include "simple_ugraph.h"
#include "utils/prec_timer.h"
#include "utils/logger.h"

template<int N>
class UgraphF : public Ugraph<bitarray>{
public:
	UgraphF						():Ugraph<bitarray>(N){}						//allocates fixed memory for N vertices
	~UgraphF					(){Ugraph<bitarray>::clear();};
	
	void create_empty_graph		();
private:
virtual	int init				(int size){	LOG_INFO("UgraphF::init: error, this function should never be called"); return -1;}			 
};

template<int N>
void UgraphF<N>::create_empty_graph(){
	for(int i=0; i<N; i++){
		m_g[i].erase_bit();
	}
}

#endif