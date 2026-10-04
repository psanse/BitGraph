/**
* @file graph_map.h
* @brief header for GraphMap class that manages pairs of vertex orderings
* @update conversions between two orderings of vertices (typically encoded by GraphFastRootSort) (14/8/17)
* @update extended to inlcude mapping to a single ordering (1/10/17)
* @date: imported from COPT framework in 2024, last_update 27/01/25
* @details: GraphMap is conceived as a wrapper for GraphFastRootSort, but it is not restricted to it due to its generic template design
* @dev pss
**/

#ifndef BITGRAPH_VERTEX_MAPPING_H
#define	BITGRAPH_VERTEX_MAPPING_H

#include "utils/logger.h"
#include "utils/collection_utils.h"
#include "bitscan/bbconfig.h"			//for INDEX_1_TO_1 macro
#include "bitscan/bbobject.h"
#include "decode.h"
#include <iostream>
#include <vector>
#include <string>

namespace bitgraph {

	///////////////////////
	//
	// GraphMap for managing pairs of vertex orderings
	//
	///////////////////////

	class GraphMap
	{

		using mapping_type = bitgraph::vertex_mapping;
		using size_type = mapping_type::size_type;

		///////////////////////
		// public interface
	public:
		enum print_t
		{
			L2R = 0,
			R2L,
			BOTH
		};	// streaming configuration

		///////////////////////
		// setters and getters

		size_type size() { return left_to_right_.size(); }
		mapping_type &get_l2r() { return left_to_right_; }
		mapping_type &get_r2l() { return right_to_left_; }
		const mapping_type &get_l2r() const { return left_to_right_; }
		const mapping_type &get_r2l() const { return right_to_left_; }
		std::string nameL() { return left_name_; }
		std::string nameR() { return right_name_; }


		void set_left_to_right(
			mapping_type left_to_right,
			std::string left_name = {},
			std::string right_name = {})
		{
			mapping_type right_to_left =
				OrderingDecoder::inverse_ordering(left_to_right);

			left_to_right_ = std::move(left_to_right);
			right_to_left_ = std::move(right_to_left);

			left_name_ = std::move(left_name);
			right_name_ = std::move(right_name);
		}

		//// sets mapping (no need to build it)
		//void set_l2r(mapping_type &l, std::string name)
		//{
		//	left_to_right_ = l;
		//	left_name_ = name;
		//}
		//void set_r2l(mapping_type &r, std::string name)
		//{
		//	right_to_left_ = r;
		//	right_name_ = name;
		//}

		////////////////
		// mapping getters

		vertex_t map_l2r(vertex_t v) const { 
			return left_to_right_[v];
		}
		vertex_t map_r2l(vertex_t v) const { 
			return right_to_left_[v]; 
		}

		/**
		 * @brief maps a (bit) set of vertices (bbl) to a (bit) set of vertices (bbr)
		 * @param bbl: input bitset of vertices in the space of the left ordering
		 * @param bbr: output bitset of vertices in the space of the right ordering
		 * @param overwrite: if TRUE, bbr is erased before mapping
		 **/
		template <class BitsetT>
		BitsetT &map_l2r(
			BitsetT &bbl,
			BitsetT &bbr,
			bool overwrite = true) const;

		/**
		 * @brief maps a (bit) set of vertices (bbr) to a (bit) set of vertices (bbl)
		 * @param bbl: output bitset of vertices in the space of the left ordering
		 * @param bbr: input bitset of vertices in the space of the right ordering
		 * @param overwrite: if TRUE, bbr is erased before mapping
		 **/
		template <class BitsetT>
		BitsetT &map_r2l(
			BitsetT &bbl,
			BitsetT &bbr,
			bool overwrite = true) const;

		////////////////////
		// build mapping operations

