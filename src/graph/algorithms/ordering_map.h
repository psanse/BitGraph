/**
 * @file ordering_map.h
 * @brief Defines OrderingMap, which maps vertices between two orderings of a graph.
 *
 * @details OrderingMap stores a pair of vertex orderings as two inverse
 * permutations, left-to-right and right-to-left, and converts single vertices
 * and bitsets of vertices between them. The original graph indices act as the
 * intermediate index space, so it supports two use cases:
 *   - two orderings, each computed from the original graph, and
 *   - a single ordering, where the left space is the original graph.
 *
 * Orderings are computed by a sorting algorithm passed as a template parameter
 * (typically GraphFastRootSort), but the class is not restricted to it.
 * The class is independent of any concrete graph type.
 *
 * @author Pablo San Segundo
 * @date Created 14/08/2017 (two orderings); extended to a single ordering 01/10/2017
 * @date Imported from the COPT framework in 2024
 * @date Last updated: 04/10/2026
 *
 * @see GraphFastRootSort, OrderingDecoder
 */
#ifndef BITGRAPH_GRAPH_ALGORITHMS_ORDERING_MAP_H
#define	BITGRAPH_GRAPH_ALGORITHMS_ORDERING_MAP_H

#include "utils/logger.h"
#include "utils/collection_utils.h"
#include "bitscan/bbconfig.h"			//for INDEX_1_TO_1 macro
#include "bitscan/bbobject.h"
#include "decode.h"
#include <iostream>
#include <vector>
#include <string>

namespace bitgraph {

	/**
	 * @brief Maps vertices between two orderings of a graph.
	 *
	 * @details Stores a left ordering and a right ordering as two inverse
	 * permutations of the vertex set: left-to-right (l2r) and right-to-left (r2l).
	 * Vertices and bitsets of vertices can be translated in either direction.
	 *
	 * The original graph indices are the intermediate index space. Two use cases
	 * are supported:
	 *   - Two orderings: both computed from the original graph
	 *     (left index -> original index -> right index).
	 *   - Single ordering: the left space is the original graph ("ORIGINAL GRAPH")
	 *     and the right space is the new ordering.
	 *
	 * Mappings are built with the build_mapping() overloads, either from a sorting
	 * algorithm (typically GraphFastRootSort) or from known orderings. The class
	 * does not hold a reference to any graph.
	 *
	 * @par Example
	 * @code
	 * using Sorter = GraphFastRootSort<Ugraph<BBScan>>;
	 * OrderingMap map;
	 * map.build_mapping<Sorter>(g,
	 *     Sorter::strategy::min_degeneracy, Sorter::placement::first_to_last,
	 *     Sorter::strategy::max, Sorter::placement::first_to_last,
	 *     "MIN_DEGEN, F2L", "MAX, F2L");
	 * vertex_t w = map.map_left_to_right(v);   // vertex v in the left space -> right space
	 * @endcode
	 *
	 * @note The mappings are plain vectors: copying is O(n), and concurrent
	 *       const access is safe but concurrent modification is not.
	 * @see GraphFastRootSort, OrderingDecoder
	 */


	class OrderingMap
	{

		using ordering_type = bitgraph::vertex_ordering;
		using size_type = ordering_type::size_type;

		///////////////////////
		// public interface
	public:
		enum class print_mode
		{
			LeftToRight = 0,
			RightToLeft,
			Both
		};	

		///////////////////////
		// setters and getters

		size_type size() const noexcept{
			 return left_to_right_.size();
		}		
		
		const ordering_type &left_to_right() const noexcept { 
			return left_to_right_;
		 }
		const ordering_type &right_to_left() const noexcept {
			 return right_to_left_;
		 }
		const std::string &left_name() const noexcept { return left_name_; }
		const std::string &right_name() const noexcept { return right_name_; }


		void set_left_to_right(
			ordering_type left_to_right,
			std::string left_name = {},
			std::string right_name = {})
		{
			ordering_type right_to_left =
				OrderingDecoder::inverse_ordering(left_to_right);

			left_to_right_ = std::move(left_to_right);
			right_to_left_ = std::move(right_to_left);

			left_name_ = std::move(left_name);
			right_name_ = std::move(right_name);
		}
	
		////////////////
		// mapping getters

		vertex_t map_left_to_right(vertex_t v) const noexcept { 
			return left_to_right_[v];
		}
		vertex_t map_right_to_left(vertex_t v) const noexcept { 
			return right_to_left_[v]; 
		}

		/**
		 * @brief maps a (bit) set of vertices (bbl) to a (bit) set of vertices (bbr)
		 * @param bbleft: input bitset of vertices in the space of the left ordering
		 * @param bbright: output bitset of vertices in the space of the right ordering
		 * @param overwrite: if TRUE, bbr is erased before mapping
		 **/
		template <class BitsetT>
		BitsetT &map_left_to_right(
			BitsetT &bbleft,
			BitsetT &bbright,
			bool overwrite = true) const;

