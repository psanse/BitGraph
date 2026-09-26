/**
* @file vertex_weighted_graph_utils.h
* @brief main header for namespace gfunc providing general purpose functions for graphs
* @details: 
*  - date 07/3/2017
*  - last_modified 30/01/2025  
* @author pss
*
* @todo CHECK edgeW::ew_shift_2_highest_index function  (09/01/25)
**/

#ifndef BITGRAPH_GRAPH_VERTEX_WEIGHTED_GRAPH_UTILS_H
#define BITGRAPH_GRAPH_VERTEX_WEIGHTED_GRAPH_UTILS_H

#include "graph/graph_vertex_weighted.h"
#include "graph/graph_traits.h"
#include "utils/logger.h"
#include "utils/sort_utils.h"
#include <algorithm>
#include <iostream>

namespace bitgraph {

	namespace graph_utils {
						
			/*
			* @brief sum of weights of the vertices in a (bit)set (bbref)
			*/
			template<typename GraphT>
			typename GraphT::Weight wsum(const GraphT& g, typename GraphT::vertex_bitset_t& bbref) {

				typename GraphT::Weight total_weight = 0;

				//bitscanning configuration
				bbref.init_scan(bbo::NON_DESTRUCTIVE);

				//bitscan to sum weights
				int v;
				while ((v = bbref.next_bit()) != EMPTY_ELEM) {
					total_weight += g.weight(v);
				}

				return total_weight;
			}

			/*
			* @brief sum of weights of the vertices in a set (lv)
			*/
			template<typename GraphT>
			typename GraphT::Weight wsum(const GraphT& g, Vertices& lv) {

				typename GraphT::Weight total_weight = 0;

				//iterate and add
				for (int v : lv) {
					total_weight += g.weight(v);
				}

				return total_weight;
			}

			/*
			* @brief sum of weights of the vertices in a graph
			*/
			template<typename GraphT>
			typename GraphT::Weight wsum(const GraphT& g) {

				typename GraphT::Weight total_weight = 0;

				// Obtiene el vector de pesos de los vértices
				const auto& weights = g.weights();

				// Recorre todos los pesos y los suma
				for (const auto& weight : weights) {
					total_weight += weight;
				}

				return total_weight;
			}


			/*
			* @brief sum of weights the neighborhood of a vertex (v) in a (bit)set of vertices (bbref)
			*		 plus the weight of vertex v
			*/
			template<typename GraphT>
			typename GraphT::Weight wsum(const GraphT& g, Vertex v, typename GraphT::vertex_bitset_t& bbref) {

				auto total_weight = g.weight(v);

				//bitscanning configuration
				bbref.init_scan(bbo::NON_DESTRUCTIVE);

				//bitscan to sum weights
				Vertex w = bbo::noBit;
				while ((w = bbref.next_bit()) != bbo::noBit) {
					if (g.is_edge(v, w)) {
						total_weight += g.weight(w);
					}
				}

				return total_weight;
			}

			/*
			* @brief sum of weights the neighborhood of a vertex (v)
			*		 plus the weight of vertex v
			*/
			template<typename GraphT>
			typename GraphT::Weight wsum(GraphT& g, Vertex v) {

				auto total_weight = g.weight(v);
				const auto& bbn = g.neighbors(v);

				//bitscanning configuration
				if (bbn.init_scan(bbo::NON_DESTRUCTIVE) != -1) {
					Vertex u = BBObject::noBit;
					while ((u = bbn.next_bit()) != BBObject::noBit) {
						total_weight += g.weight(u);
					}
				}

				return total_weight;
			}

			/*
			* @brief sorts a set of vertices according to non-increasing or non-decreasing weights
			* @param g: a simple graph
			* @param lv: std::vector of vertices to be sorted
			* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
			* @returns sorted set of vertices
			*/
			template<typename GraphT>
			Vertices& sort_w(const GraphT& g, Vertices& lv, bool min_sort = true) {

				//sorting criteria
				const auto& weights = g.weight();

				//sorting lambdas
				auto compare_smaller = [&weights](int a, int b) {
					return weights[a] < weights[b];
					};

				auto compare_greater = [&weights](int a, int b) {
					return weights[a] > weights[b];
					};

				//sorts vertices according to the given criteria
				if (min_sort) {
					std::sort(lv.begin(), lv.end(), compare_smaller);
				}
				else {
					std::sort(lv.begin(), lv.end(), compare_greater);
				}

				return lv;
			}

			/*
			* @brief sorts a C-style set of vertices according to non-increasing or non-decreasing weights
			* @param g: a simple graph
			* @param lv: pointer to an array of vertices to be sorted
			* @param: size: number of vertices in the array
			* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
			* @returns pointer to the sorted array
			*/
			template<typename GraphT>
			Vertex* sort_w(const GraphT& g, Vertex* lv, std::size_t size, bool min_sort = true) {

				//sorting criteria
				const auto& weights = g.weight();

				//sorting lambdas
				auto compare_smaller = [&weights](int a, int b) {
					return weights[a] < weights[b];
					};

				auto compare_greater = [&weights](int a, int b) {
					return weights[a] > weights[b];
					};

				//sorting according to ldeg
				if (min_sort) {
					std::sort(lv, lv + size, compare_smaller);
				}
				else {
					std::sort(lv, lv + size, compare_greater);
				}
				return lv;
			}

