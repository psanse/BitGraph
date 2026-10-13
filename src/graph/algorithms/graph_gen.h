/**
 * @file graph_gen.h
 *
 * @brief Random graph generation utilities (Erdős–Rényi G(n,p)) for BitGraph facades.
 *
 * This header provides utilities to generate undirected simple graphs following
 * the Erdős–Rényi model G(n,p), supporting both dense and sparse bitset-backed
 * graph representations.
 *
 * It includes:
 *  - Parameter bundles for benchmark generation (`random_attr_t`)
 *  - Generic random generators for graph structure (`RandomGen<GraphT>`)
 *  - Generation of isomorphic graphs by vertex permutation
 *  - Batch generation of benchmark sets over ranges of n and p
 *
 * @details
 * The generators operate on the **graph structure only** (vertex/edge set).
 * When `GraphT` is a weighted-graph facade type, weights are not generated or
 * modified unless explicitly handled by the caller; only edges are created.
 *
 * The main generation model is:
 *  - Simple graphs (no self-loops)
 *  - Uniform edge creation probability p for each potential edge
 *
 * This header is intended for benchmarking, testing, and dataset generation.
 * It may include facade headers to make common concrete graph types available.
 *
 * @note
 * The current architecture mixes facade bindings and generator logic; a future
 * refactor is planned to simplify class structure and reduce include coupling.
 *
 * @par Historical notes
 *  - Created: (unknown)
 *  - Last modified: 01/02/2026
 *
 * @todo Simplify class architecture for graph generation (07/03/2025).
 *
 * @author pss
 */

#ifndef BITGRAPH_GRAPH_GEN_H
#define BITGRAPH_GRAPH_GEN_H

#include "utils/path_utils.h"
#include "graph/ugraph_vertex_weighted.h"						// facade types 
#include "graph/ugraph_edge_weighted.h"							// facade types 
#include "graph_fast_sort.h"

#include <iostream>
#include <sstream>
#include <random>								
#include <cassert>

namespace bitgraph {
		
		/**
		 * @brief Parameters for random graph generation.
		 */
		struct random_attr_t {
			
			//constructor
			random_attr_t(std::size_t nLB, std::size_t  nUB,
				double pLB, double pUB, 
				std::size_t  nRep,
				std::size_t  incN, double incP) 
			{
				set(nLB, nUB, pLB, pUB, nRep, incN, incP);				
			}

			//setter
			void set(std::size_t  nLB, std::size_t  nUB, 
				double pLB, double pUB,
				std::size_t  nRep, 
				std::size_t  incN, double incP) 
			{
				assert(nLB <= nUB);
				assert(0.0 <= pLB && pLB <= pUB && pUB <= 1.0);
				assert(nRep > 0);
				assert(nLB == nUB || incN > 0);
				assert(pLB == pUB || incP > 0.0);
				assert(nLB <= nUB && pLB <= pUB && incP > 0 && "bad parameters - random_attr_t");

				this->nLB = nLB;
				this->nUB = nUB;
				this->pLB = pLB;
				this->pUB = pUB;
				this->nRep = nRep;
				this->incN = incN;
				this->incP = incP;
			}

			friend std::ostream& operator<<(std::ostream& os,
				const random_attr_t& r)
			{
				return os << '['
					<< r.nLB << ' ' << r.nUB << ' '
					<< r.pLB << ' ' << r.pUB << ' '
					<< r.nRep << ' '
					<< r.incN << ' ' << r.incP << ']'
					<< '\n';
			}
			

			std::size_t  nLB;					//range of vertices [nLB, nUB]
			std::size_t  nUB;
			double pLB;							//range of densities [pLB, pUB]	
			double pUB;
			std::size_t  nRep;					//number of repetitions
			std::size_t  incN;					//increment of vertices
			double incP;						//increment of densities
		};

}//end of namespace bitgraph	

namespace bitgraph {

	/**
	 * @brief Random generation of graph structures.
	 *
	 * Supports unweighted and weighted graph types.
	 * For weighted graphs, only the graph topology is generated.
	 *
	 * @tparam GraphT Graph type.
	 */
	template<class GraphT>
	class RandomGen {
	public:

		using graph_t = GraphT;

		/**
		 * @brief Generates a simple random graph G(n, p).
		 *
		 * Each possible edge is generated independently with probability @p p.
		 *
		 * - No self-loops are generated.
		 * - Directed or undirected edges are generated according to GraphT.
		 * - The graph is named "r<NV>_<p>.txt".
		 * - Any existing graph contents are replaced.
		 *
		 * @param[out] graph Generated random graph.
		 * @param[in] NV Number of vertices (must be greater than 0).
		 * @param[in] p Edge probability in [0, 1].
		 *
		 * @note Invalid parameters cause program termination (fail-fast).
		 */
		static void  create_graph(
			graph_t& graph,
			std::size_t NV,
			double p);

		/**
		 * @brief Creates and returns a simple random graph G(n, p).
		 *
		 * Each possible edge is generated independently with probability @p p.
		 *
		 * - No self-loops are generated.
		 * - Directed or undirected edges are generated according to GraphT.
		 * - The graph is named "r<NV>_<p>.txt".
		 *
		 * @param[in] NV Number of vertices (must be greater than 0).
		 * @param[in] p Edge probability in [0, 1].
		 * @return Generated random graph.
		 *
		 * @note Invalid parameters cause program termination (fail-fast).
		 */
		static graph_t create_graph(std::size_t NV,
			double p) 
		{
			graph_t g;
			create_graph(g, NV, p);
			return g;
		}

