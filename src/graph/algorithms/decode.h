/*
* @file decode.h
* @brief interface for the Decode class, which decodes orderings in graphs
* @date 29/11/13
* @last_update 17/12/24
*/

#ifndef __DECODE_ORDERINGS_H__
#define __DECODE_ORDERINGS_H__

#include "graph_globals.h"
#include "utils/logger.h"
#include <vector>

namespace bitgraph {
	namespace _impl {

		class Decode {
		private:

			using VertexOrdering = bitgraph::VertexOrdering;

			/*
			* @brief functor which unwinds composite orderings for a given vertex
			*/
			struct DecodeVertex {
				DecodeVertex(const std::vector<VertexOrdering>& ords) :
					ords_(ords)
				{}

				int operator() (int v) const {
					for (auto it = ords_.rbegin(); it != ords_.rend(); ++it) {
						try {
							v = (*it).at(v);								//will throw exception	
						}
						catch (const std::out_of_range& oor) {
							LOGG_ERROR("Out of Range error: ", oor.what());
							LOGG_ERROR("DecodeVertex: operator() - vertex : ", v);
							LOGG_ERROR("exiting...");
							std::exit(EXIT_FAILURE);
						}
					}
					return v;
				}

				/////////////////////////////////////		
				const std::vector<VertexOrdering>& ords_;
			};

		public:
			///////////////////////////////////////////////
			static void reverse_in_place(VertexOrdering& o);						//changes [index]-->[value] in place
			static VertexOrdering reverse(const VertexOrdering& o);							//changes [index]-->[value] 
			////////////////////////////////////////////////

			void clear() { ords_.clear(); }
			void add_ordering(const VertexOrdering& o) { ords_.emplace_back(o); }
			bool is_empty() const noexcept { return ords_.empty(); }
			const VertexOrdering& first_ordering() const noexcept {
				static const VertexOrdering empty; 
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
			VertexOrdering decode(const VertexOrdering& list) const;

			/*
			* @brief modifies the list of vertices @list in place, decoding them for the given orderings
			* @param list list of vertices
			* @returns -1 if list is empty, 0 otherwise
			* @details: created 4/10/17, last_update 31/08/2025
			*/
			int decode_in_place(VertexOrdering& list) const;


			//int decode_list(const VertexOrdering& l, VertexOrdering& res) const;

			//////////////////////
			// data members
			std::vector<VertexOrdering> ords_;					//composition of orderings in [new]--[old] format
		};

	}//end of namespace _impl

	using _impl::Decode;								//alias for the Decode class	

}//end of namespace bitgraph


#endif