/**
 * @file graph_types.h
 * @brief Shared vertex, collection, ordering, and weight types for GRAPH.
 *
 * @details
 * This header defines the fundamental scalar and container types used across
 * BitGraph graph classes and algorithms, including vertex identifiers, degree
 * values, vertex collections, vertex mappings, orderings, and the default
 * weight modulus.
 *
 * The API intentionally contains several redundant aliases for backward
 * compatibility. The preferred modern names use snake_case, while aliases
 * such as @c Vertex, @c Vertices, @c VertexMapping, @c VertexOrdering,
 * @c vertex_set_t, and @c DEFAULT_WEIGHT_MODULUS preserve compatibility with
 * older graph code. New code should use the canonical snake_case names where
 * possible.
 *
 * @date Created: 30/06/2025
 * @date Last updated: 19/09/2026
 * @author Pablo San Segundo
 *
 * @warning The compatibility aliases are type synonyms, not distinct types;
 *          changing the underlying canonical type affects all aliases.
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
	 * @brief classical compatibility alias for vertices.
	 */
	using vertex_set_t = vertices;

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
	 * @brief Integer type used to identify the degree of a vertex.
	 */
	using degree_t = int;

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
