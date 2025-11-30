/**
* @file graph_map.h
* @brief header for GraphMap class that manages pairs of vertex orderings
* @update conversions between two orderings of vertices (typically encoded by GraphFastRootSort) (14/8/17)
* @update extended to inlcude mapping to a single ordering (1/10/17)
* @date: imported from COPT framework in 2024, last_update 27/01/25
* @details: GraphMap is conceived as a wrapper for GraphFastRootSort, but it is not restricted to it due to its generic template design
* @dev pss
**/

#ifndef __GRAPH_MAPPINGS_H__
#define	__GRAPH_MAPPINGS_H__

#include "utils/logger.h"
#include "utils/common.h"
#include "bitscan/bbconfig.h"			//for INDEX_1_TO_1 macro
#include "decode.h"
#include <iostream>
#include <vector>
#include <string>

namespace bitgraph {
	namespace _impl {
				
		///////////////////////
		// 
		// GraphMap for managing pairs of vertex orderings	
		//
		///////////////////////

		class GraphMap {

		///////////////////////
		//public interface
		public:
			enum print_t { L2R = 0, R2L, BOTH };		//streaming configuration

			///////////////////////
			//setters and getters

			int size() { return l2r_.size(); }
			vint& get_l2r() { return l2r_; }
			vint& get_r2l() { return r2l_; }
			const vint& get_l2r() const { return l2r_; }
			const vint& get_r2l() const { return r2l_; }
			std::string nameL() { return nameL_; }
			std::string nameR() { return nameR_; }

			//sets mapping (no need to build it)
			void set_l2r(vint& l, std::string name) { l2r_ = l; nameL_ = name; }
			void set_r2l(vint& r, std::string name) { r2l_ = r; nameR_ = name; }

			////////////////
			// mapping getters

			int map_l2r(int v) const { return l2r_[v]; }
			int map_r2l(int v) const { return r2l_[v]; }

			/**
			* @brief maps a (bit) set of vertices (bbl) to a (bit) set of vertices (bbr)
			* @param bbl: input bitset of vertices in the space of the left ordering
			* @param bbr: output bitset of vertices in the space of the right ordering
			* @param overwrite: if TRUE, bbr is erased before mapping
			**/
			template<class bitset_t>
			bitset_t& map_l2r(bitset_t& bbl, bitset_t& bbr, bool overwrite = true)			const;

			/**
			* @brief maps a (bit) set of vertices (bbr) to a (bit) set of vertices (bbl)
			* @param bbl: output bitset of vertices in the space of the left ordering
			* @param bbr: input bitset of vertices in the space of the right ordering
			* @param overwrite: if TRUE, bbr is erased before mapping
			**/
			template<class bitset_t>
			bitset_t& map_r2l(bitset_t& bbl, bitset_t& bbr, bool overwrite = true)			const;

			//TODO - extend to other set representations

		////////////////////
		// build mapping operations 

			/**
			* @brief Computes mapping between two different vertex orderings of a graph, 
			*		 internally stored as left and right orderings.
			*		 (the indexes of the original graph can be seen as in-between :-))
			*
			*		 I. The mappings are available by the functions map_l2r(), map_r2l()
			*
			*		 II. Type Alg_t is a sorting algorithm (e.g. GraphFastRootSort<Graph_t>)
			*
			* @param lhs_s, rhs_s: input sorting strategies of the left and right orderings
			* @param lhs_p, rhs_p: input placement strategies of the left and right orderings 
			*						(FALSE:first-to-last, TRUE:last-to-first)
			* @param lhs_name, rhs_name: fancy names for the orderings
			**/
			template< typename Alg_t>
			void build_mapping(typename Alg_t::_gt&, int lhs_s, bool lhs_p, int rhs_s, bool rhs_p,
										std::string lhs_name = "", std::string rhs_name = "");

			/**
			* @brief Helper when the two mappings @lhs_o2n  and @rhs_o2n are knownt
			* @param lhs_o2n: known mapping in [OLD]->[NEW] format 
			* @param rhs_o2n: known mapping in [OLD]->[NEW] format 
			* @param lhs_name, rhs_name: fancy names for the orderings
			**/
			void build_mapping(const vint& lhs_o2n, const vint& rhs_o2n, std::string lhs_name = "", std::string rhs_name = "");

			//////////////////////
			//single ordering

			/**
			* @brief Computes and manages a vertex ordering of a graph, internally stored as:
			*		RIGHT ordering: new index
			*		LEFT : the index of the original graph
			*
			*		 I. The mappings are available by the functions map_l2r(), map_r2l()
			*
			*		 II. Type Alg_t is a sorting algorithm (e.g. GraphFastRootSort<Graph_t>)
			*
			* @param lhs_s, rhs_s: input sorting strategies of the left and right orderings
			* @param lhs_p, rhs_p: input placement strategies of the left and right orderings
			*						(FALSE:first-to-last, TRUE:last-to-first)
			* @param rhs_name: fancy name for the ordering (e.g. "MIN_DEG, F2L")
			* @details: internally lhs_name is assigned "ORIGINAL GRAPH"
			**/