		/**
		 * @brief maps a (bit) set of vertices (bbr) to a (bit) set of vertices (bbl)
		 * @param bbleft: output bitset of vertices in the space of the left ordering
		 * @param bbright: input bitset of vertices in the space of the right ordering
		 * @param overwrite: if TRUE, bbr is erased before mapping
		 **/
		template <class BitsetT>
		BitsetT &map_right_to_left(
			BitsetT &bbleft,
			BitsetT &bbright,
			bool overwrite = true) const;

		////////////////////
		// build mapping operations

		/**
		 * @brief Builds the left-to-right and right-to-left vertex mappings between two
		 *        orderings of @p graph, both computed by the sorting algorithm @p SortAlgT.
		 *
		 * @details The original graph indices act as the intermediate index space:
		 * each ordering is computed independently from the original graph, and the
		 * mappings are composed as left index -> original index -> right index.
		 * Any previous state is replaced. The resulting mappings are available through
		 * left_to_right(), right_to_left(), map_left_to_right() and map_right_to_left().
		 *
		 * @tparam SortAlgT Sorting algorithm type, typically GraphFastRootSort<GraphT>.
		 *                  It must define graph_type, be constructible from a graph
		 *                  and provide new_order(int strategy, bool last_to_first, bool o2n).
		 *
		 * @param graph           Graph to be ordered.
		 * @param left_strategy   Sorting strategy of the left ordering (a SortAlgT strategy value).
		 * @param left_placement  Placement of the left ordering: false = first-to-last,
		 *                        true = last-to-first.
		 * @param right_strategy  Sorting strategy of the right ordering.
		 * @param right_placement Placement of the right ordering (same convention as @p left_placement).
		 * @param left_name       Descriptive name of the left ordering (e.g. "MAX_DEG, F2L").
		 * @param right_name      Descriptive name of the right ordering (e.g. "MIN_DEG, F2L").
		 *
		 * @post size() == graph.num_vertices() and is_consistent() is true.
		 *
		 * @see build_mapping() overload taking typed strategy and placement enums.
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

		/**
		 * @brief Type-safe overload of build_mapping() taking the strategy and
		 *        placement enums of @p SortAlgT instead of int and bool.
		 *
		 * @details Equivalent to the int/bool overload, with placement::last_to_first
		 * mapped to true and placement::first_to_last to false.
		 *
		 * @tparam SortAlgT Sorting algorithm type defining the nested enum classes
		 *                  strategy and placement (e.g. GraphFastRootSort<GraphT>).
		 *
		 * @param graph           Graph to be ordered.
		 * @param left_strategy   Sorting strategy of the left ordering.
		 * @param left_placement  Placement of the left ordering.
		 * @param right_strategy  Sorting strategy of the right ordering.
		 * @param right_placement Placement of the right ordering.
		 * @param left_name       Descriptive name of the left ordering.
		 * @param right_name      Descriptive name of the right ordering.
		 *
		 * @post size() == graph.num_vertices() and is_consistent() is true.
		 **/		template <typename SortAlgT>
		void build_mapping(
			typename SortAlgT::graph_type& graph,
			typename SortAlgT::strategy left_strategy,
			typename SortAlgT::placement left_placement,
			typename SortAlgT::strategy right_strategy,
			typename SortAlgT::placement right_placement,
			std::string left_name = "",
			std::string right_name = "");

		/**
		 * @brief Builds the left-to-right and right-to-left vertex mappings from two
		 *        known orderings of the same graph.
		 *
		 * @details Both orderings are expressed relative to the original graph, which
		 * acts as the intermediate index space:
		 * @code
		 *   left index --(left_n2o)--> original index --(right_o2n)--> right index
		 * @endcode
		 * where left_n2o is the inverse of @p left_o2n. The right-to-left mapping is
		 * the inverse of the left-to-right mapping. Any previous state, including the
		 * ordering names, is replaced.
		 *
		 * @param left_o2n   Ordering [ORIGINAL graph index] -> [LEFT index].
		 * @param right_o2n  Ordering [ORIGINAL graph index] -> [RIGHT index].
		 * @param left_name  Descriptive name of the left ordering (e.g. "MAX_DEG, F2L").
		 * @param right_name Descriptive name of the right ordering (e.g. "MIN_DEG, F2L").
		 *
		 * @pre @p left_o2n and @p right_o2n are permutations of [0, n) of the same
		 *      size n. Only the size is checked (by assert, in debug builds).
		 * @post size() == n and is_consistent() is true.
		 **/	
		void build_mapping(
			const ordering_type &left_o2n,
			const ordering_type &right_o2n,
			std::string left_name = "",
			std::string right_name = "");

