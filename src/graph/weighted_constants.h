/**
 * @file weighted_constants.h
 * @brief Common sentinel and default values for weighted graph types.
 *
 * @date 2024-10-10
 *
 * @details
 * Defines the `weight_constants<WeightT>` traits structure used by weighted
 * graph implementations to obtain consistent values for an absent weight,
 * a zero weight, and the default edge or vertex weight. The constants are
 * parameterized by the graph's weight type and are available at compile time.
 *
 * @tparam WeightT Numeric type used to represent graph weights.
 *
 * @note `NO_WEIGHT` uses the value `-1`; callers should ensure that the
 *       selected weight type can represent this sentinel.
 *
 * @see simple_graph_ew.h
 * @see ugraph_vertex_weighted.h
 */

#ifndef BITGRAPH_GRAPH_WEIGHT_CONSTANTS_H
#define BITGRAPH_GRAPH_WEIGHT_CONSTANTS_H


namespace bitgraph {

	template<class WeightT>
	struct weight_constants {
		using weight_t = WeightT;

		static constexpr weight_t NO_WEIGHT = -1;
		static constexpr weight_t ZERO_WEIGHT = 0;
		static constexpr weight_t DEFAULT_WEIGHT = 1;
	};

} 

// Out-of-class definitions for static constexpr members (C++14).

namespace bitgraph {

	template<class WeightT>
	constexpr WeightT weight_constants<WeightT>::NO_WEIGHT;

	template<class WeightT>
	constexpr WeightT weight_constants<WeightT>::ZERO_WEIGHT;

	template<class WeightT>
	constexpr WeightT weight_constants<WeightT>::DEFAULT_WEIGHT;
}

#endif // BITGRAPH_GRAPH_WEIGHT_CONSTANTS_H
