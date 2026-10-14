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
#include <fstream>
#include <random>
#include <cmath>
#include <cstdint>
#include <limits>
#include <cassert>
#include <algorithm>

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
		 * @param[in] g Source graph (not modified).
		 *
		 * @note Uses a fixed random seed for reproducibility.
		 */
		static void create_isomorphism(graph_t& g_iso,
			graph_t& g);
	};	

}//end of namespace bitgraph	

namespace bitgraph {
	
	namespace experimental {

		/**
		 * @brief Experimental random graph generator for sparse graphs.
		 *
		 * Generates random unweighted graph structures using sparse
		 * graph representations. Currently supports only undirected graphs.
		 *
		 * @tparam GraphT Sparse graph type (only sparse_ugraph is supported).
		 *
		 * @warning Experimental implementation. Not part of the stable BITGRAPH API.
		 */
		template<class GraphT>
		class SparseRandomGen {
			static_assert(sizeof(GraphT) == 0,
				"SparseRandomGen<GraphT> : only sparse_ugraph is currently supported");
		};

		/**
		 * @brief Experimental random graph generator for massive sparse undirected graphs.
		 *
		 * Specialization of SparseRandomGen for sparse_ugraph.
		 * Generates G(n, p) random graphs by geometric skipping over the
		 * n(n-1)/2 candidate edges [Batagelj & Brandes, 2005], so that the
		 * cost is proportional to the number of generated edges.
		 *
		 * @warning Experimental implementation. Not part of the stable BITGRAPH API.
		 */
		template<>
		class SparseRandomGen<sparse_ugraph>	{
		public:

			/**
			 * @brief Generates a simple random undirected graph G(n, p).
			 *
			 * @param[out] graph Generated graph (existing contents are replaced).
			 * @param[in] NV Number of vertices, in [1, max(vertex_t)].
			 * @param[in] p Edge probability in [0, 1].
			 * @param[in] seed Seed of the random engine (same seed, same graph).
			 *
			 * @note Invalid parameters cause program termination (fail-fast).
			 */
			static void create_ugraph(
				sparse_ugraph& graph,
				std::size_t NV, 
				double p,
				std::uint64_t seed = utils::detail::RandomUniformGen<>::FIXED_RANDOM_SEED);

		private:

			/**
			 * @brief Decodes a linear edge index into a pair of vertices.
			 *
			 * Maps an edge index to the corresponding position (i, j) in the
			 * upper triangular part of the adjacency matrix, excluding the diagonal.
			 * The mapping represents an undirected graph without self-loops.
			 *
			 * The row is estimated in floating point and then corrected with
			 * exact integer arithmetic, so the result is exact for any NV.
			 *
			 * @param[in] edge_index Linear index of the edge.
			 * @param[out] i First vertex of the decoded edge.
			 * @param[out] j Second vertex of the decoded edge.
			 * @param[in] NV Number of vertices.
			 *
			 * @pre NV >= 2.
			 * @pre edge_index < NV * (NV - 1) / 2.
			 *
			 * @note The resulting vertices satisfy 0 <= i < j < NV.
			 * @see Fast Random Graph Generation (2011).
			 */
			static void decode(
				std::uint64_t edge_index,
				vertex_t& i,
				vertex_t& j,
				std::size_t NV);
		};

	}//end of namespace experimental

	//using experimental::SparseRandomGen;		//alias for SparseRandomGen<GraphT>

}//end of namespace bitgraph



namespace bitgraph {

	namespace experimental {
		
		/**
		 * @brief Experimental vertex weight generator for weighted graphs.
		 *
		 * Provides strategies for assigning vertex weights based on vertex
		 * indices (modulus-based) or vertex degrees.
		 * Existing weights are overwritten, and generated weights can
		 * optionally be written to a file.
		 *
		 * @tparam GraphT Vertex-weighted graph type.
		 *
		 * @note Currently supports non-sparse vertex-weighted graphs.
		 * @warning Experimental implementation. Not part of the stable BITGRAPH API.
		 */
		template <class GraphT>
		class VertexWeightGen
		{
			//filter out invalid types - currently only non-sparse vertex weighted graphs
			static_assert(std::is_same<GraphT, ugraph_vw>::value ||
						  std::is_same<GraphT, ugraph_vwi>::value,						
						 "VertexWeightGen<GraphT> : GraphT must be a simple, non-sparse,  vertex-weighted graph type ");

		public:

			using graph_t = GraphT;

