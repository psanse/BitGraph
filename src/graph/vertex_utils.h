/**
 * @file vertex_utils.h
 * @brief Inline utilities for constructing graph vertex collections.
 *
 * @details
 * Provides lightweight helpers for creating and initializing collections of
 * graph vertex indices used throughout the BITGRAPH library. The utilities
 * use the library's `vertex_t` type and generate the zero-based sequence
 * `[0, vertex_count)`.
 *
 * Existing collection contents are replaced by the generated sequence. The
 * functions are implemented directly in this header so they can be inlined
 * at call sites.
 *
 * @author Pablo San Segundo
 * @date Last updated: 23/09/2026
 *
 * @see graph_types.h
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
