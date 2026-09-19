/**
 * @file graph_types.h
 * @brief Defines common types and constants used by the GRAPH library.
 *
 * This header provides the fundamental vertex type, semantic collection
 * aliases, and constants shared by BitGraph graph classes and algorithms.
 *
 * Both snake_case and CamelCase aliases are currently provided for API
 * compatibility.
 *
 * @date Created: 30/06/2025
 * @date Last updated: 19/09/2026
 * @author Pablo San Segundo
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
	 * @brief CamelCase compatibility alias for vertex_t.
	 */
	using Vertex = vertex_t;

	/**
	 * @brief Sequence of graph vertices.
	 */
	using vertices = std::vector<vertex_t>;

	/**
	 * @brief CamelCase compatibility alias for vertices.
	 */
	using Vertices = vertices;

	/**
	 * @brief Mapping from vertices to vertices or vertex positions.
	 */
	using vertex_mapping = std::vector<vertex_t>;

	/**
	 * @brief CamelCase compatibility alias for vertex_mapping.
	 */
	using VertexMapping = vertex_mapping;

	/**
	 * @brief Ordering or permutation of graph vertices.
	 */
	using vertex_ordering = std::vector<vertex_t>;

	/**
	 * @brief CamelCase compatibility alias for vertex_ordering.
	 */
	using VertexOrdering = vertex_ordering;

	/**
	 * @brief Default modulus used to generate vertex and edge weights.
	 *
	 * @see Pullman (2008).
	 */
	constexpr int default_weight_modulus = 200;

	/**
	 * @brief Legacy alias for default_weight_modulus.
	 */
	constexpr int DEFAULT_WEIGHT_MODULUS = default_weight_modulus;		

}

#endif // BITGRAPH_GRAPH_TYPES_H