			template< typename Alg_t >
			void build_mapping(typename Alg_t::_gt&, int rhs_s, bool rhs_p, std::string rhs_name = "") ;

			/**
			* @brief Helper when the two mappings are known
			* @param rhs_n2o: known mapping in [NEW]->[OLD] format (which is more intuitive for single ordering)
			* @param rhs_name: fancy name for the ordering (e.g. "MIN_DEG, F2L")
			**/
			void build_mapping(const vint& rhs_n2o, std::string rhs_name = "");

			//////////////
			//I/O
			std::ostream& print_mappings(print_t type = BOTH, std::ostream& o = std::cout);
			std::ostream& print_names(print_t type = BOTH, std::ostream& o = std::cout);

			///////////////
			//Boolean operations

			/**
			* @brief checks if the internal mapping state @l2r_, @r2l_ is consistent
			**/
			bool is_consistent();

		///////////////////////
		//private interface

		protected:
			void clear() { l2r_.clear(); r2l_.clear(); nameL_.clear(); nameR_.clear(); }
			void reset(std::size_t NV) { clear(); 	l2r_.resize(NV); r2l_.resize(NV); }

			////////////////
			// data members

			vint l2r_, r2l_;						//mapping between left and right ordering
			std::string nameL_;						//fancy name describing the left ordering
			std::string nameR_;						//fancy name describing the right ordering	
		};
	}//end of namespace _impl

	using _impl::GraphMap;					//alias for the GraphMap class

}//end of namespace bitgraph
	

///////////////////////////////////////////////////////////////
// Necessary implementations for templates in header file

namespace bitgraph{

	template<class bitset_t>
	inline
		bitset_t& GraphMap::map_l2r(bitset_t& bbl, bitset_t& bbr, bool overwrite) const {

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		assert(bbl.capacity() == bbr.capacity() && "bizarre bitsets with different capacity - GraphMap::map_l2r");
		assert(INDEX_1TO1(l2r_.size()) == bbr.capacity() && "not adequate bitset capacity for the mapping - GraphMap::map_l2r ");
		///////////////////////////////////////////////////////////////////////////////////////////////////////////

		//cleans bbr if requested
		if (overwrite) { bbr.erase_bit(); }
		
		//scan the bitset bbl and map
		bbl.init_scan(BBObject::NON_DESTRUCTIVE) != bbo::noBit;
		int v = BBObject::noBit;
		while ( (v = bbl.next_bit()) != BBObject::noBit) {
			bbr.set_bit(l2r_[v]);
		}
		

		return bbr;
	}

	template<class bitset_t>
	inline
		bitset_t& GraphMap::map_r2l(bitset_t& bbl, bitset_t& bbr, bool overwrite) const {

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		assert(bbl.capacity() == bbr.capacity() && "bizarre bitsets with different capacity - GraphMap::map_l2r");
		assert(INDEX_1TO1(l2r_.size()) == bbr.capacity() && "not adequate bitset capacity for the mapping - GraphMap::map_l2r ");
		///////////////////////////////////////////////////////////////////////////////////////////////////////////

		//cleans bbr if requested
		if (overwrite) { bbl.erase_bit(); }

		//sets bitscanning configuration
		bbr.init_scan(BBObject::NON_DESTRUCTIVE) != bbo::noBit;
		int v = BBObject::noBit;
		while ((v = bbr.next_bit()) != BBObject::noBit) {
			bbl.set_bit(r2l_[v]);
		}
		
		return bbl;
	}

	inline
		bool GraphMap::is_consistent() {

		for (auto v = 0; v < l2r_.size(); ++v) {
			if (v != r2l_[l2r_[v]]) {
				return false;
			}
		}

		return true;
	}

	template<class Alg_t>
	inline
	void GraphMap::build_mapping(typename Alg_t::_gt& g, int slhs, bool plhs, int srhs, bool prhs,
															std::string lhs_name, std::string rhs_name)
	{
		vint lhs_o2n, lhs_n2o, rhs_o2n, rhs_n2o;

		auto NV = g.number_of_vertices();

		reset(NV);

		//determine sorting lhs
		Alg_t gol(g);
		lhs_o2n = gol.new_order(slhs, plhs /* false:first to last*/, true /* o2n*/);				 // vint new_order(int alg, bool ltf = true, bool o2n = true);
		lhs_n2o = Decode::reverse(lhs_o2n);

		//determine sorting rhs
		Alg_t gor(g);
		rhs_o2n = gor.new_order(srhs, prhs/* false:first to last*/, true /* o2n */);
		rhs_n2o = Decode::reverse(rhs_o2n);

		//determines direct and reverse mappings independently
		for (auto v = 0; v < NV; v++) {
			l2r_[v] = rhs_o2n[lhs_n2o[v]];				// l->r
		}
		for (auto v = 0; v < NV; v++) {
			r2l_[v] = lhs_o2n[rhs_n2o[v]];				// r->l 
		}

		nameL_ = std::move(lhs_name);
		nameR_ = std::move(rhs_name);
			
		
		/*if (!is_consistent()) {
			LOG_ERROR("L2R and R2L are inconsistent orderings - GraphMap::build_mapping (2 ord...)");
			LOG_ERROR("exiting...");
			std::exit(EXIT_FAILURE);
		}*/

		//I/O
	  /*cout<<"N2O_L: "; bitgraph::_stl::print_collection(lhs_n2o, cout, true);
		cout<<"O2N_L: "; bitgraph::_stl::print_collection(lhs_o2n, cout, true); 
		cout<<"N2O_R: "; bitgraph::_stl::print_collection(rhs_n2o, cout, true);
		cout<<"O2N_R: "; bitgraph::_stl::print_collection(rhs_o2n, cout, true);
		print_mappings();*/		 
	}

