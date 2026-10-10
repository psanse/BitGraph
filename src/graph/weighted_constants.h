/**
 * @file weight_constants.h
 * @brief Common constants for weighted graph types.
 */

#ifndef BITGRAPH_GRAPH_WEIGHT_CONSTANTS_H
#define BITGRAPH_GRAPH_WEIGHT_CONSTANTS_H


namespace bitgraph {

	template<class WeightT>
	struct weight_constants {
		using weight_t = WeightT;

		static constexpr weight_t NO_WEIGHT{ -1 };
		static constexpr weight_t ZERO_WEIGHT{ 0 };
		static constexpr weight_t DEFAULT_WEIGHT{ 1 };
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