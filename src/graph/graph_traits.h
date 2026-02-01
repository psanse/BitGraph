/**
* @file graph_traits.h
* @brief header only pattern that defines traits for GRAPH types, later used for tag-dispatch
* @date 30/01/02026
* @author pss
*
**/

#ifndef __GRAPH_TRAITS_H__
#define __GRAPH_TRAITS_H__

#include "simple_ugraph.h"
#include <type_traits>

namespace bitgraph {

    //directed / undirected trait

    template<class GraphT>
    struct graph_traits {
        static constexpr bool is_undirected = false;
    };

    // specialize for undirected graph type
    template<class BitsetT>
    struct graph_traits<Ugraph<BitsetT>> {
        static constexpr bool is_undirected = true;
    };

	// helper to trigger static_assert false in templates
	// public derivation from std::false_type gives standard trait behavior
	// mental model: dependent false has value false unless specialized otherwise
    template<class>
    struct dependent_false : std::false_type {};        // alternatively always_false
       
} 


#endif // __GRAPH_TRAITS_H__