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

//aliases
using vint = std::vector<int>;

namespace bitgraph {
	namespace _impl {

		class Decode {
		private:
			/*
			* @brief functor which unwinds composite orderings for a given vertex
			*/
			struct DecodeVertex {
				DecodeVertex(const std::vector<vint>& ords) :
					ords_(ords)
				{
				}

				int operator() (int v) const {
					for (auto it = ords_.rbegin(); it != ords_.rend(); ++it) {
						try {
							v = (*it).at(v);								//will throw exception	
						}
						catch (const std::out_of_range& oor) {
							LOGG_ERROR("Out of Range error: ", oor.what());
							LOGG_ERROR("DecodeVertex: operator() - vertex : ", v);
							LOGG_ERROR("exiting...");
							std::abort();
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
			void add_ordering(const vint& o) { ords_.emplace_back(o); }
			bool is_empty() const noexcept { return ords_.empty(); }
			const vint& first_ordering() const noexcept {
				static const vint empty; 
				return ords_.empty()? empty : ords_.front();
			}

			/*
			* @brief decodes a single vertex for the given orderings
			* @date 16/6/17
			* @last_update 17/12/2024
			*/
			int decode(int v) const;

			/*
			* @brief decodes of a list of vertices @list for the given orderings
			* @param list list of vertices
			* @returns decoded list of vertices - empty if @list is empty
			* @details: created 4/10/17, last_update 31/08/2025			 
			*/
			vint decode(const vint& list) const;

			/*
			* @brief modifies the list of vertices @list in place, decoding them for the given orderings
			* @param list list of vertices
			* @returns -1 if list is empty, 0 otherwise
			* @details: created 4/10/17, last_update 31/08/2025
			*/
			int decode_in_place(vint& list) const;


			//int decode_list(const vint& l, vint& res) const;

			//////////////////////
			// data members
			std::vector<vint> ords_;					//composition of orderings in [new]--[old] format
		};

	}//end of namespace _impl

	using _impl::Decode;								//alias for the Decode class	

}//end of namespace bitgraph


#endif