		/**
		 * @brief Computes mapping between two different vertex orderings of a graph,
		 *		 internally stored as left and right orderings.
		 *		 (the indexes of the original graph can be seen as in-between :-))
		 *
		 *		 I. The mappings are available by the functions map_l2r(), map_r2l()
		 *
		 *		 II. Type SortAlgT is a sorting algorithm (typically from GraphFastRootSort<Graph_t>)
		 *
		 * @param left_strategy, right_strategy: input sorting strategies of the left and right orderings
		 * @param left_placement, right_placement: input placement strategies of the left and right orderings
		 *						(FALSE:first-to-last, TRUE:last-to-first)
		 * @param left_name, right_name: fancy names for the orderings
		 **/
		template <typename SortAlgT>
		void build_mapping(
			typename SortAlgT::graph_type& graph,
			int left_strategy,
			bool left_placement,
			int right_strategy,
			bool right_placement,
			std::string left_name = "",
			std::string right_name = "");

		template <typename SortAlgT>
		void build_mapping(
			typename SortAlgT::graph_type& graph,
			typename SortAlgT::strategy left_strategy,
			typename SortAlgT::placement left_placement,
			typename SortAlgT::strategy right_strategy,
			typename SortAlgT::placement right_placement,
			std::string left_name = "",
			std::string right_name = "");

		/**
		 * @brief Helper when the two orderings @left_o2n and @right_o2n are known
		 * @param left_o2n: mapping [ORIGINAL graph index]->[LEFT index]
		 * @param right_o2n: mapping [ORIGINAL graph index]->[RIGHT index]
		 * @param left_name, right_name: fancy names for the orderings
		 **/
		void build_mapping(
			const mapping_type &left_o2n,
			const mapping_type &right_o2n,
			std::string left_name = "",
			std::string right_name = "");

		//////////////////////
		// single ordering

		/**
		 * @brief Computes and manages a vertex ordering of a graph, internally stored as:
		 *		RIGHT ordering: new index
		 *		LEFT : the index of the original graph
		 *
		 *		 I. The mappings are available by the functions map_l2r(), map_r2l()
		 *
		 *		 II. Type SortAlgT is a sorting algorithm (typically from GraphFastRootSort<Graph_t>)
		 *
		 * @param right_strategy: input sorting strategies for the ordering (considered to the right)
		 * @param right_placement: input placement strategy right_sorter the ordering (considered to the right)
		 *						(FALSE:first-to-last, TRUE:last-to-first)
		 * @param right_name: fancy name for the ordering (e.g. "MIN_DEG, F2L")
		 * @details: internally left_name is assigned "ORIGINAL GRAPH"
		 **/

		template <typename SortAlgT>
		void build_mapping(
			typename SortAlgT::graph_type& graph,
			int right_strategy,
			bool right_placement,
			std::string right_name = "");

		/**
		 * @brief Helper when the two mappings are known
		 * @param right_n2o: known mapping [RIGHT index]->[ORIGINAL graph index] (more intuitive for single ordering)
		 * @param right_name: fancy name for the ordering (e.g. "MIN_DEG, F2L")
		 **/
		void build_mapping(
			const VertexMapping &right_n2o,
			std::string right_name = "");

		//////////////
		// I/O
		std::ostream &print_mappings(
			print_t type = BOTH,
			std::ostream &out = std::cout);
		std::ostream &print_names(
			print_t type = BOTH,
			std::ostream &out = std::cout);

		///////////////
		// Boolean operations

		/**
		 * @brief checks if the internal mapping state @left_to_right_, @right_to_left_ is consistent
		 **/
		bool is_consistent() const noexcept;

		///////////////////////
		// private interface

	protected:
		void clear()
		{
			left_to_right_.clear();
			right_to_left_.clear();
			left_name_.clear();
			right_name_.clear();
		}
		void reset(size_type vertex_count)
		{
			clear();
			left_to_right_.resize(vertex_count);
			right_to_left_.resize(vertex_count);
		}

		////////////////
		// data members

		VertexMapping left_to_right_;			// mapping between left to right ordering
		VertexMapping right_to_left_;			// mapping between right to left ordering
		std::string left_name_;						// fancy name describing the left ordering
		std::string right_name_;						// fancy name describing the right ordering
	};

} // end of namespace bitgraph

///////////////////////////////////////////////////////////////
// Necessary implementations for templates in header file

namespace bitgraph
{

