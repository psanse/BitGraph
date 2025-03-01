/*
* @file decode.h
* @brief interface for the Decode class, which decodes orderings in graphs
* @date 29/11/13
* @last_update 17/12/24
*/

#ifndef __DECODE_ORDERINGS_H__
#define __DECODE_ORDERINGS_H__

#include "utils/logger.h"
#include <vector>
#include <algorithm>


using vint = std::vector<int>;

class Decode {
private:
	/*
	* @brief functor which unwinds composite orderings for a given vertex
	*/
	struct DecodeVertex {
		DecodeVertex(const std::vector<vint>& ords) :
			ords_(ords)
		{}

		int operator() (int v) const {
			for (auto it = ords_.rbegin(); it != ords_.rend(); ++it) {
				try {
					v = (*it).at(v);								//will throw exception	
				}
				catch (const std::out_of_range& oor) {
					LOGG_ERROR("Out of Range error: ", oor.what());
					LOGG_ERROR("DecodeVertex: operator() - vertex : ", v );
					LOGG_ERROR("exiting...");
					std::exit(-1);
				}
			}
			return v;
		}
		
		/////////////////////////////////////		
		const std::vector<vint>& ords_;
	};

public:
	///////////////////////////////////////////////
	static void reverse_in_place(vint& o);						//changes [index]-->[value] in place
	static vint reverse(const vint& o);							//changes [index]-->[value] 
	////////////////////////////////////////////////

	void clear() { ords_.clear(); }
	void insert_ordering(const vint& o) { ords_.emplace_back(o); }
	vint  get_first_ordering() const  { return ords_.front(); }
	bool is_empty() const { return ords_.empty(); }
	
	/*
	* @brief decodes a single vertex for the given orderings
	* @date 16/6/17 
	* @last_update 17/12/2024
	*/
	int decode_node(int v) const;											
	
	/*
	* @brief decodes of a list of vertices for the given orderings
	* @date 4/10/17
	* @last_update 17/12/2024
	* @param list list of vertices
	* TODO- check what should be the return value when the given orderings are empty!
	*/
	vint decode_list(const vint& list) const;
	int decode_list(const vint& l, vint& res) const;						
	int decode_list_in_place(vint& list);

//////////////////////
// data members
	std::vector<vint> ords_;					//composition of orderings in [new]--[old] format
};


#endif