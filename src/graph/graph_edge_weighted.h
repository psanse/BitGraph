/** 
 * @file graph_edge_weighted.h
 * @brief  
 *		    
 * @comment:  
 *
 * @author: pss (main developper)
 * @date 01/02/2026 
 **/

#ifndef __GRAPH_INCLUDE_H__
#define __GRAPH_INCLUDE_H__

#include "graph_types.h"
#include "graph_basic.h"
#include "simple_graph_ew.h"											// MUST BE AFTER graph_basic.h

namespace bitgraph {
		 
	// specialization for undirected graphs

	template <class WeightT>
	class Graph_EW<ugraph, WeightT> : public Base_Graph_EW<ugraph, WeightT> {
	public:

		using BaseT = Base_Graph_EW<ugraph, WeightT>;					// parent type
		using graph_type = typename BaseT::graph_type;
		using bitset_type = typename BaseT::bitset_type;
		using VertexBitset = bitset_type;								// alias for semantic type
		using Weight = typename BaseT::Weight;

		using BaseT::NO_WEIGHT;
		using BaseT::ZERO_WEIGHT;
		using BaseT::DEFAULT_WEIGHT;

		using mat_t = typename BaseT::mat_t;							// matrix type for weights

		//constructors (inherited)
		using Base_Graph_EW<ugraph, Weight>::Base_Graph_EW;

		/////////////////////////
		// basic operations

		/**
		* @ brief adds an edge (v, w) with weight val
		**/
		void add_edge(int v, int w, Weight val = BaseT::ZERO_WEIGHT) override;

		/**
		*  @brief sets edge-weight val to the undirected edge {v, w} if the edge exists
		*		  (in a non-edge it can only set the weight to NO_WEIGHT)
		*  @param v input vertex
		*  @param w input vertex
		*  @param we input weight value
		*
		*	(weights in self-loops are always added - considered vertex weights)
		*
		*  @details: asserts v!=w
		*  @details: in a non-edge it can only set weight to NO_WEIGHT
		**/
		void set_weight(int v, int w, Weight val) override;

		/**
		*  @brief sets edge-weights in lw consistently to the graph
		*
		*		I. NO_WEIGHT is set as weight value to non-edges in lw
		*		II. Weights in self-loops (vertices) are NOT modified
		*
		*  @param lw matrix of edge weights
		*  @param template EraseNonEdges: if TRUE weights of non-edges are set to NO_WEIGHT
		*  @details: asserts lw.size() == |V|
		**/
		template<bool EraseNonEdges = false>
		void set_weight(mat_t& lw, bool edges_only = true);

		/**
		*  @brief sets edge-weight val to existing edges
		*
		*		I. Weights in self-loops (vertices) are NOT modified
		*
		* @param template EraseNonEdges: if TRUE weights of non-edges are set to NO_WEIGHT
		* @param val weight value
		**/
		template <bool EraseNonEdges = false>
		void set_edge_weight(Weight val = BaseT::ZERO_WEIGHT);

		/**
		* @brief sets new edge-weights based on modulus operation [Pullan 2008, MODULUS = 200]
		*
		*			I.  we(v, w) = 1 + ((v + w) % MODULUS)	(1-based index)
		*			II. Non-edges and vertex-weights are not modified
		* @param template Erase: if TRUE weights of non-edges are set to NO_WEIGHT
		* @param MODULUS: modulus value [Pullan 2008, DEFAULT_WEIGHT_MODULUS = 200]
		**/
		template<bool EraseNonEdges = false>
		void set_modulus_edge_weight(int MODULUS = DEFAULT_WEIGHT_MODULUS);


		/////////////
		//useful interface for graph operations (no weights)

		int max_graph_degree() const { return this->g_.max_graph_degree(); }
		int degree(int v) const { return this->g_.degree(v); }
		int degree(int v, const VertexBitset& bbn) const { return this->g_.degree(v, bbn); }

		/////////////
		//other operations

		/**
		* @brief generates random edges uniformly with probability p and weight val
		**/
		void gen_random_edges(double, Weight val = BaseT::ZERO_WEIGHT) override;

		/////////////
		// I/O operations
	protected:
		/**
		* @brief prints the edges of the graph in line format [v]-(val)->[w], one edge per line
		**/
		std::ostream& print_edges(std::ostream& o = std::cout, bool eofl = false) const override;

