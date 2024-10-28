/////////////////////////////////////
// target to draw graphs with LEDA
// imported to this package: 17/3/16
// author: pss

#include <iostream>
#include "../draw/ldraw.h"
#include "utils/common.h"
#include "utils/logger.h"
#include "../graph.h"
#include "../algorithms/graph_sort.h"

#define TEST_BENCHMARK_PATH		"C:/Users/Pablo/Desktop/results/"
//#define GRAPH					"r10_0.3_0.txt"
#define GRAPH					"r9_0.6_201.txt"

int main(){
	//graphwin
	/*Ldraw::graph_win(true);
	return 0;*/

	//draw graph 
	/*ugraph g(std::string(TEST_BENCHMARK_PATH)+GRAPH);
	Ldraw<ugraph> l;
	l.draw_circular_layout_and_edit(g);*/
	

	//draw ordered graph
	ugraph g(std::string(TEST_BENCHMARK_PATH)+GRAPH);
	Ldraw<ugraph> l;
	l.draw_circular_layout_and_edit(g);
	GraphSort<ugraph> gs(g);
	gs.new_order(gbbs::MAX_DEG_DEGEN, gbbs::PLACE_LF);
	gs.reorder(gs.new_order(gbbs::MAX_DEG_DEGEN, gbbs::PLACE_LF));
	l.draw_circular_layout_and_edit(g);
}