/*
* @file decode.h
* @brief interface for the Decode class, which decodes orderings in graphs
* @date 29/11/13
* @last_update 17/12/24
*/

#ifndef __DECODE_ORDERINGS_H__
#define __DECODE_ORDERINGS_H__

#include "../utils/logger.h"
#include <algorithm>
#include <vector>

using vint = std::vector<int>;

class Decode {
private:
	struct DecodeFunction {
	public:
		DecodeFunction(const vector< vint >& o) : orderings(o) {}
		int operator() (int v) const {
			for (auto it = orderings.rbegin(); it != orderings.rend(); ++it) {
				try {
					v = (*it).at(v);								//will throw exception	
				}
				catch (const std::out_of_range& oor) {
					LOG_ERROR("Out of Range error: ", oor.what());
					LOG_ERROR("DecodeFunction: operator() - vertex : ", v );
					LOG_ERROR("exiting...");
					std::exit(-1);
				}
			}
			return v;
		}
		
		/////////////////////////////////////		
		const vector< vint >& orderings;
	};

public:
	///////////////////////////////////////////////
	static void reverse_in_place(vint& o);						//changes [index]-->[value] in place
	static vint reverse(const vint& o);							//changes [index]-->[value] 
	////////////////////////////////////////////////

	void clear() { orderings.clear(); }
	void insert_ordering(const vint& o) { orderings.push_back(o); }
	vector<int> get_first_ordering() const { return orderings.front(); }
	bool is_empty() const { return orderings.empty(); }
	
	/*
	* @brief decodes a single vertex for the given orderings
	* @date 16/6/17 
	* @last_update 17/12/2024
	*/
	int decode_node(int v) const;											//determines original vertex given a composition of orderings [new]-[old]								
	
	/*
	* @brief decodes of a list of vertices for the given orderings
	* @date 4/10/17
	* @last_update 17/12/2024
	* 
	* TODO- check what should be the return value when the given orderings are empty!
	*/
	vint decode_list(const vint& list) const;
	int decode_list(const vint& l, vint& res) const;						
	int decode_list_in_place(vint& list);

//////////////////////
// data members
	vector< vint> orderings;												//[new]--[old] composition of orderings
};

inline
int Decode::decode_node(int v) const 
{
	DecodeFunction df(orderings);
	return df(v);
}

inline
void Decode::reverse_in_place(vint& o)
{
	vint vaux(o.size());
	for (std::size_t i = 0; i < o.size(); ++i) {
		vaux[o[i]] = i;
	}
	o = std::move(vaux);
}

inline
vint Decode::reverse(const vint& o)  
{
	vint vres(o.size());
	for (std::size_t i = 0; i < o.size(); ++i) {
		vres[o[i]] = i;
	}
	return vres;
}

inline
vint Decode::decode_list(const vint& l) const 
{
	vint res;
	if (!l.empty()) {
		DecodeFunction df(orderings);
		res.resize(l.size());
		transform(l.cbegin(), l.cend(), res.begin(), df);
	}
	return res;
}

inline
int Decode::decode_list(const vint& l, vint& res) const
{
	if (orderings.empty()) {					//no reordering, return a copy
		res = l;
		return 0;
	}

	res.clear();
	res.reserve(l.size());
	DecodeFunction df(orderings);
	transform(l.cbegin(), l.cend(), res.begin(), df);

	//old code- why using a back_insert_iterator?
	//std::back_insert_iterator< std::vector<int> > b_it(res);	
	//transform(l.cbegin(), l.cend(), b_it, df );
	
	return 0;
}

inline
int Decode::decode_list_in_place(vint& l)
{	
	if (l.empty()) return -1;

	DecodeFunction df(orderings);
	transform(l.begin(), l.end(), l.begin(), df);
	return 0;
}

#endif