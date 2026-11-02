/**
 * @file test_graph_fast_sort.cpp
 * @brief Unit tests for the GraphFastRootSort class.
 *
 * @details Tests vertex ordering strategies and graph reordering
 *          operations for unweighted and vertex-weighted graphs,
 *          using the GoogleTest framework.
 *
 * @author pss
 * @date 2024
 * @last_update 02/11/2026
 */

#include "graph/algorithms/graph_fast_sort.h"	
#include "gtest/gtest.h"
#include <string>

#include "graph/simple_ugraph.h"
#include "graph/ugraph_vertex_weighted.h"

using namespace std;
using namespace bitgraph;

namespace {

	class GraphFastRootSortTest : public ::testing::Test {
	protected:
		void SetUp() override {
			ug.reset(NV);
			ug.add_edge(1, 2);
			ug.add_edge(1, 3);
			ug.add_edge(1, 4);
			ug.add_edge(3, 5);
		}
		void TearDown() override {}

		//ugraph instance	
		const int NV = 6;
		ugraph ug;
	};

	template<class GraphT>
	class TestGraphFastRootSort : public GraphFastRootSort<GraphT> {
	public:
		using base_t = GraphFastRootSort<GraphT>;

		using base_t::base_t;

		using base_t::compute_deg_root;
		using base_t::compute_support_root;

		using base_t::sort_non_increasing_deg;
		using base_t::sort_non_decreasing_deg;
		using base_t::sort_non_increasing_deg_with_support_tb;
		using base_t::sort_non_decreasing_deg_with_support_tb;

		using base_t::sort_degen_non_decreasing_deg;
		using base_t::sort_degen_non_increasing_deg;
		using base_t::sort_degen_non_decreasing_deg_B;

		using base_t::sort_degen_composite_non_decreasing_deg;
		using base_t::sort_degen_composite_non_increasing_deg;
	};
}

TEST_F(GraphFastRootSortTest, constructor) {
	using gt = GraphFastRootSort<ugraph>;
	gt sorter(ug);

	//unweighted part
	auto g = sorter.graph();
	///////////////////////////////////////////////////////////////////
	EXPECT_EQ(sorter.num_vertices(), g.num_vertices());
	EXPECT_EQ(NV, g.num_vertices());
	///////////////////////////////////////////////////////////////////

	//...
}

TEST_F(GraphFastRootSortTest, compute_deg_root) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();

	//////////////////////////////////////////
	gt::vertex_degrees_t deg_exp = { 0, 3, 1, 2, 1, 1};
	EXPECT_EQ(deg_exp, sorter.degree());
	//////////////////////////////////////////
	
	//I/O
	//cout << "degree info:" << endl;
	//sorter.print((gt::PRINT_DEGREE), cout);*/
}

TEST_F(GraphFastRootSortTest, compute_support_root) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();
	sorter.compute_support_root();

	//////////////////////////////////////////
	gt::vertex_supports_t support_exp = { 0, 4, 3, 4, 3, 2 };
	EXPECT_EQ(support_exp, sorter.support());
	//////////////////////////////////////////

	//I/O
	/*cout << "support info:" << endl;
	sorter.print((gt::PRINT_SUPPORT), cout);*/
}

TEST_F(GraphFastRootSortTest, sort_non_decreasing_deg) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();
	VertexMapping mapping = sorter.sort_non_decreasing_deg(false);

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 0, 2, 4, 5, 3, 1 };
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	//cout << "non_decreasing degree ordering:" << endl;
	//sorter.print(gt::PRINT_NODES, cout);
}

TEST_F(GraphFastRootSortTest, sort_non_increasing_deg) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();
	VertexMapping mapping = sorter.sort_non_increasing_deg(false);

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 1, 3, 2, 4, 5, 0 };
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "non_increasing degree ordering:" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}

TEST_F(GraphFastRootSortTest, sort_non_decreasing_deg_with_support_tb) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();
	sorter.compute_support_root();
	VertexMapping mapping = sorter.sort_non_decreasing_deg_with_support_tb(false);

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 0, 5, 2, 4, 3, 1 };
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "non-decreasing degree ordering with support tb:" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}

TEST_F(GraphFastRootSortTest, sort_non_increasing_deg_with_support_tb) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();
	sorter.compute_support_root();
	VertexMapping mapping = sorter.sort_non_increasing_deg_with_support_tb(false);

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 1, 3, 2, 4, 5, 0 };
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "non-increasing degree ordering with support tb:" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}

TEST_F(GraphFastRootSortTest, sort_degen_non_decreasing_deg) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();	
	VertexMapping mapping = sorter.sort_degen_non_decreasing_deg(false);

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 0, 2, 4, 1, 3, 5 };
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "non-decreasing degenerate ordering:" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}

