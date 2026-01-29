/*
* @file decode.cpp
* @brief implementation of the Decode class, which decodes orderings in graphs
* @date 29/11/13
* @last_update 17/12/24
*/

#include "graph/algorithms/decode.h"
#include <algorithm>

using namespace bitgraph;

int Decode::decode(int v) const
{ 
	DecodeVertex df(ords_);
	return df(v); 
};

void Decode::reverse_in_place(VertexOrdering& o)
{
	VertexOrdering vaux(o.size());
	for (std::size_t i = 0; i < o.size(); ++i) {
		vaux[o[i]] = i;
	}
	o = std::move(vaux);
}

VertexOrdering Decode::reverse(const VertexOrdering& o)
{
	VertexOrdering vres(o.size());
	for (std::size_t i = 0; i < o.size(); ++i) {
		vres[o[i]] = i;
	}
	return vres;
}

VertexOrdering Decode::decode(const VertexOrdering& l) const
{
	VertexOrdering res;
	
	if (!l.empty()) {
		res.resize(l.size());			//must be resized for transform 
		DecodeVertex df(ords_);
		transform(l.cbegin(), l.cend(), res.begin(), df);
	}
	return res;
}

//int Decode::decode_list(const VertexOrdering& l, VertexOrdering& res) const
//{
//	if (ords_.empty() || l.empty() ) {					//no reordering, return a copy
//		res = l;
//		return -1;										//CHECK user cases- why not return -1?
//	}
//
//	res.clear();
//	res.reserve(l.size());
//	DecodeVertex df(ords_);
//	transform(l.cbegin(), l.cend(), res.begin(), df);
//
//	//old code- why using a back_insert_iterator?
//	//std::back_insert_iterator< std::vector<int> > b_it(res);	
//	//transform(l.cbegin(), l.cend(), b_it, df );
//
//	return 0;
//}

int Decode::decode_in_place(VertexOrdering& l) const
{
	if (l.empty()) return -1;
	DecodeVertex df(ords_);
	transform(l.begin(), l.end(), l.begin(), df);
	return 0;
}