	template <class BitsetT>
	inline BitsetT &GraphMap::map_l2r(BitsetT &bbl, BitsetT &bbr, bool overwrite) const
	{		
		assert(
			bbl.num_blocks() == bbr.num_blocks()
			&& "bizarre bitsets with different num_blocks - GraphMap::map_l2r");
		assert(
			INDEX_1TO1(left_to_right_.size()) == bbr.num_blocks() 
			&& "not adequate bitset num_blocks for the mapping - GraphMap::map_l2r ");
		

		// cleans bbr if requested
		if (overwrite)
		{
			bbr.erase_bit();
		}

		// scan the bitset bbl and map
		bbl.init_scan(BBObject::NON_DESTRUCTIVE);
		int v = BBObject::noBit;
		while ((v = bbl.next_bit()) != BBObject::noBit)
		{
			bbr.set_bit(left_to_right_[v]);
		}

		return bbr;
	}

	template <class BitsetT>
	inline BitsetT &GraphMap::map_r2l(
		BitsetT &bbl,
		BitsetT &bbr, 
		bool overwrite) const
	{
		
		assert(
			bbl.num_blocks() == bbr.num_blocks()
			&& "bizarre bitsets with different num_blocks - GraphMap::map_l2r");
		assert(
			INDEX_1TO1(left_to_right_.size()) == bbr.num_blocks() 
			&& "not adequate bitset num_blocks for the mapping - GraphMap::map_l2r ");
	

		// cleans bbr if requested
		if (overwrite)
		{
			bbl.erase_bit();
		}

		// sets bitscanning configuration
		bbr.init_scan(BBObject::NON_DESTRUCTIVE);
		int v = BBObject::noBit;
		while ((v = bbr.next_bit()) != BBObject::noBit)
		{
			bbl.set_bit(right_to_left_[v]);
		}

		return bbl;
	}

	inline
		bool GraphMap::is_consistent() const noexcept
	{
		for (vertex_t v = 0; v < left_to_right_.size(); ++v)
		{
			if (v != right_to_left_[left_to_right_[v]])
			{
				return false;
			}
		}

		return true;
	}

	template <class SortAlgT>
	inline void GraphMap::build_mapping(
		typename SortAlgT::graph_type &graph,
		int left_strategy, 
		bool left_placement,
		int right_strategy, 
		bool right_placement, 
		std::string left_name, std::string right_name)
	{
		mapping_type left_o2n, left_n2o, right_o2n, right_n2o;
		auto vertex_count = graph.num_vertices();

		reset(vertex_count);

		// determine left sorting
		SortAlgT left_sorter(graph);
		left_o2n = left_sorter.new_order(left_strategy, left_placement /* false:first to last*/, true /* o2n*/); // VertexMapping new_order(int alg, bool ltf = true, bool o2n = true);
		left_n2o = Decode::reverse(left_o2n);

		// determine right sorting
		SortAlgT right_sorter(graph);
		right_o2n = right_sorter.new_order(right_strategy, right_placement /* false:first to last*/, true /* o2n */);
		right_n2o = Decode::reverse(right_o2n);

		// determines direct and reverse mappings independently
		for (auto v = 0; v < vertex_count; v++)
		{
			left_to_right_[v] = right_o2n[left_n2o[v]]; // l->r
		}
		for (auto v = 0; v < vertex_count; v++)
		{
			right_to_left_[v] = left_o2n[right_n2o[v]]; // r->l
		}

		left_name_ = std::move(left_name);
		right_name_ = std::move(right_name);

		/*if (!is_consistent()) {
			LOG_ERROR("L2R and R2L are inconsistent orderings - GraphMap::build_mapping (2 ord...)");
			LOG_ERROR("exiting...");
			std::exit(EXIT_FAILURE);
		}*/

		// I/O
		/*cout<<"N2O_L: "; bitgraph::_stl::print_collection(left_n2o, cout, true);
		  cout<<"O2N_L: "; bitgraph::_stl::print_collection(left_o2n, cout, true);
		  cout<<"N2O_R: "; bitgraph::_stl::print_collection(right_n2o, cout, true);
		  cout<<"O2N_R: "; bitgraph::_stl::print_collection(right_o2n, cout, true);
		  print_mappings();*/
	}