TEST_F(GraphFastRootSortTest, sort_degen_non_decreasing_deg_ltf) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();	
	VertexMapping mapping = sorter.sort_degen_non_decreasing_deg(true);

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 0, 2, 4, 1, 3, 5 };						//taken from previous test
	std::reverse(mapping_exp.begin(), mapping_exp.end());
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "non-decreasing degenerate ordering last-to-first:" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}


TEST_F(GraphFastRootSortTest, sort_degen_non_increasing_deg) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();	
	VertexMapping mapping = sorter.sort_degen_non_increasing_deg(false);

	//TODO - complete test

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 1, 3, 0, 2, 4, 5 };
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "non-increasing degenerate ordering:" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}

TEST_F(GraphFastRootSortTest, sort_degen_non_increasing_deg_ltf) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();	
	VertexMapping mapping = sorter.sort_degen_non_increasing_deg(true);

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 1, 3, 0, 2, 4, 5 };							//taken from previous test
	std::reverse(mapping_exp.begin(), mapping_exp.end());
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "non-increasing degenerate ordering last-to-first:" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}

/////////////////
//
// Composite orderings
//
/////////////////

TEST_F(GraphFastRootSortTest, sort_degen_composite_non_decreasing_deg) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();
	sorter.sort_non_decreasing_deg(false);										//ordering I
	VertexMapping mapping = sorter.sort_degen_composite_non_decreasing_deg(false);		//ordering II (tbs are based on ordering I)

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 0, 2, 4, 5, 3, 1 };
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "composite: (i) non-decreasing + (ii) non-decreasing degree degenerate ordering" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}

TEST_F(GraphFastRootSortTest, sort_degen_composite_non_decreasing_deg_ltf) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();
	sorter.sort_non_decreasing_deg(false);											//ordering I
	VertexMapping mapping = sorter.sort_degen_composite_non_decreasing_deg(true);		//ordering II (tbs are based on ordering I)

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 0, 2, 4, 5, 3, 1 };
	std::reverse(mapping_exp.begin(), mapping_exp.end());
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "composite: (i) non-decreasing + (ii) non-decreasing degree degenerate ordering last-to-first" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}

TEST_F(GraphFastRootSortTest, sort_degen_composite_non_increasing_deg) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();
	sorter.sort_non_increasing_deg(false);												//ordering I
	VertexMapping mapping = sorter.sort_degen_composite_non_increasing_deg(false);		//ordering II (tbs are based on ordering I)

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 1, 3, 2, 4, 5, 0 };
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	//cout << "composite: (i) non-decreasing + (ii) non-increasing degree degenerate ordering" << endl;
	//sorter.print(gt::PRINT_NODES, cout);
}

