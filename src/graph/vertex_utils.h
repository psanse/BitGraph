/**
 * @file vertex_utils.h
 * @brief Utility operations for graph vertex collections.
 *
 * This header provides lightweight helper functions for creating and
 * manipulating collections of vertex indices used by the GRAPH library.
 *
 * @note Small utility functions are implemented directly in this header.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 */

#ifndef BITGRAPH_GRAPH_VERTEX_UTILS_H
#define BITGRAPH_GRAPH_VERTEX_UTILS_H

#include "graph_types.h"

#include <cassert>
#include <cstddef>
#include <limits>
#include <numeric>

namespace bitgraph {
    
    /**
     * @brief Fills a vertex collection with consecutive indices.
     *
     * Existing contents are discarded and replaced with the sequence
     * `[0, vertex_count)`.
     *
     * @param vertices Collection receiving the vertex indices.
     * @param vertex_count Number of vertex indices to generate.
     *
     * @pre @p vertex_count must be representable by vertex_t.
     */
    inline void fill_vertices(
        Vertices& vertices,
        std::size_t vertex_count)
    {
        assert(
            vertex_count <=
            static_cast<std::size_t>(
                std::numeric_limits<vertex_t>::max()));

        vertices.resize(vertex_count);
        std::iota(vertices.begin(), vertices.end(), vertex_t{ 0 });
    }

} // namespace bitgraph

#endif