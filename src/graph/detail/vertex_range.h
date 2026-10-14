/**
 * @file vertex_range.h
 * @brief [EXPERIMENTAL] Defines an internal closed range of graph vertices.
 *
 * This header provides a lightweight value type representing a closed interval
 * of nonnegative vertex indices. It is intended for internal use by GRAPH
 * algorithms and is not part of the public BitGraph API.
 *
 * @author Pablo San Segundo
 * @date Last updated: 26/09/2026
 *
 * @warning EXPERIMENTAL: this component lives in namespace bitgraph::experimental.
 *          Its interface is not stable and may change or be removed in any
 *          release without notice.
 */

#ifndef BITGRAPH_GRAPH_DETAIL_VERTEX_RANGE_H
#define BITGRAPH_GRAPH_DETAIL_VERTEX_RANGE_H

#include "graph/graph_types.h"

#include <ostream>

namespace bitgraph {

    namespace experimental {

        /**
         * @brief Closed interval of graph vertex indices.
         *
         * Represents the closed vertex interval [first,last]. A default-constructed
         * range is unset. A proper range has both endpoints set and satisfies
         * `0 <= first <= last`.
         *
         * @note This is an internal GRAPH algorithm type.
         */
        struct VertexRange {
            /**
             * @brief Sentinel representing an unspecified endpoint.
             */
            enum : vertex_t {
                no_vertex = static_cast<vertex_t>(-1)
            };

            vertex_t first = no_vertex;  ///< Inclusive first vertex.
            vertex_t last = no_vertex;   ///< Inclusive last vertex.

            /**
             * @brief Constructs a vertex range.
             *
             * @param first_vertex Inclusive first vertex.
             * @param last_vertex Inclusive last vertex.
             */
            constexpr VertexRange(
                vertex_t first_vertex = no_vertex,
                vertex_t last_vertex = no_vertex) noexcept
                : first{ first_vertex },
                last{ last_vertex }
            {}

            /**
             * @brief Determines whether either endpoint is unspecified.
             */
            constexpr bool is_unset() const noexcept
            {
                return first == no_vertex ||
                    last == no_vertex;
            }

            /**
             * @brief Determines whether both endpoints are specified.
             */
            constexpr bool is_set() const noexcept
            {
                return !is_unset();
            }

            /**
             * @brief Determines whether this is a valid nonnegative closed range.
             */
            constexpr bool is_proper() const noexcept
            {
                return is_set() &&
                    first >= 0 &&
                    first <= last;
            }

            /**
             * @brief Returns the number of vertices in the closed range.
             *
             * @return Number of vertices, or zero if the range is not proper.
             */
            constexpr std::size_t size() const noexcept
            {
                return is_proper()
                    ? static_cast<std::size_t>(last - first) + 1u
                    : 0u;
            }

            /**
             * @brief Determines whether the range contains @p vertex.
             */
            constexpr bool contains(vertex_t vertex) const noexcept
            {
                return is_proper() &&
                    vertex >= first &&
                    vertex <= last;
            }

            /**
             * @brief Writes the range in `[first,last]` format.
             *
             * @param out Destination stream.
             * @return Reference to @p out.
             */
            std::ostream& print(std::ostream& out) const
            {
                return out << '[' << first << ',' << last << ']';
            }

            friend std::ostream& operator<<(
                std::ostream& out,
                const VertexRange& range)
            {
                return range.print(out);
            }

            friend constexpr bool operator==(
                const VertexRange& lhs,
                const VertexRange& rhs) noexcept
            {
                return lhs.first == rhs.first &&
                    lhs.last == rhs.last;
            }

            friend constexpr bool operator!=(
                const VertexRange& lhs,
                const VertexRange& rhs) noexcept
            {
                return !(lhs == rhs);
            }
        };

    } // namespace experimental

} // namespace bitgraph

#endif // BITGRAPH_GRAPH_ALGORITHMS_DETAIL_VERTEX_RANGE_H