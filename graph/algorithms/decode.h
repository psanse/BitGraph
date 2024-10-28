///////////////////////////////
// interface for the Decode class, which decodes orderings in graphs
// author: pss
// date: 29/11/2013
// last update:15/8/17

#ifndef __DECODE_H__
#define __DECODE_H__

#include <vector>
#include <iterator>
#include <algorithm>

using namespace std;
typedef vector<int> vint;

class Decode{
public:
	static void reverse_in_place(vint& o);			//changes [index]-->[value] in place
	static vint reverse(const vint& o);			

	void clear(){orderings.clear();}
	void insert_ordering(const vint& o){ orderings.push_back(o);}
	vector<int> get_first_ordering() const{return orderings.front(); }
	vector<int> decode_list(const vint& list);
	int decode_list	(const vint& l, vint& res);					/* 4/10/17 */
	int decode_list_in_place(vint& list);

	int decode_node(int v);										/* 16/6/17 */
	bool is_empty() const { return orderings.empty(); }
private:
	struct DecodeFunction{
	public:
		DecodeFunction(vector< vint >& o): orderings(o){}
		int operator() (int v) const {  
			vector< vint >::reverse_iterator it;
			it=orderings.rbegin();
			int new_v=v;
			while(it!=orderings.rend()){
				new_v=(*it)[new_v];
				it++;
			}
			return new_v;
		}

		vector< vint >& orderings;
	};
//////////////////////
// data members
	vector< vint> orderings;	
};

inline
int Decode::decode_node(int v){
////////////////////////
// decodes a single node
// date: 16/6/17

	vector<vint>::reverse_iterator rit;
	int v_map=v;
	for(rit=orderings.rbegin(); rit!=orderings.rend(); rit++){
		v_map=(*rit)[v_map];
	}
	return v_map;
}

inline
void Decode::reverse_in_place(vector<int>& o){
///////////////////////////////
//changes [index]-->[value] in place

	vint vaux(o.size());
	vint::iterator it;
	int i=0;
	for(it=o.begin(); it!=o.end(); it++){
		vaux.at(*it)=i++;
	}
	o.assign(vaux.begin(), vaux.end());
}

inline
vint Decode::reverse(const vint& o){
///////////////////////////////
//changes [index]-->[value] in place
//
// RETURNS reverse ordering

	vint vres(o.size());
	vint::const_iterator it;
	int i=0;
	for(it=o.begin(); it!=o.end(); it++){
		vres.at(*it)=i++;
	}

	return vres;
}

inline
vector<int> Decode::decode_list(const vint& l){
////////////////////////////////
// For a given vertex list decodes the vertices according to the orderings
// 
// OBSERVATIONS: Assumes that all orderings are in the form [NEW_INDEX]= OLD_INDEX
	
	//assert
	if (l.empty()) return l;

	vint res(l.size());
	if(orderings.empty()){					//no reordering, return a copy
		res.assign(l.begin(), l.end());
		return res;
	}
		
	DecodeFunction df(orderings);
	transform(l.cbegin(), l.cend(), res.begin(), df );
return res;
}

inline
int Decode::decode_list(const vint& l, vint& res){
////////////////////////////////
// For a given vertex list decodes the vertices according to the orderings
// 
// OBSERVATIONS: Assumes that all orderings are in the form [NEW_INDEX]= OLD_INDEX
//
// TODO* CHECK!  (4/10/17)
		
	if(orderings.empty()){					//no reordering, return a copy
		res=l;
		return 0;		
	}
	
	res.clear();
	std::back_insert_iterator< std::vector<int> > b_it(res);
	DecodeFunction df(orderings);
	transform(l.cbegin(), l.cend(), b_it, df );

return 0;
}

inline
int Decode::decode_list_in_place(vint& l){
////////////////////////////////
// For a given vertex list decodes (NEW-2-OLD) the vertices in place according to the
// current orderings established in the decoder
//
// date: 8/6/17
// 
// RETURNS -1 (orderings empty, l remains unchanged), 0 ok
//
// OBSERVATIONS: All orderings in the decoder have to be of the type
//                [NEW_INDEX]= OLD_INDEX
//
// /* TODO-CHECK */
			
	if(l.empty()) return -1;
	
	DecodeFunction df(orderings);
	transform(l.begin(), l.end(), l.begin(), df );
return 0;
}

#endif