		/**
		 * @brief Generates and saves a benchmark collection of random graphs.
		 *
		 * Generates simple random graphs G(n, p) for the specified
		 * ranges of vertices and edge probabilities.
		 *
		 * For each (n, p) combination, generates nRep independent
		 * graph instances and writes them in DIMACS format.
		 *
		 * Output filenames follow the convention:
		 * "r<n>_<p>_<k>.txt", where k is the repetition index.
		 *
		 * @param[in] path Output directory.
		 * @param[in] rd Random graph generation parameters.
		 *
		 * @note The output directory must already exist.
		 * @note Invalid parameters or file I/O failures cause
		 *       program termination (fail-fast).
		 */
		static void create_graph_benchmark(const std::string& path, 
			const random_attr_t& rd);

		/**
		 * @brief Creates an isomorphic graph by randomly permuting the vertices
		 *        of the source graph using std::shuffle.
		 *
		 * @param[out] g_iso Generated isomorphic graph.
		 * @param[in] g_source Source graph (not modified).
		 *
		 * @note Uses a fixed random seed for reproducibility.
		 */
		static void create_isomorphism(graph_t& g_iso,
			graph_t& g);
	};	

}//end of namespace bitgraph	

namespace bitgraph {
	
	namespace experimental {

		/////////////////
		//
		// class SparseRandomGen
		// (random generation of generic sparse unweighted graphs)
		// 
		// Currently only for undirected graphs
		//
		//////////////////
		template<class GraphT, int logSizeTable = 35000 >
		class SparseRandomGen {};

		/////////////////
		//
		// Specialization of SparseRandomGen for sparse_ugraph
		// (massive sparse uniform graph generator)
		// 
		//////////////////

		template< int logSizeTable >
		class SparseRandomGen <sparse_ugraph, logSizeTable>
		{
			/*
			*  @brief decodes undimensional edge index to upper triangle of adjacency matrix
			*
			*		  (see [Fast Random graph generation, 2011], personal comm. with authors)
			*/
			static void decode(BITBOARD edge_index, unsigned int& i, unsigned int& j, unsigned int n);

		public:

			//constructor
			SparseRandomGen() { init_log_table(); }

			//driver
			int create_ugraph(sparse_ugraph& g, std::size_t n, double p);

		private:
			/*
			* @brief 16-bit table for precomputed logarithms
			*/
			void init_log_table();


			///////////////////////
			// data
		private:

			double log_[logSizeTable];
		};

	}//end of namespace experimental

	//using experimental::SparseRandomGen;		//alias for SparseRandomGen<GraphT, logSizeTable>

}//end of namespace bitgraph



namespace bitgraph {

	namespace experimental {
		
		/////////////////
		//
		// Class WeightGen
		// (generates weights for vertex-weighted graphs)
		// 
		// TODO specialize for sparse-graphs
		// TODO SFINAE to control template graph type deduction (07/03/2025)
		// 
		// TODO - in general revise the whole logic of the generator classes for different graph types (07/03/2025)
		//
		////////////////

		template <class GraphT>
		class WeightGen
		{
			//filter out invalid types - currently only non-sparse vertex weighted graphs
			static_assert(std::is_same<GraphT, ugraph_vw>::value ||
						 std::is_same<GraphT, ugraph_vwi>::value,						
						"WeightGen<GraphT> : GraphT must be a simple, non-sparse,  vertex-weighted graph type ");

		public:
			enum type_t { WMOD = 0, WDEG };

			/*
			* @brief Overrites new vertex weights in the graph
			*
			*		I. we(v) = 1 + (v % mod) [Pullham 2008]
			*
			*		II. Optionally the weights are written to a file
			*
			*		TODO: specialize for sparse-graphs
			*
			* @returns -1 if error, 0 if OK
			*/
			static int create_weights(GraphT& g, type_t, int wmod = DEFAULT_WEIGHT_MODULUS,
				std::string FILE_EXTENSION = "", std::string FILE_PATH = "");
		};

	}//end of namespace experimental

//	using experimental::WeightGen;		//alias for WeightGen<GraphT>

}//end of namespace bitgraph


namespace bitgraph {

	namespace experimental {

		/////////////////
		//
		// Class EdgeWeightGen
		// (generates weights for edge-weighted graphs)
		//
		// TODO refactor to the new type (22/01/19)
		// TODO specialize for sparse-graphs (08/01/2025)
		// TODO add more weight generation modes (08/01/2025)
		// TODO SFINAE to control template graph type deduction (07/03/2025)
		//
		////////////////


		template<class GraphT>
		class EdgeWeightGen
		{
			// filter out invalid types - currently only non-sparse vertex weighted graphs
			static_assert(std::is_same<GraphT, ugraph_ew>::value ||
						  std::is_same<GraphT, ugraph_ewi>::value,
							"EdgeWeightGen<GraphT> : GraphT must be a simple, non-sparse,  edge-weighted graph type ");


		public:
			enum type_t { WMOD = 0 };

			/*
			* @brief Generates weights for existing edges in a graph
			*
			*		I. we(v, w) = 1 + ((v + w) % mod) [Pullham 2008]
			*
			*		II. we(v, v) and non-edge weights are set to NOWT
			*
			*		III. Optionally the weights are written to a file
			*
			*		TODO: specialize for sparse-graphs
			*
			* @returns -1 if error, 0 if OK
			*/
			static int create_weights(GraphT& g, type_t, int wmod = DEFAULT_WEIGHT_MODULUS,
				std::string FILE_EXTENSION = "", std::string FILE_PATH = "");

		};
	}//end of namespace experimental

//	using experimental::EdgeWeightGen;		//alias for EdgeWeightGen<GraphT>

}//end of namespace bitgraph

////////////////////////////////////////////////////////////////////////
// Necessary implementations of generic code in header file

#include "detail/graph_gen_imp.h"

#endif //BITGRAPH_GRAPH_GEN_H__
