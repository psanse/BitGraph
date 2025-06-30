//filter_graph_sort_type.h : filter types for GraphSort<T> metaclass 
//							 Graphs which are not undirected cannot be sorted

#ifndef __FILTER_GRAPH_SORT_TYPE__
#define __FILTER_GRAPH_SORT_TYPE__

template <class Graph_t>
struct filterGraphSortType;		//error at compile time

//valid specializations
template <>
struct filterGraphSortType<bitgraph::ugraph>{};
template <>
struct filterGraphSortType<bitgraph::sparse_ugraph>{};

template <>
struct filterGraphSortType<bitgraph::ugraph_w>{};		/* for weighted graphs */
#endif