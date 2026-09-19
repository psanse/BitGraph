/**
 * @file graph_traits.h
 * @brief Defines compile-time traits for BitGraph graph types.
 *
 * This header provides the primary graph_traits template and its
 * specializations for supported graph families. Generic graph algorithms can
 * use these traits for compile-time selection, tag dispatch, and validation.
 *
 * It also provides internal metaprogramming utilities used to defer static
 * assertions until template instantiation.
 *
 * @date Last updated: 30/01/2026
 * @author P. San Segundo
 */

#ifndef BITGRAPH_GRAPH_TRAITS_H
#define BITGRAPH_GRAPH_TRAITS_H__

#include "simple_ugraph.h"
#include <type_traits>

namespace bitgraph {

    //directed / undirected trait

    /**
     * @brief Compile-time properties of a graph type.
     *
     * The primary template describes graph types as directed. Specializations
     * override the properties for particular graph families.
     *
     * @tparam GraphT Graph type being inspected.
     */
    template<class GraphT>
    struct graph_traits {
        static constexpr bool is_undirected = false;
    };

    /**
     * @brief Graph traits specialization for undirected graphs.
     *
     * @tparam BitsetT Bitset type used to represent graph neighborhoods.
     */
    template<class BitsetT>
    struct graph_traits<Ugraph<BitsetT>> {
        static constexpr bool is_undirected = true;
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


#endif // BITGRAPH_GRAPH_TRAITS_H__ 