TEST_F(GraphFastRootSortTest, sort_degen_composite_non_increasing_deg_ltf) {

	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);
	sorter.compute_deg_root();
	sorter.sort_non_increasing_deg(false);											//ordering I
	VertexMapping mapping = sorter.sort_degen_composite_non_increasing_deg(true);		//ordering II (tbs are based on ordering I)

	////////////////////////////////////////////
	VertexMapping mapping_exp = { 1, 3, 2, 4, 5, 0 };
	std::reverse(mapping_exp.begin(), mapping_exp.end());
	EXPECT_EQ(mapping_exp, mapping);
	////////////////////////////////////////////

	//I/O
	/*cout << "composite: (i) non-increasing + (ii) non-increasing degree degenerate ordering last-to-first" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/
}

TEST_F(GraphFastRootSortTest, new_order) {

	using gt = GraphFastRootSort<ugraph>;
	gt sorter(ug);

	//absolute min degree ordering first-to-last and old-to-new 
	//nodes_ = {0, 2, 4, 5, 3, 1}
	VertexMapping mapping = sorter.new_order((gt::MIN), gt::FIRST_TO_LAST, gt::OLD_TO_NEW);

	//////////////////////////////////////////
	EXPECT_EQ(mapping[0], 0);					/* old to new */
	EXPECT_EQ(mapping[2], 1);
	EXPECT_EQ(mapping[4], 2);
	EXPECT_EQ(mapping[5], 3);
	EXPECT_EQ(mapping[3], 4);
	EXPECT_EQ(mapping[1], 5);
	//////////////////////////////////////////

	//I/O
	/*cout <<"degenerate minimum degree ordering - format last-to-first and old-to-new" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/

}

TEST_F(GraphFastRootSortTest, reorder) {
	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);

	sorter.compute_deg_root();
	VertexMapping mapping_o2n=sorter.sort_non_decreasing_deg(false);					//nodes_ = {0, 2, 4, 5, 3, 1}
																			//deg =	{ 0, 3, 1, 2, 1, 1 };
	//compute isomorphism
	ugraph ugn = sorter.reorder(mapping_o2n);
		
	EXPECT_EQ(ug.num_vertices(), ugn.num_vertices());
	EXPECT_EQ(ug.num_edges(), ugn.num_edges());
	EXPECT_EQ(ug.degree(0), ugn.degree(0));
	EXPECT_EQ(ug.degree(2), ugn.degree(1));
	EXPECT_EQ(ug.degree(1), ugn.degree(2));					
	EXPECT_EQ(ug.degree(4), ugn.degree(3));
	EXPECT_EQ(ug.degree(5), ugn.degree(4));					
	EXPECT_EQ(ug.degree(3), ugn.degree(5));						
	
}


TEST_F(GraphFastRootSortTest, reorder_static) {
	using gt = TestGraphFastRootSort<ugraph>;
	gt sorter(ug);

	sorter.compute_deg_root();
	VertexMapping mapping_o2n = sorter.sort_non_decreasing_deg(false);					//nodes_ = {0, 2, 4, 5, 3, 1}
	//deg =	{ 0, 3, 1, 2, 1, 1 };BITGRAPH_GRAPH_TEST_DATA_DIR

	
	//////////////////////////////////////////////////////
	ugraph ugn = gt::reorder(ug, mapping_o2n, nullptr);							//compute isomorphism
	//////////////////////////////////////////////////////

	EXPECT_EQ(ugn.num_vertices(), ug.num_vertices());	
	EXPECT_EQ(ugn.num_edges(), ug.num_edges());

	////////////////////////////////////////////
	EXPECT_EQ(ug.num_vertices(), ugn.num_vertices());
	EXPECT_EQ(ug.num_edges(), ugn.num_edges());
	EXPECT_EQ(ug.degree(0), ugn.degree(0));
	EXPECT_EQ(ug.degree(2), ugn.degree(1));
	EXPECT_EQ(ug.degree(1), ugn.degree(2));
	EXPECT_EQ(ug.degree(4), ugn.degree(3));
	EXPECT_EQ(ug.degree(5), ugn.degree(4));
	EXPECT_EQ(ug.degree(3), ugn.degree(5));
	////////////////////////////////////////////
}

TEST(GraphFastRootSort, new_order_dimacs) {

	using gt = GraphFastRootSort<ugraph>;
	string name = "brock200_2.clq";
	name.insert(0, BITGRAPH_GRAPH_TEST_DATA_DIR);

	ugraph ug(name);
	gt sorter(ug);

	//degenerate min degree ordering
	VertexMapping mapping = sorter.new_order((gt::MIN_DEGEN), gt::LAST_TO_FIRST, gt::NEW_TO_OLD);

	//////////////////////////////////////////
	EXPECT_EQ(mapping[0],	199);
	EXPECT_EQ(mapping[1],	194);
	EXPECT_EQ(mapping[199], 139);
	//////////////////////////////////////////

	//I/O
	/*cout <<"degenerate minimum degree ordering - format last-to-first and old-to-new" << endl;
	sorter.print(gt::PRINT_NODES, cout);*/


	//composite ordering based on the previous ordering
	VertexMapping mapping_compo = sorter.new_order((gt::MIN_DEGEN_COMPO), gt::LAST_TO_FIRST, gt::NEW_TO_OLD);

	//////////////////////////////////////////
	EXPECT_EQ(mapping_compo[0], 69);
	EXPECT_EQ(mapping_compo[1], 9);
	EXPECT_EQ(mapping_compo[199], 139);
	//////////////////////////////////////////

	//I/O
	//cout <<"composite degenerate minimum degree ordering - format last-to-first and old-to-new" << endl;
	//sorter.print(gt::PRINT_NODES, cout);
}

////////////////////////////////////
// 
// TESTS for sorting subgraphs
// (experimental)
//
////////////////////////////////////