			/*
			* @brief sorts a set of vertices according to weight x degree
			* @param g: a simple graph
			* @param lv: set of vertices
			* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
			* @returns sorted set of vertices
			*/
			template<typename GraphT>
			Vertices& sort_wdProd(const GraphT& g, Vertices& lv, bool min_sort = true) {

				//weights as part of the sorting criteria
				const auto& weights = g.weights();

				//sorting lambdas - degree * weight criteria
				auto compare_smaller = [&g, &weights](int a, int b) {
					return (weights[a] * g.degree(a)) < (weights[b] * g.degree(b));
					};

				auto compare_greater = [&g, &weights](int a, int b) {
					return (weights[a] * g.degree(a)) > (weights[b] * g.degree(b));
					};

				//sorting according to lambdas
				if (min_sort) {
					std::sort(lv.begin(), lv.end(), compare_smaller);
				}
				else {
					std::sort(lv.begin(), lv.end(), compare_greater);
				}
				return lv;
			}


			/*
			* @brief sorts a C-style set of vertices according to weight x degree
			* @param g: a simple graph
			* @param lv: set of vertices
			* @param size: number of vertices in the array
			* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
			* @returns pointer to the sorted array
			*/
			template<typename GraphT>
			Vertex* sort_wdProd(const GraphT& g, Vertex* lv, int size, bool min_sort = true) {

				//weights as part of the sorting criteria
				const auto& weights = g.weights();

				//sorting lambdas - degree * weight criteria
				auto compare_smaller = [&g, &weights](int a, int b) {
					return (weights[a] * g.degree(a)) < (weights[b] * g.degree(b));
					};

				auto compare_greater = [&g, &weights](int a, int b) {
					return (weights[a] * g.degree(a)) > (weights[b] * g.degree(b));
					};

				//sorting according to ldeg
				if (min_sort) {
					std::sort(lv, lv + size, compare_smaller);
				}
				else {
					std::sort(lv, lv + size, compare_greater);
				}

				return lv;
			}

			/*
			* @brief sorts a set of vertices according to weight - degree
			* @param g: a simple graph
			* @param lv: set of vertices
			* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
			* @returns sorted set of vertices
			*/
			template<typename GraphT>
			Vertices& sort_wdDif(const GraphT& g, Vertices& lv, bool min_sort = true) {

				//weights as part of the sorting criteria
				const auto& weights = g.weights();

				//sorting lambdas - weight - degree criteria
				auto compare_smaller = [&g, &weights](int a, int b) {
					return (weights[a] - g.degree(a)) < (weights[b] - g.degree(b));
					};

				auto compare_greater = [&g, &weights](int a, int b) {
					return (weights[a] - g.degree(a)) > (weights[b] - g.degree(b));
					};

				/*utils::has_smaller_val_diff<int, typename vector<GraphT::Weight>> my_struct_smaller_diff(g.weights());
				utils::has_greater_val_diff<int, typename vector<GraphT::Weight>> my_struct_greater_diff(g.weights());*/

				//sorting according to ldeg
				if (min_sort) {
					std::sort(lv.begin(), lv.end(), compare_smaller);
				}
				else {
					std::sort(lv.begin(), lv.end(), compare_greater);
				}
				return lv;
			}

			/*
			* @brief sorts a C-style set of vertices according to weight - degree
			* @param g: a simple graph
			* @param lv: set of vertices
			* @param size: number of vertices in the array
			* @param min_sort: sorting flag (true: non-decreasing, false: non-increasing)
			* @returns pointer to the sorted array
			*/
			template<typename GraphT>
			Vertex* sort_wdDif(const GraphT& g, Vertex* lv, int size, bool min_sort = true) {

				//weights as part of the sorting criteria
				const auto& weights = g.weights();

				//sorting lambdas - weight - degree criteria
				auto compare_smaller = [&g, &weights](int a, int b) {
					return (weights[a] - g.degree(a)) < (weights[b] - g.degree(b));
					};

				auto compare_greater = [&g, &weights](int a, int b) {
					return (weights[a] - g.degree(a)) > (weights[b] - g.degree(b));
					};

				//sorting according to ldeg
				if (min_sort) {
					std::sort(lv, lv + size, compare_smaller);
				}
				else {
					std::sort(lv, lv + size, compare_greater);
				}
				return lv;
			}
		
				
	} //namespace graph_utils

	
} //namespace bitgraph

#endif // BITGRAPH_GRAPH_VERTEX_WEIGHTED_GRAPH_UTILS_H