		std::ostream& print_edge_weights(std::ostream& o = std::cout, bool line_format = true) const override;
		std::ostream& print_edge_weights(VertexList& lv, std::ostream& o = std::cout)	const override;

	public:
		/**
		* @brief Writes undirected graph to stream in dimacs format
		*
		*		 I. weights in self-loops are considered vertex weights
		*
		*		II. vertex weights with value NO_WEIGHT are not written to file
		*
		*		III.for every edge, a weight is written (including NO_WEIGHT)
		*
		* @param o output stream
		* @returns output stream
		**/
		virtual	std::ostream& write_dimacs(std::ostream& o);

	}; //end of Graph_EW class
	
      
	// edge-weighted facade types

    using ugraph_ew = Graph_EW<ugraph, double>;                 // simple edge weighted graph with double weights
    using ugraph_ewi = Graph_EW<ugraph, int>;                   // simple edge weighted graph with integer weights
}

//////////////////////
// implementation in header files

namespace bitgraph {

	template <class WeightT>
	template <bool EraseNonEdges>
	void Graph_EW< ugraph, WeightT >::set_edge_weight(Weight val)
	{
		const int NV = this->num_vertices();

		//set to empty wv and non-edges UPPER-T
		for (int v = 0; v < NV - 1; v++) {
			for (int w = v + 1; w < NV; w++) {
				if (this->g_.is_edge(v, w)) {
					this->we_[v][w] = val;
					this->we_[w][v] = val;
				}
				else {
					if (EraseNonEdges) {
						this->we_[v][w] = BaseT::NO_WEIGHT;
						this->we_[w][v] = BaseT::NO_WEIGHT;
					}
				}
			}
		}

		//vertex weights
		/*for (int v = 0; v < NV; v++) {
			this->we_[v][v] = val;
		}*/
	}

	template <class WeightT>
	template<bool EraseNonEdges>
	void Graph_EW< ugraph, WeightT >::set_weight(typename Graph_EW<ugraph, WeightT>::mat_t& lw, bool edges_only)
	{
		const int NV = this->num_vertices();

		/////////////////////////
		assert(lw.size() == static_cast<std::size_t>(NV));
		/////////////////////////

		////assert
		//if (lw.size() != NV) {
		//	LOG_ERROR("bizarre matrix of weights - Graph_EW< ugraph, W >::set_edge_weight");
		//	LOG_ERROR("weights remain unchanged");
		//	return -1;
		//}

		//sets edge-weights
		for (int v = 0; v < NV - 1; ++v) {
			for (int w = v + 1; w < NV; ++w) {
				if (this->g_.is_edge(v, w)) {
					this->we_[v][w] = lw[v][w];
					this->we_[w][v] = lw[w][v];
				}
				else {
					if (EraseNonEdges) {
						this->we_[v][w] = BaseT::NO_WEIGHT;
						this->we_[w][v] = BaseT::NO_WEIGHT;
					}
				}
			}
		}

		//vertex weights
		if (!edges_only) {
			for (int v = 0; v < NV; v++) {
				this->we_[v][v] = lw[v][v];
			}
		}
	}

	template<class WeightT>
	template<bool EraseNonEdges>
	void Graph_EW<ugraph, WeightT>::set_modulus_edge_weight(int MODULUS) 
	{
		const int NV = this->num_vertices();

		//vertex-weights NO_WEIGHT
		//set_vertex_weight(BaseT::NO_WEIGHT);

		//sets weights of undirected edges
		for (int v = 0; v < NV - 1; ++v) {
			for (int w = v + 1; w < NV; ++w) {
				if (this->g_.is_edge(v, w)) {

					///////////////////////////////////////
					set_weight(v, w, (1 + ((v + w + 2 /* 0-based index*/) % MODULUS)));
					///////////////////////////////////////

				}
				else {
					if (EraseNonEdges) { set_weight(v, w, BaseT::NO_WEIGHT); }
				}
			}
		}
	}


	template<class WeightT>
	std::ostream& Graph_EW<ugraph, WeightT>::print_edges(std::ostream& o, bool eofl)  const 
	{
		const int NV = this->num_vertices();

		for (int i = 0; i < NV - 1; ++i) {
			for (int j = i + 1; j < NV; ++j) {

				if (this->g_.is_edge(i, j)) {
					o << "[" << i << "]" << "--(" << this->we_[i][j] << ")-->" << "[" << j << "]" << endl;
				}
			}
		}

		if (eofl) { o << std::endl; }
		return o;
	}

