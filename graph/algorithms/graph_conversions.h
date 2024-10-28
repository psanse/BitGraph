//graph_conversions.h: conversions between different graph types of the GRAPH library

#ifndef __GRAPH_TYPE_CONVERSIONS_H__
#define	__GRAPH_TYPE_CONVERSIONS_H__


#include <iostream>
#include <algorithm>
#include "../graph.h"
#include "filter_graph_sort_type.h"			//Template Graph_t reduced to undirected types
#include "utils/logger.h"
#include "utils/common.h"

using namespace std;

class GraphConversion{
public:
	static int sug2ug(const sparse_ugraph& , ugraph&);
	static int ug2sug(const ugraph& , sparse_ugraph&);
	
};

inline
int GraphConversion::sug2ug(const sparse_ugraph& sug, ugraph& ug){
///////////////////
// Converts sug into ug
// 
// date_of_creation: 25/11/16

	int NV=sug.number_of_vertices();

///////////////
//allocation
	//init ug: ***TODO lazy flag?
	ug.clear();

	//***allocation-check
	try{
		ug.m_g.resize(NV);	
	}catch(...){
		LOG_ERROR("GraphConversion::sug2ug()-:memory for graph not allocated");
		return -1;
	}
	ug.m_size=NV;
	ug.m_BB=INDEX_1TO1(NV);

	for(int i=0; i<NV; i++){
		ug.m_g[i].init(NV);		
	}
	////////////////////

/////////////////////
//conversion
	int ugi=0;
	for(int v=0; v<NV; v++){
		for(sparse_bitarray::velem_cit it= sug.m_g[v].begin(); it!=sug.m_g[v].end(); ++it){
			int sgi=it->index;
			BITBOARD sgbb=it->bb;

			if(ugi<sgi){ugi=sgi;}
			ug.m_g[v].get_bitboard(sgi)=sgbb;
		}
	}
	
	//copy other data
	ug.set_name(sug.get_name());
	//ug.m_is_wv=sug.is_weighted_v();
		
	return 0;
}

inline
int GraphConversion::ug2sug(const ugraph& ug, sparse_ugraph& sug){
///////////////////
// Converts ug into sparse sug
// 
// date_of_creation: 25/11/16

	int NV=ug.number_of_vertices();

///////////////
//allocation
	sug.clear();

	//***allocation-check
	try{
		sug.m_g.resize(NV);	
	}catch(...){
		LOG_ERROR("GraphConversion::sug2ug()-:memory for graph not allocated");
		return -1;
	}
	sug.m_size=NV;
	sug.m_BB=INDEX_1TO1(NV);

	for(int i=0; i<NV; i++){
		sug.m_g[i].init(NV);		
	}


/////////////////////
//conversion
	
	int ugi=0;
	int NBB=ug.number_of_blocks();
	for(int v=0; v<NV; v++){
		for(int bbi=0; bbi<NBB; bbi++){
			BITBOARD bb=ug.m_g[v].get_bitboard(bbi);
			if(bb!=0){
				sparse_bitarray::elem_t elem;
				elem.index=bbi;
				elem.bb=bb;
				sug.m_g[v].get_elem_set().push_back(elem);
			}
		}
	}
	
	//copy other data
	sug.set_name(ug.get_name());
	//sug.m_is_wv=ug.is_weighted_v();

	return 0;
}



#endif