/**
 * @file graph_traits.h
 * @brief Defines compile-time properties of BitGraph graph types.
 *
 * This header provides the primary graph_traits template and its
 * specializations for supported graph families. Generic algorithms can query
 * these traits to determine whether a graph is directed, vertex-weighted, or
 * edge-weighted without inspecting its concrete implementation.
 *
 * The traits support compile-time selection, tag dispatch, validation, and
 * specialization of graph algorithms. This header also provides internal
 * metaprogramming utilities for deferred static assertions.
 *
 * Only graph type declarations are required; complete graph definitions are
 * intentionally not included to reduce coupling and avoid circular
 * dependencies.
 *
 * @date Last updated: 19/01/2026
 * @author P. San Segundo
 *
 * @todo Add specializations for vertex- and edge-weighted graph families.
 */

 // TODO Add graph_traits specializations for vertex- and edge-weighted
 // graph families when those types are integrated.

#ifndef BITGRAPH_GRAPH_TRAITS_H
#define BITGRAPH_GRAPH_TRAITS_H

#include <type_traits>

namespace bitgraph {

    /**
     * @brief Forward declaration of the non-sparse undirected graph class template.
     *
	 * @tparam BitsetT Bitset type used to encode the graph's adjacency matrix.
     */
    template<class BitsetT>
    class Ugraph;
    

    /**
     * @brief Provides compile-time properties of a graph type.
     *
     * The primary template describes a directed, unweighted graph.
     * Specializations override these properties for supported graph families.
     *
     * @tparam GraphT Graph type whose properties are queried.
     */
    template<class GraphT>
    struct graph_traits {
        static constexpr bool is_undirected = false;
        static constexpr bool is_vertex_weighted = false;
        static constexpr bool is_edge_weighted = false;
    };

    /**
      * @brief Provides compile-time properties of Ugraph specializations.
      *
      * Every Ugraph specialization is classified as undirected and unweighted,
      * independently of the bitset type used for its neighborhoods.
      *
      * @tparam BitsetT Bitset type used to represent graph neighborhoods.
      */
    template<class BitsetT>
    struct graph_traits<Ugraph<BitsetT>> {
        static constexpr bool is_undirected = true;
        static constexpr bool is_vertex_weighted = false;
        static constexpr bool is_edge_weighted = false;
    };

    namespace detail {

        /**
         * @brief Type-dependent false condition for deferred static assertions.
         *
         * Unlike a literal false condition, this trait depends on a template
         * parameter, so the assertion is evaluated only when the enclosing template
         * is instantiated.
         *
         * @tparam T Arbitrary dependent type.
         */
        template<class>
        struct dependent_false : std::false_type {};       

	} // namespace detail
       
} // namespace bitgraph



#endif // BITGRAPH_GRAPH_TRAITS_H