	template<class WeightT>
	void Graph_EW<ugraph, WeightT>::add_edge(int v, int w, Weight val)
	{
		//sets undirected edge
		this->g_.add_edge(v, w);

		//sets both weights
		this->we_[v][w] = val;
		this->we_[w][v] = val;
	}

	template <class W>
	void Graph_EW< ugraph, W >::set_weight(int v, int w, Weight val)
	{

		//////////////////
		assert(v != w);
		//////////////////

		auto is_edge = this->g_.is_edge(v, w);


		if (is_edge || (val == BaseT::NO_WEIGHT && !is_edge)) {
			this->we_[v][w] = val;
			this->we_[w][v] = val;
		}
		else {
			LOGG_WARNING("edge-weight cannot be added to a non-edge",
				"(", v, ",", w, ")", "- Graph_EW<Graph_t,WeightT >::set_weight");
		}

	}

	template<class WeightT>
	void Graph_EW<ugraph, WeightT>::gen_random_edges(double p, Weight val)
	{
		const int NV = this->num_vertices();

		//removes all edges
		this->g_.remove_edges();

		//sets directed edges with probability p
		for (auto i = 0; i < NV - 1; ++i) {
			for (auto j = i + 1; j < NV; ++j) {

				if (_rand::uniform_dist(p)) {
					add_edge(i, j, val);
				}
			}
		}
	}

	template<class WeightT>
	std::ostream& Graph_EW<ugraph, WeightT>::print_edge_weights(std::ostream& o, bool line_format) const
	{
		const int NV = this->num_vertices();

		//streams edge-weights 
		if (line_format) {
			print_edges(o, false);
		}
		else {								//outputs to stream edge-weights in matrix form
			for (auto i = 0; i < NV - 1; ++i) {
				for (auto j = i + 1; j < NV; ++j) {
					if (this->we_[i][j] != BaseT::NO_WEIGHT) {
						o << this->we_[i][j] << '\t';
					}
					else {
						o << "--" << '\t';
					}
				}
				o << endl;
			}
		}

		return o;
	}

	template<class WeightT>
	std::ostream& Graph_EW<ugraph, WeightT>::print_edge_weights(VertexList& lv, std::ostream& o) const
	{
		const int SIZE = static_cast<int>(lv.size());
		for (int i = 0; i < SIZE - 1; ++i) {
			for (int j = i + 1; j < SIZE; ++j) {
				if (this->we_[lv[i]][lv[j]] != BaseT::NO_WEIGHT) {

					/////////////////////////////////////////////////////////////////////////////////////////	
					//o << "[" << lv[i] << "-" << lv[j] << " (" << this->we_[lv[i]][lv[j]] << ")] " << endl;
					///////////////////////////////////////////////////////////////////////////////////////////

					///////////////////////////////////////////////////////////////////////////////////////////
					o << "[" << i << "]" << "--(" << this->weight(i, j) << ")-->" << "[" << j << "]" << endl;
					///////////////////////////////////////////////////////////////////////////////////////////
				}
			}
		}

		return o;
	}


	template<class WeightT>
	ostream& Graph_EW<ugraph, WeightT>::write_dimacs(ostream& o)
	{
		const int NV = this->num_vertices();

		//timestamp 
		this->g_.timestamp_dimacs(o);

		//name
		this->g_.name_dimacs(o);

		//dimacs header - recompute edges
		this->g_.header_dimacs(o, false);

		//write vertex weights
		for (int v = 0; v < NV; ++v) {
			if (this->we_[v][v] != BaseT::NO_WEIGHT) {
				o << "n " << v + 1 << " " << this->we_[v][v] << endl;
			}
		}

		//write undirected edges and edge weights
		for (int v = 0; v < NV - 1; ++v) {
			for (int w = v + 1; w < NV; ++w) {
				if (this->g_.is_edge(v, w)) {														//O(log) for sparse graphs: specialize
					o << "e " << v + 1 << " " << w + 1 << " " << this->we_[v][w] << endl;			//1-based vertex notation dimacs
				}
			}
		}

		return o;
	}
}


#endif // __GRAPH_INCLUDE_H__