	template< typename Alg_t>
	void GraphMap::build_mapping(typename Alg_t::_gt& g, int srhs, bool prhs, std::string rhs_name)  {

		auto NV = g.number_of_vertices();

		reset(NV);

		//determine sorting lhs
		Alg_t gol(g);
		l2r_ = gol.new_order(srhs, prhs /* false:first to last */ , true /* o2n */);
		r2l_ = Decode::reverse(l2r_);

		nameL_ = "ORIGINAL GRAPH";
		nameR_ = std::move(rhs_name);
	
		/*if (!is_consistent()) {
			LOG_ERROR("L2R and R2L are inconsistent orderings - GraphMap::build_mapping(single ord...)");
			LOG_ERROR("exiting...");
			std::exit(EXIT_FAILURE);
		}*/
			
		//return 0;
	}

	inline
		void GraphMap::build_mapping(const vint& lhs_o2n, const vint& rhs_o2n, std::string lhs_name, std::string rhs_name) {

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		assert(lhs_o2n.size() == rhs_o2n.size() && "ERROR: different size orderings - GraphMap::build_mapping");
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		vint lhs_n2o, rhs_n2o;

		auto NV = lhs_o2n.size();

		reset(NV);

		//determine sorting lhs
		lhs_n2o = Decode::reverse(lhs_o2n);

		//determine sorting rhs
		rhs_n2o = Decode::reverse(rhs_o2n);

		//determines direct and reverse mappings independently
		for (int v = 0; v < NV; v++) {
			l2r_[v] = rhs_o2n[lhs_n2o[v]];				// l->r 
		}
		for (int v = 0; v < NV; v++) {
			r2l_[v] = lhs_o2n[rhs_n2o[v]];				// r->l 
		}

		nameL_ = std::move(lhs_name);
		nameR_ = std::move(rhs_name);


		//assert
		/*if (!is_consistent()) {
			LOG_ERROR("bad ordering - GraphMap::build_mapping");
			return -1;
		}*/

		//I/O
		//cout<<"N2O_D"; com::stl::print_collection(n2o_d); cout<<endl;
		//cout<<"O2N_D";com::stl::print_collection(o2n_d); cout<<endl;
		//cout<<"O2N_W";com::stl::print_collection(o2n_w); cout<<endl;
		//cout<<"N2O_W";com::stl::print_collection(n2o_w); cout<<endl;
		//print_mappings();
		
	}

	inline
		void GraphMap::build_mapping(const vint& rhs_n2o, std::string lhs_name) {

		l2r_ = Decode::reverse(rhs_n2o);
		r2l_ = rhs_n2o;

		nameL_ = "ORIGINAL GRAPH";
		nameR_ = std::move(lhs_name);

		//return 0;
	}

	inline
		std::ostream& GraphMap::print_mappings(print_t type, std::ostream& o) {

		switch (type) {
		case L2R:
			o << "\n*****************" << std::endl;
			o << "L->R" << std::endl;
			_stl::print_collection(l2r_, o, true);
			o << "\n*****************" << std::endl;
			break;
		case R2L:
			o << "\n*****************" << std::endl;
			o << "R->L" << std::endl;
			_stl::print_collection(r2l_, o, true);
			o << "******************" << std::endl;
			break;
		case BOTH:
			o << "\n*****************" << std::endl;
			o << "L->R and R->L" << std::endl;
			_stl::print_collection(l2r_, o, true);
			_stl::print_collection(r2l_, o, true);
			o << "*****************" << std::endl;
			break;
		default:
			LOG_WARNING("bad printing type - GraphMap::print_mappings");
		}

		return o;
	}

	inline
		std::ostream& GraphMap::print_names(print_t type, std::ostream& o) {

		switch (type) {
		case L2R:
			o << "L:" << nameL_;
			break;
		case R2L:
			o << "R:" << nameR_;
			break;
		case BOTH:
			o << "L:" << nameL_; o << std::endl;
			o << "R:" << nameR_;
			break;
		default:
			LOG_WARNING("bad printing type - GraphMap::print_names");
		}

		return o;
	}

}//end of namespace bitgraph

#endif