		//////////////////////
		// single ordering

		/**
		 * @brief Builds the mappings between the original graph and a single new ordering
		 *        of @p graph computed by the sorting algorithm @p SortAlgT.
		 *
		 * @details The LEFT space is the original graph (identity ordering) and the RIGHT
		 * space is the new ordering, so map_left_to_right() maps an original vertex to its new index
		 * and map_right_to_left() maps a new index back to the original vertex. The left name is set
		 * to "ORIGINAL GRAPH". Any previous state is replaced.
		 *
		 * @tparam SortAlgT Sorting algorithm type, typically GraphFastRootSort<GraphT>.
		 *                  It must define graph_type, be constructible from a graph
		 *                  and provide new_order(int strategy, bool last_to_first, bool o2n).
		 *
		 * @param graph           Graph to be ordered.
		 * @param right_strategy  Sorting strategy of the new ordering (a SortAlgT strategy value).
		 * @param right_placement Placement of the new ordering: false = first-to-last,
		 *                        true = last-to-first.
		 * @param right_name      Descriptive name of the new ordering (e.g. "MIN_DEG, F2L").
		 *
		 * @post size() == graph.num_vertices() and is_consistent() is true.
		 *
		 * @see build_mapping(const ordering_type&, std::string) to use a known ordering.
		 **/
		template <typename SortAlgT>
		void build_mapping(
			typename SortAlgT::graph_type& graph,
			int right_strategy,
			bool right_placement,
			std::string right_name = "");

		/**
		 * @brief Builds the mappings between the original graph and a known single ordering.
		 *
		 * @details The LEFT space is the original graph and the RIGHT space is the given
		 * ordering. The left name is set to "ORIGINAL GRAPH". Any previous state is replaced.
		 *
		 * @param right_n2o  Ordering [RIGHT index] -> [ORIGINAL graph index], the natural
		 *                   direction when a single ordering is given.
		 * @param right_name Descriptive name of the ordering (e.g. "MIN_DEG, F2L").
		 *
		 * @pre @p right_n2o is a permutation of [0, n).
		 * @post size() == n and is_consistent() is true.
		 **/	
		void build_mapping(
			const ordering_type &right_n2o,
			std::string right_name = "");

		//////////////
		// I/O

		std::ostream &print_mappings (
			print_mode type = print_mode::Both,
			std::ostream &out = std::cout) const;

		std::ostream &print_names  (
			print_mode type = print_mode::Both,
			std::ostream &out = std::cout) const;

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

		ordering_type left_to_right_;			// mapping between left to right ordering
		ordering_type right_to_left_;			// mapping between right to left ordering
		std::string left_name_;						// fancy name describing the left ordering
		std::string right_name_;						// fancy name describing the right ordering
	};

} // end of namespace bitgraph

///////////////////////////////////////////////////////////////
// Necessary implementations for templates in header file

namespace bitgraph
{

	template <class BitsetT>
	inline BitsetT &OrderingMap::map_left_to_right(BitsetT &bbl, BitsetT &bbr, bool overwrite) const
	{		
		assert(
			bbl.num_blocks() == bbr.num_blocks()
			&& "bizarre bitsets with different num_blocks - OrderingMap::map_left_to_right");
		assert(
			INDEX_1TO1(static_cast<int>(left_to_right_.size())) == bbr.num_blocks()
			&& "not adequate bitset num_blocks for the mapping - OrderingMap::map_left_to_right ");
		

		// cleans bbr if requested
		if (overwrite)
		{
			bbr.erase_bit();
		}

		// scan the bitset bbl and map
		bbl.init_scan(BBObject::NON_DESTRUCTIVE);
		vertex_t v = BBObject::noBit;
		while ((v = bbl.next_bit()) != BBObject::noBit)
		{
			bbr.set_bit(left_to_right_[v]);
		}

		return bbr;
	}

