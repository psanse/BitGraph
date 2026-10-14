/**
 * @file edge_weighted_graph_utils.h
 * @brief Header-only algorithms for undirected edge-weighted graphs.
 *
 * @details
 * This header defines utilities in @c bitgraph::graph_utils for aggregating
 * edge weights and redistributing the weights of a selected vertex-induced
 * subproblem. The @c wesum overloads support complete weighted graphs and
 * vertex collections, with an option to include or exclude diagonal
 * vertex-weight entries. The @c UEW alias provides a convenient weighted
 * undirected graph specialization backed by the public dense bit-array type.
 *
 * @c ew_shift_2_highest_index initializes a caller-provided weight array and
 * shifts each selected edge's weight between its endpoints, assigning the
 * proportion controlled by @p wper to the endpoint with the higher vertex
 * index. The utility functions are templates and are defined inline in this
 * header.
 *
 * @author pss
 * @date Created: 7 March 2017
 * @date Last updated: 30 January 2025
 *
 * @see ugraph_edge_weighted.h
 * @see graph_traits.h
 * @see sort_utils.h
 * @warning Callers of @c ew_shift_2_highest_index must provide a valid output
 *          array covering the referenced vertex indices; @p wper is intended
 *          to lie in the interval [0, 1].
 * @todo Review and test @c ew_shift_2_highest_index, especially for integral
 *       weight types.
 */

#ifndef BITGRAPH_GRAPH_EDGE_WEIGHTED_GRAPH_UTILS_H
#define BITGRAPH_GRAPH_EDGE_WEIGHTED_GRAPH_UTILS_H

#include "graph/ugraph_edge_weighted.h"
#include "graph/graph_traits.h"
#include "utils/logger.h"
#include "utils/sort_utils.h"
#include <algorithm>
#include <iostream>

namespace bitgraph {

	namespace graph_utils {

		template<typename GraphT>
		typename GraphT::Weight wesum(const GraphT& g, bool only_we = false) {
			static_assert(bitgraph::dependent_false<GraphT>::value,
					"wesum(GraphT) requires an edge-weighted graph");
			return typename GraphT::Weight{};
		}

		//alias
		template<class WeighT>
		using UEW = bitgraph::Graph_EW<Ugraph<bitarray>, WeighT>;

		/*
		* @brief Determines the sum of the edge-weights of a given undirected graph
		* @param only_we: flag to consider only edge-weights (true) or also vertex-weights we_(v, v) (false)
		* @returns sum of the weights
		*
		* TODO - simplified code (09/01/25)
		*/
		template<typename WeighT>
		typename UEW<WeighT>::WeighT wesum(const UEW<WeighT>& g, bool only_we = false) {

			WeighT total_weight = 0.0;
			const int NV = g.num_vertices();

			for (int i = 0; i < NV - 1; ++i) {
				for (int j = (only_we ? i + 1 : i); j < NV; j++) {

					if (g.get_we(i, j) != UEW<WeighT>::NOWT) {
						total_weight += g.get_we(i, j);				// no edge-checking!
					}
				}
			}

			return total_weight;
		}

		/*
		* @brief Determines the sum of the edge-weights of an induced subgraph
		*
		*		I. Does not add default empty weigth value in case of inconsisten graphs
		*
		* @param lv: list of vertices in the induced subgraph
		* @param only_we: flag to consider only edge-weights (true) or also vertex-weights we_(v, v) (false)
		* @returns sum of the weights
		*
		*/
		template<typename GraphT>
		typename GraphT::Weight wesum(const GraphT& g, Vertices& lv, bool only_we = false) {

			typename GraphT::Weight total_weight = 0.0;
			const int NV = static_cast<int>(lv.size());

			for (int i = 0; i < NV - 1; ++i) {
				for (int j = (only_we ? i + 1 : i); j < NV; ++j) {

					if (g.get_we(i, j) != GraphT::NOWT) {				// Checks graph consistency - perhaps remove or do it in DEBUG mode only
						total_weight += g.get_we(lv[i], lv[j]);			// no edge-checking!
					}
				}
			}

			return total_weight;
		}

		template<class GraphT, class WeightT>
		int ew_shift_2_highest_index(const GraphT& g, const Vertex* lv, WeightT* lw, int size_lv, double wper = 1.0) {
			/////////////////////////
			// 
			// last_update@: 8/8/2018
			// param@lv: subproblem nodes
			// param@size: number of nodes of subproblem lv
			// param@lw: array of weights of nodes- weights will be initialied to 0. Edge weight shifted to the 
			//			 endpoint node with highest index (WRITE)
			//	param@wper: [0,1] ratio of the weight that is shifted towards highest index (DEFAULT VALUE 1-100% )
			//
			// RETURNS: 0 if OK, -1 if ERROR
			//
			// comments@: TEST-Does not work for integer weights! (25/11/2021)

			//init weights
			for (int i = 0; i < size_lv; i++) {
				lw[lv[i]] = 0.0;
			}

			//main loop, enumerating all edges in the subproblem
			for (int i = 0; i < size_lv - 1; i++) {
				for (int j = i + 1; j < size_lv; j++) {
					if (g.is_edge(lv[i], lv[j])) {
						if (lv[i] < lv[j]) {
							lw[lv[j]] += wper * g.get_we(lv[i], lv[j]);
							lw[lv[i]] += (1 - wper) * g.get_we(lv[i], lv[j]);
						}
						else {
							lw[lv[i]] += wper * g.get_we(lv[i], lv[j]);
							lw[lv[j]] += (1 - wper) * g.get_we(lv[i], lv[j]);
						}
					}
				}
			}

			return 0;
		}


	} //namespace graph_utils


} //namespace bitgraph

#endif // BITGRAPH_GRAPH_EDGE_WEIGHTED_GRAPH_UTILS_H
