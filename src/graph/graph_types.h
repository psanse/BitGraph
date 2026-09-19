/**
* @file: graph_types.h
* @brief: common types / contants used by GRAPH in BitGraph
* @date: created 30/06/2025
* @date: last_update 30/06/2025
*/

#ifndef BITGRAPH_GRAPH_TYPES_H
#define BITGRAPH_GRAPH_TYPES_H

#include <vector>	

namespace bitgraph {

	/**
	 * @brief Integer type used to identify a graph vertex.
	 */
	using vertex_t = int;

	/**
	 * @brief CamelCase alias for vertex_t.
	 */
	using Vertex = vertex_t;

	/** @brief Collection of vertices. */
	using Vertices = std::vector<vertex_t>;

	/** @brief Mapping from vertices to vertices or vertex positions. */
	using VertexMapping = std::vector<vertex_t>;

	/** @brief Ordering or permutation of vertices. */
	using VertexOrdering = std::vector<vertex_t>;

	/**
	 * @brief Default modulus used to generate vertex and edge weights.
	 *
	 * @see Pullman (2008).
	 */
	constexpr int default_weight_modulus = 200;
	constexpr int DEFAULT_WEIGHT_MODULUS = default_weight_modulus; // legacy alias

}

#endif // BITGRAPH_GRAPH_TYPES_H