	template <class BitsetT>
	inline BitsetT &OrderingMap::map_right_to_left(
		BitsetT &bbl,
		BitsetT &bbr, 
		bool overwrite) const
	{
		
		assert(
			bbl.num_blocks() == bbr.num_blocks()
			&& "bizarre bitsets with different num_blocks - OrderingMap::map_right_to_left");
		assert(
			INDEX_1TO1(static_cast<int>(left_to_right_.size())) == bbr.num_blocks()
			&& "not adequate bitset num_blocks for the mapping - OrderingMap::map_right_to_left ");
	

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
		bool OrderingMap::is_consistent() const noexcept
	{
		for (size_type v = 0; v < left_to_right_.size(); ++v)
		{
			if (static_cast<vertex_t>(v) != right_to_left_[left_to_right_[v]])
			{
				return false;
			}
		}

		return true;
	}

	template <class SortAlgT>
	inline void OrderingMap::build_mapping(
		typename SortAlgT::graph_type &graph,
		int left_strategy, 
		bool left_placement,
		int right_strategy, 
		bool right_placement, 
		std::string left_name, std::string right_name)
	{
		ordering_type left_o2n, left_n2o, right_o2n, right_n2o;
		auto vertex_count = graph.num_vertices();

		reset(vertex_count);

		// determine left sorting
		SortAlgT left_sorter(graph);
		left_o2n = left_sorter.new_order(left_strategy, left_placement /* false:first to last*/, true /* o2n*/); // VertexMapping new_order(int alg, bool ltf = true, bool o2n = true);
		left_n2o = OrderingDecoder::inverse_ordering(left_o2n);

		// determine right sorting
		SortAlgT right_sorter(graph);
		right_o2n = right_sorter.new_order(right_strategy, right_placement /* false:first to last*/, true /* o2n */);
		right_n2o = OrderingDecoder::inverse_ordering(right_o2n);

		// determines direct and reverse mappings independently
		for (size_type = 0; v < vertex_count; v++)
		{
			left_to_right_[v] = right_o2n[left_n2o[v]]; // l->r
		}
		for (size_type v = 0; v < vertex_count; v++)
		{
			right_to_left_[v] = left_o2n[right_n2o[v]]; // r->l
		}

		left_name_ = std::move(left_name);
		right_name_ = std::move(right_name);

		/*if (!is_consistent()) {
			LOG_ERROR("L2R and R2L are inconsistent orderings - OrderingMap::build_mapping (2 ord...)");
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
	inline void OrderingMap::build_mapping(
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
	void OrderingMap::build_mapping(
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
		right_to_left_ = OrderingDecoder::inverse_ordering(left_to_right_);

		left_name_ = "ORIGINAL GRAPH";
		right_name_ = std::move(right_name);

		/*if (!is_consistent()) {
			LOG_ERROR("L2R and R2L are inconsistent orderings - OrderingMap::build_mapping(single ord...)");
			LOG_ERROR("exiting...");
			std::exit(EXIT_FAILURE);
		}*/

		// return 0;
	}

	inline void OrderingMap::build_mapping(
		const ordering_type &left_o2n,
		const ordering_type &right_o2n,
		std::string left_name, std::string right_name)
	{
		assert(left_o2n.size() == right_o2n.size() && "different size orderings - OrderingMap::build_mapping");

		const auto vertex_count = left_o2n.size();
		const ordering_type left_n2o = OrderingDecoder::inverse_ordering(left_o2n);

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

	inline void OrderingMap::build_mapping(
		const ordering_type &right_n2o,
		std::string right_name)
	{

		left_to_right_ = OrderingDecoder::inverse_ordering(right_n2o);
		right_to_left_ = right_n2o;

		left_name_ = "ORIGINAL GRAPH";
		right_name_ = std::move(right_name);

		// return 0;
	}

	inline std::ostream &OrderingMap::print_mappings(
		print_mode type,
		std::ostream &o) const
	{

		switch (type)
		{
		case print_mode::LeftToRight:
			o << "\n*****************" << std::endl;
			o << "L->R" << std::endl;
			utils::print_collection(left_to_right_, o, true);
			o << "\n*****************" << std::endl;
			break;
		case print_mode::RightToLeft:
			o << "\n*****************" << std::endl;
			o << "R->L" << std::endl;
			utils::print_collection(right_to_left_, o, true);
			o << "******************" << std::endl;
			break;
		case print_mode::Both:
			o << "\n*****************" << std::endl;
			o << "L->R and R->L" << std::endl;
			utils::print_collection(left_to_right_, o, true);
			utils::print_collection(right_to_left_, o, true);
			o << "*****************" << std::endl;
			break;
		default:
			LOG_WARNING("bad printing type - OrderingMap::print_mappings");
		}

		return o;
	}

	inline std::ostream &OrderingMap::print_names(
		print_mode type, 
		std::ostream &o) const
	{

		switch (type)
		{
		case print_mode::LeftToRight:
			o << "L:" << left_name_;
			break;
		case print_mode::RightToLeft:
			o << "R:" << right_name_;
			break;
		case print_mode::Both:
			o << "L:" << left_name_;
			o << std::endl;
			o << "R:" << right_name_;
			break;
		default:
			LOG_WARNING("bad printing type - OrderingMap::print_names");
		}

		return o;
	}

	// Alias for backward compatibility with the previous name of the class
	using GraphMap = OrderingMap;

} // end of namespace bitgraph

#endif // BITGRAPH_GRAPH_ALGORITHMS_ORDERING_MAP_H