			/**
			 * @brief Strategies for generating vertex weights.
			 */
			enum class weight_type {
				modulus = 0,   ///< Weight derived from the vertex index modulo a given value.
				degree         ///< Weight derived from the vertex degree.
			};

			/**
			 * @brief Generates and overwrites vertex weights in a graph.
			 *
			 * Supports two weight generation strategies:
			 * - Modulus-based: w(v) = 1 + ((v + 1) % wmod), i.e., w(i) = 1 + (i % wmod)
			 *   for 1-based vertex indices i = v + 1 [Pullan, 2008].
			 * - Degree-based: w(v) = degree(v) (isolated vertices get weight 0).
			 *
			 * Optionally writes the generated weights to a file.
			 *
			 * @param[in,out] graph Graph whose vertex weights are overwritten.
			 * @param[in] type_of_weights Weight generation strategy.
			 * @param[in] wmod Modulus for index-based weights (must be positive).
			 * @param[in] file_extension Extension appended to the graph name.
			 * @param[in] file_path Output directory for the weight file.
			 *
			 * @note File output is enabled only when both file_extension and
			 *       file_path are non-empty. The file is named
			 *       <file_path>/<graph name><file_extension> and contains the
			 *       weights only, in vertex order (see print_weights).
			 * @note Invalid parameters or file I/O failures cause program termination.
			 *
			 * @todo Add support for sparse graphs.
			 */
			static void create_weights(
				graph_t& graph,
				weight_type type_of_weights,
				int wmod = DEFAULT_WEIGHT_MODULUS,
				const std::string& file_extension = "",
				const std::string& file_path = "");
		};

	}//end of namespace experimental


}//end of namespace bitgraph


namespace bitgraph {

	namespace experimental {

		/**
		 * @brief Experimental edge weight generator for weighted graphs.
		 *
		 * Generates edge weights for existing edges using a modulus-based
		 * strategy. Graph topology is preserved, and generated weights
		 * can optionally be written to a file.
		 *
		 * @tparam GraphT Edge-weighted graph type.
		 *
		 * @note Currently supports non-sparse edge-weighted graphs.
		 * @warning Experimental implementation. Not part of the stable BITGRAPH API.
		 *
		 * @todo Refactor to support the new graph types (22/01/2019).
		 * @todo Add support for sparse graphs (08/01/2025).
		 * @todo Add more edge weight generation strategies (08/01/2025).
		 * @todo Consider SFINAE for restricting supported graph types (07/03/2025).
		 */
		template<class GraphT>
		class EdgeWeightGen
		{
			// Restrict GraphT to supported non-sparse edge-weighted graph types
			static_assert(
				std::is_same<GraphT, ugraph_ew>::value ||
				std::is_same<GraphT, ugraph_ewi>::value,
				"EdgeWeightGen<GraphT>: unsupported edge-weighted graph type"
				);

		public:

			using graph_t = GraphT;
					
			/**
			 * @brief Generates and overwrites edge weights in a graph.
			 *
			 * Assigns weights to existing edges with graph_t::set_modulus_weights(),
			 * for 0-based vertex indices:
			 * w(v, u) = 2 + ((v + u) % (wmod - 1)), with weights in [2, wmod].
			 *
			 * Pullan-style scheme; not identical to Pullan's (2008) 1-based
			 * formula w(i, j) = 1 + ((i + j) % wmod).
			 *
			 * Non-edges are assigned NO_WEIGHT; the diagonal (self-loops) is not modified.
			 * Optionally writes the resulting edge-weight matrix to a file.
			 *
			 * @param[in,out] graph Graph whose edge weights are overwritten.
			 * @param[in] wmod Maximum generated weight (must be at least 2).
			 * @param[in] file_extension Extension appended to the graph name.
			 * @param[in] file_path Output directory for the weight file.
			 *
			 * @note File output is enabled only when both file_extension and
			 *       file_path are non-empty.
			 * @note Invalid parameters or file I/O failures cause program termination.
			 *
			 * @todo Add support for sparse graphs.
			 */
			static void create_weights(
				graph_t& graph, 
				int wmod = DEFAULT_WEIGHT_MODULUS,
				const std::string& file_extension = "",
				const std::string& file_path = "");

		};
	}//end of namespace experimental

//	using experimental::EdgeWeightGen;		//alias for EdgeWeightGen<GraphT>

}//end of namespace bitgraph

////////////////////////////////////////////////////////////////////////
// Necessary implementations of generic code in header file

#include "detail/graph_gen_imp.h"

#endif //BITGRAPH_GRAPH_GEN_H__