	template <class SortAlgT>
	inline void GraphMap::build_mapping(
		typename SortAlgT::graph_type& graph,
		typename SortAlgT::strategy left_strategy,
		typename SortAlgT::placement left_placement,
		typename SortAlgT::strategy right_strategy,
		typename SortAlgT::placement right_placement,
		std::string left_name,
		std::string right_name)
	{
		this->build_mapping<SortAlgT>(
			graph,
			static_cast<int>(left_strategy),
			left_placement == SortAlgT::placement::last_to_first,
			static_cast<int>(right_strategy),
			right_placement == SortAlgT::placement::last_to_first,
			std::move(left_name),
			std::move(right_name));
	}

	template <typename SortAlgT>
	void GraphMap::build_mapping(
		typename SortAlgT::graph_type &graph, 
		int right_strategy, 
		bool right_placement,
		std::string right_name)
	{

		auto vertex_count = graph.num_vertices();

		reset(vertex_count);

		// determine left sorting
		SortAlgT left_sorter(graph);
		left_to_right_ = left_sorter.new_order(right_strategy, right_placement /* false:first to last */, true /* o2n */);
		right_to_left_ = Decode::reverse(left_to_right_);

		left_name_ = "ORIGINAL GRAPH";
		right_name_ = std::move(right_name);

		/*if (!is_consistent()) {
			LOG_ERROR("L2R and R2L are inconsistent orderings - GraphMap::build_mapping(single ord...)");
			LOG_ERROR("exiting...");
			std::exit(EXIT_FAILURE);
		}*/

		// return 0;
	}

	inline void GraphMap::build_mapping(
		const mapping_type &left_o2n,
		const mapping_type &right_o2n,
		std::string left_name, std::string right_name)
	{
		assert(left_o2n.size() == right_o2n.size() && "different size orderings - GraphMap::build_mapping");

		const auto vertex_count = left_o2n.size();
		const mapping_type left_n2o = OrderingDecoder::inverse_ordering(left_o2n);

		reset(vertex_count);

		// l->r: left index -> original index -> right index
		for (size_type v = 0; v < vertex_count; ++v)
		{
			left_to_right_[v] = right_o2n[left_n2o[v]];
		}

		// r->l is the inverse of l->r
		for (size_type v = 0; v < vertex_count; ++v)
		{
			right_to_left_[left_to_right_[v]] = static_cast<vertex_t>(v);
		}

		left_name_ = std::move(left_name);
		right_name_ = std::move(right_name);
	}

	inline void GraphMap::build_mapping(const VertexMapping &right_n2o, std::string right_name)
	{

		left_to_right_ = Decode::reverse(right_n2o);
		right_to_left_ = right_n2o;

		left_name_ = "ORIGINAL GRAPH";
		right_name_ = std::move(right_name);

		// return 0;
	}

	inline std::ostream &GraphMap::print_mappings(print_t type, std::ostream &o)
	{

		switch (type)
		{
		case L2R:
			o << "\n*****************" << std::endl;
			o << "L->R" << std::endl;
			utils::print_collection(left_to_right_, o, true);
			o << "\n*****************" << std::endl;
			break;
		case R2L:
			o << "\n*****************" << std::endl;
			o << "R->L" << std::endl;
			utils::print_collection(right_to_left_, o, true);
			o << "******************" << std::endl;
			break;
		case BOTH:
			o << "\n*****************" << std::endl;
			o << "L->R and R->L" << std::endl;
			utils::print_collection(left_to_right_, o, true);
			utils::print_collection(right_to_left_, o, true);
			o << "*****************" << std::endl;
			break;
		default:
			LOG_WARNING("bad printing type - GraphMap::print_mappings");
		}

		return o;
	}

	inline std::ostream &GraphMap::print_names(print_t type, std::ostream &o)
	{

		switch (type)
		{
		case L2R:
			o << "L:" << left_name_;
			break;
		case R2L:
			o << "R:" << right_name_;
			break;
		case BOTH:
			o << "L:" << left_name_;
			o << std::endl;
			o << "R:" << right_name_;
			break;
		default:
			LOG_WARNING("bad printing type - GraphMap::print_names");
		}

		return o;
	}

} // end of namespace bitgraph

#endif // BITGRAPH_VERTEX_MAPPING_H
