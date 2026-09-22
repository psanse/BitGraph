 /**
  * @file graph_conversions.h
  * @brief Declares conversions between dense and sparse undirected graphs.
  *
  * The convert_graph() overloads copy a graph between the ugraph and
  * sparse_ugraph representations. The destination graph is reset, so its
  * previous contents are discarded. Vertex indices and adjacency are
  * preserved.
  *
  * @date Created: 22/09/2026
  * @author Pablo San Segundo
  */

#ifndef BITGRAPH_GRAPH_CONVERSIONS_H
#define BITGRAPH_GRAPH_CONVERSIONS_H

#include "graph/graph_unweighted.h"

namespace bitgraph {

	/**
	 * @brief Converts a sparse undirected graph to a dense graph.
	 *
	 * @param source Sparse input graph.
	 * @param destination Dense output graph. Its previous contents are discarded.
	 *
	 * @note The destination's edge count is recomputed when next requested.
	 */
	void convert_graph(const sparse_ugraph& source, ugraph& destination);


	/**
	 * @brief Converts a dense undirected graph to a sparse graph.
	 *
	 * @param source Dense input graph.
	 * @param destination Sparse output graph. Its previous contents are discarded.
	 *
	 * @note The destination's edge count is recomputed when next requested.
	 */
	void convert_graph(const ugraph& source, sparse_ugraph& destination);

} // namespace bitgraph

#endif // BITGRAPH_GRAPH_CONVERSIONS_H