TEST(subgraphSort, first_k) {
	
	const int NV = 6;
	ugraph ug(NV);
	ug.add_edge(1, 2);
	ug.add_edge(1, 5);			
	ug.add_edge(2, 4);
	ug.add_edge(2, 5);
	ug.add_edge(3, 5);										//deg(0)=0, deg(1)=2, deg(2)=3, deg(3)=1, deg(4)=1, deg(5)=3 

	//setup
	using gt = TestGraphFastRootSort<ugraph>;
	const int K = 3;
	gt sorter(ug);
	sorter.compute_deg_root();

	//sorts first K=3 vertices according to non-increasing deg
	VertexMapping mapping = sorter.sort_non_increasing_deg(K, false);
		
	//I/O
	//sorter.print(gt::PRINT_NODES, cout);

	//////////////////////////////////////////
	VertexMapping mapping_exp = { 2, 1, 0, 3, 4, 5 };				//vertices 0, 1, 2 are sorted according to non-increasing deg
	EXPECT_EQ(mapping.size(), NV);
	EXPECT_EQ(mapping, mapping_exp);
	//////////////////////////////////////////


	//sorts first K=3 vertices according to non-decreasing deg
	mapping = sorter.sort_non_decreasing_deg(K, false);
		
	//I/O
	//sorter.print(gt::PRINT_NODES, cout);

	//////////////////////////////////////////
	mapping_exp = { 0, 1, 2, 3, 4, 5 };						//vertices 0, 1, 2 are sorted according to non-increasing deg
	EXPECT_EQ(mapping.size(), NV);
	EXPECT_EQ(mapping, mapping_exp);
	//////////////////////////////////////////

}

TEST(subgraphSort, first_to_last) {

	const int NV = 6;
	ugraph ug(NV);
	ug.add_edge(1, 2);
	ug.add_edge(1, 5);
	ug.add_edge(2, 4);
	ug.add_edge(2, 5);
	ug.add_edge(3, 5);										//deg(0)=0, deg(1)=2, deg(2)=3, deg(3)=1, deg(4)=1, deg(5)=3 

	//setup
	using gt = TestGraphFastRootSort<ugraph>;
	const int FIRST = 1, LAST=2;
	gt sorter(ug);
	sorter.compute_deg_root();

	//sorts vertices in the interval [FIRST, LAST] according to non-increasing deg
	VertexMapping mapping = sorter.sort_non_increasing_deg(FIRST, LAST, false);

	//I/O
	//sorter.print(gt::PRINT_NODES, cout);

	//////////////////////////////////////////
	VertexMapping mapping_exp = { 0, 2, 1, 3, 4, 5 };
	EXPECT_EQ(mapping.size(), NV);
	EXPECT_EQ(mapping, mapping_exp);
	//////////////////////////////////////////


	//sorts vertices in the interval [FIRST, LAST] according to non-decreasing deg
	mapping = sorter.sort_non_decreasing_deg(FIRST, LAST, false);

	//I/O
	//sorter.print(gt::PRINT_NODES, cout);

	////////////////////////////////////////////
	mapping_exp = { 0, 1, 2, 3, 4, 5 };						
	EXPECT_EQ(mapping.size(), NV);
	EXPECT_EQ(mapping, mapping_exp);
	//////////////////////////////////////////

}

TEST(subgraphSort, newOrderSubgraph) {

	const int NV = 6;
	ugraph ug(NV);
	ug.add_edge(1, 2);
	ug.add_edge(1, 3);
	ug.add_edge(1, 5);
	ug.add_edge(2, 4);
	ug.add_edge(2, 5);
	ug.add_edge(3, 5);										

	//setup
	using gt = GraphFastRootSort<ugraph>;
	gt sorter(ug);
	auto bbsg = bitgraph::make_BBScan(NV, { 1, 3, 4 });	  //deg(1)=2, deg(3)=2, deg(4)=0 in the induced subgraph G[{1, 3, 4}]

	///////////////////////////////////////////////////////////////////
	VertexMapping ord = sorter.new_order(gt::MAX, bbsg, true, false /* n2o */ );
	//////////////////////////////////////////////////////////////////
		
	////////////////////////////////////////////
	VertexMapping ord_exp = { 0, 4, 2, 3, 1, 5 };					//only vertices in the subgraph are sorted
	EXPECT_EQ(ord.size(), NV);
	EXPECT_EQ(ord, ord_exp);
	//////////////////////////////////////////
}

TEST(GraphFastRootSort, is_valid_permutation)
{
	using ordering_t = vertex_ordering_t;
	using sorter_t = GraphFastRootSort<ugraph>;

	EXPECT_TRUE(sorter_t::is_valid_permutation(
		ordering_t{ 2, 0, 3, 1 }));

	EXPECT_TRUE(sorter_t::is_valid_permutation(
		ordering_t{ 0, 1, 2, 3 }));

	EXPECT_TRUE(sorter_t::is_valid_permutation(
		ordering_t{}));

	EXPECT_FALSE(sorter_t::is_valid_permutation(
		ordering_t{ 0, 1, 1, 3 }));  // duplicate

	EXPECT_FALSE(sorter_t::is_valid_permutation(
		ordering_t{ 0, 1, 2, 4 }));  // out of range

	EXPECT_FALSE(sorter_t::is_valid_permutation(
		ordering_t{ -1, 0, 1 }));    // negative index
}


