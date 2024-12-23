/*
* graph_fast_sort_test.cpp unit tests for graph_fast_sort.h
* @date 
* @last_update 20/12/24
* @TODO - REFACTOR TESTS!, logs etc.. (20/12/2024)
* @TODO - Check logger! (20/12/2024)
*/ 

//#define LOGGER_DEBUG_LEVEL		//all except DEBUG (second priority) -  CMake option by default
//constexpr int LOGGER_DEBUG_LEVEL = 0;

#include "../algorithms/graph_fast_sort.h"
#include "../algorithms/graph_fast_sort_weighted.h"
#include "../graph.h"
#include "gtest/gtest.h"
#include <iostream>
//#include "../algorithms/graph_sort.h"
#include "utils/common.h"
#include "utils/logger.h"
#include "utils/file.h"
#include <string>

using namespace std;
//using namespace gbbs;

#define PATH		 "C:/Users/pablo/Desktop/dimacs/"
#define LOG_FILE	 "C:/Users/pablo/Desktop/log.txt"

#define TEST_GRAPH_FAST_SORT_STEP_BY_STEP				//DEF-OFF
#define print_graph_fast_sort_logs


TEST(GraphFastRootSort_W,basic) {

	LOG_INFO("GraphFastRootSort_W: basic-----------------------");
	const int NV = 6;
	ugraph_w ugw(NV);		//unti weights (1.0) to all vertices

	ugw.add_edge(1, 2);				
	ugw.add_edge(1, 5);			
	ugw.add_edge(2, 4);
	ugw.add_edge(2, 5);
	ugw.add_edge(3, 5);

	ugw.set_w(0, 4.0);
	ugw.set_w(1, 5.0);
	ugw.set_w(2, 3.0);

	using gt = GraphFastRootSort_W<ugraph_w>;
	gt gf(ugw);

//////////////////
//SORT BY DEGREE
//////////////////
	vint order= gf.new_order((int)gt::ptype::sort_alg_t::MIN, false /*f2l*/, false /*n2o*/);
	
	//deg(5)=3,  deg(2)=3, deg(1)=2,  deg(3)=1, deg(4)=1, deg(0)=0
	vint order_exp;
	order_exp.push_back(0);
	order_exp.push_back(3);
	order_exp.push_back(4);
	order_exp.push_back(1);
	order_exp.push_back(2);
	order_exp.push_back(5);

	//////////////////
	//TEST
	EXPECT_EQ(order_exp, order);

	   	
#ifdef  print_graph_fast_sort_logs
	com::stl::print_collection(order, cout, true);
#endif

//////////////////
//SORT BY WEIGHT (non-increasing)
//////////////////
	//w(1)=5.0,  w(0)=4.0, w(2)=3.0,  w(3)=1.0, w(4)=1.0, w(5)=1.0
	order = gf.new_order((int)gt::sort_algw_t::MAX_WEIGHT, false /*f2l*/, false /*n2o*/);			
	order_exp.clear();
	order_exp.push_back(1);
	order_exp.push_back(0);
	order_exp.push_back(2);
	order_exp.push_back(3);
	order_exp.push_back(4);
	order_exp.push_back(5);

	//////////////////
	//TEST
	EXPECT_EQ(order_exp, order);

#ifdef  print_graph_fast_sort_logs
	com::stl::print_collection(order, cout, true);
#endif		  
	
////////////////////
//REORDER  (by non-increasing weight)
//////////////////
	ugraph_w ugw_sorted;
	gf.reorder(order, ugw_sorted);

	//////////////////
	//TEST
	EXPECT_TRUE(5, ugw_sorted.get_w(0));
	EXPECT_TRUE(4, ugw_sorted.get_w(1));
	EXPECT_TRUE(3, ugw_sorted.get_w(2));
	EXPECT_TRUE(1, ugw_sorted.get_w(3));
	EXPECT_TRUE(1, ugw_sorted.get_w(4));
	EXPECT_TRUE(1, ugw_sorted.get_w(5));

#ifdef  print_graph_fast_sort_logs
	ugw_sorted.print_weights();
#endif


	LOG_INFO("GraphFastRootSort_W: END basic-------------------");
#ifdef	TEST_GRAPH_FAST_SORT_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
}


TEST(Fast_Sorting_stateless, SORT_SUBGRAPH_DEG) {
///////////////
//Very experimental function (06/01/2021)

	LOG_INFO("Fast_Sorting_stateless: SORT_SUBGRAPH_DEG-----------------------");

	const int NV = 6;
	ugraph ug(NV);
	ug.add_edge(1, 2);
	ug.add_edge(1, 5);			//deg(5)=2, deg(4)=1, deg(3)=0 wrt to the set {0, 1, 2}
	ug.add_edge(2, 4);
	ug.add_edge(2, 5);
	ug.add_edge(3, 5);

	//////////////////////////  
	//sorts the second half of the vertices according to degenerate deg wrt to the first half
	int n = (NV / 2);
	vint rhs, lhs;
	for (int i = 0; i < n; i++) {
		rhs.push_back(i);
		lhs.push_back(i + n);
	}
	///////////////////
	   
	int status=GraphFastRootSort<ugraph>::SORT_SUBGRAPH_NON_INC_DEG(ug, lhs, rhs, true /* lhs-> max_deg in rhs, FIRST_TO_LAST */);     //TODO@ TEST SORT_SUBGRAPH_NON_DEC_DEG(ug, lhs, rhs, true /* lhs-> max_deg in rhs, FIRST_TO_LAST */);
					
	vint res;
	res.push_back(5); 
	res.push_back(4); 
	res.push_back(3);
	EXPECT_EQ(res, lhs);
	EXPECT_EQ(res.size(), lhs.size());
		

	LOG_INFO("Fast_Sorting_stateless: END SORT_SUBGRAPH_DEG-------------------");
#ifdef	TEST_GRAPH_FAST_SORT_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
}


TEST(Fast_Sorting, gen_min_width_tb_support_graphs) {
	LOG_INFO("Fast_Sorting: gen_min_width_tb_support_graphs-------------------------");
	//string name = "brock400_1.clq";
	//string name = "p_hat700-3.clq";
	//string name = "gen400_p0.9_75.clq";
	//string name = "keller5.clq";
	string name = "C250.9.clq";
	
	string fullname = PATH;
	fullname += name;

	cout << fullname << endl;
	ugraph ug(fullname);

	using gt = GraphFastRootSort<ugraph>;
	gt cfs(ug);
	
	vint otn_minwc = cfs.new_order((int)gt::sort_alg_t::MIN_DEGEN_COMPO, true);
	cout << "MAPPING MIN_WIDTH_DEGEN_COMPOSITE LAST TO FIRST-old to new" << endl;
	com::stl::print_collection<vint>(otn_minwc, cout, true);
		
	//copy to file	
	ugraph ugo;
	cfs.reorder(otn_minwc, ugo);
	ugo.print_data();
	ugo.write_dimacs(FILE_LOG((ugo.get_name() + "s").c_str(), WRITE));

	LOG_INFO("Fast_Sorting: END gen_min_width_tb_support_graphs-------------------");
#ifdef	TEST_GRAPH_FAST_SORT_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
}


TEST(Fast_Sorting, DISABLED_dimacs) {
	LOG_INFO("Fast_Sorting: dimacs-------------------------");
	string name = "brock200_2.clq";
	string fullname = PATH;
	fullname += name;
	
	cout << fullname << endl;
	ugraph ug(fullname);

	using gt = GraphFastRootSort<ugraph>;
	gt cfs(ug);
			
	vint otn_minw=cfs.new_order((int)gt::sort_alg_t::MIN_DEGEN, true);
	cout << "MAPPING MIN_WIDTH_DEGEN LAST TO FIRST-old to new" << endl;
	com::stl::print_collection<vint>(otn_minw, cout, true);										//checked with framework for a number  of graphs!

	vint otn_minwc = cfs.new_order((int)gt::sort_alg_t::MIN_DEGEN_COMPO, true);
	cout << "MAPPING MIN_WIDTH_DEGEN_COMPOSITE LAST TO FIRST-old to new" << endl;
	com::stl::print_collection<vint>(otn_minwc, cout, true);
	
	
	LOG_INFO("Fast_Sorting: END dimacs-------------------");
#ifdef	TEST_GRAPH_FAST_SORT_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
	
}


TEST(Fast_Sorting, DISABLED_basic_new_graph_sort){
	LOG_INFO("Fast_Sorting: basic_new_graph_sort-------------------------");

	ugraph ug(6);    
    ug.add_edge(1, 2);
    ug.add_edge(1, 3);
    ug.add_edge(1, 4);
	ug.add_edge(3, 5);
		
	using gt = GraphFastRootSort<ugraph>;
	gt cfs(ug);
		
	//cfs.print(GraphFastSort<ugraph>::PRINT_ADY_LIST, cout);
	//cfs.print(GraphFastSort<ugraph>::PRINT_DEGREE, cout);
	//cfs.print(GraphFastSort<ugraph>::PRINT_SUPPORT, cout);
		
	cfs.compute_deg_root();
	cout <<"degree info:" << endl;
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_DEGREE), cout);

	cfs.compute_support_root();
	//cfs.print(GraphFastRootSort<ugraph>::PRINT_DEGREE, cout);
	cout << "support info:" << endl;
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_SUPPORT), cout);

	cfs.sort_non_decreasing_deg();
	cout << "non_decreasing mapping:" << endl;
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);
	
	cfs.sort_non_increasing_deg();	
	cout << "non_increasing mapping:" << endl;
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);
	
	cout << "non_decreasing support tb mapping:" << endl;
	cfs.sort_non_decreasing_deg_with_support_tb();
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);
	
	cout << "non_increasing support tb mapping:" << endl;
	cfs.sort_non_increasing_deg_with_support_tb();
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);
	
///////////////
//DEGEN SORTING
	cout << endl << "DEGENERATE SORTING..." << endl;
	
	cout<<"non decreasing degen mapping:"<<endl;
	cfs.sort_degen_non_decreasing_degree();
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);

	cout << "non decreasing degen mapping reverse...." << endl;
	cfs.compute_deg_root();												//necessary after prior degen ordering
	cfs.sort_degen_non_decreasing_degree(true);
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);
	
	cout << "non increasing degen mapping:" << endl;
	cfs.compute_deg_root();
	cfs.sort_degen_non_increasing_degree();
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);

	cout << "non increasing degen mapping reverse...." << endl;
	cfs.compute_deg_root();
	cfs.sort_degen_non_increasing_degree(true);
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);
	
	//Composites
	cout << endl << "DEGENERATE COMPOSITES SORTING..." << endl;	
	
	cout << "non decreasing + non decreasing degen mapping:" << endl;
	cfs.compute_deg_root();												 //necessary after prior degen ordering
	cfs.sort_non_decreasing_deg();
	cfs.sort_degen_composite_non_decreasing_degree();
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);

	cout << "non decreasing with support tb + non decreasing degen mapping:" << endl;
	cfs.compute_deg_root();												 //necessary after prior degen ordering
	cfs.sort_non_decreasing_deg_with_support_tb();
	cfs.sort_degen_composite_non_decreasing_degree();
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);
		
	cout << "non increasing + non increasing degen mapping:" << endl;
	cfs.compute_deg_root();												 //necessary after prior degen ordering
	cfs.sort_non_increasing_deg();
	cfs.sort_degen_composite_non_increasing_degree();
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);
	
	cout << "non increasing with support tb + non increasing degen mapping:" << endl;
	cfs.compute_deg_root();												 //necessary after prior degen ordering
	cfs.sort_non_increasing_deg_with_support_tb();
	cfs.sort_degen_composite_non_increasing_degree();
	cfs.print(static_cast<int>(gt::sort_print_t::PRINT_NODES), cout);

	LOG_INFO("Fast_Sorting: END basic_new_graph_sort-------------------");
#ifdef	TEST_GRAPH_FAST_SORT_STEP_BY_STEP
	LOG_ERROR("press any key to continue");
	cin.get();
#endif
}


////////////////////
//
// OLD FRAMEWORK- see other tests (here just to inspire)
//
///////////////////


//TEST(GraphFastSort_II, random_gen){
////////////////////////////
////Compares fast ordering with previous ordering
//	LOG_INFO("GraphFastSort_II: random_gen-------------------------");
//	Result r;
//	ugraph ug;
//	RandomGen<ugraph>::create_ugraph(ug, 20000, .50);
//	ug.print_data();
//
//	LOG_INFO("STARTING_TEST");		
//	r.tic();
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_fast(MIN_DEG_DEGEN, PLACE_LF);
//	//com::stl::print_collection(vno); cout<<endl;
//	r.toc();
//	LOG_INFO("Fast-[t: "<<r.get_user_time()<<"]");
//	
//	r.tic();
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order_fast_II(MIN_DEG_DEGEN);
//	//com::stl::print_collection(vno);
//	r.toc();
//	LOG_INFO("Fast_II-[t: "<<r.get_user_time()<<"]");
//
//	EXPECT_EQ(vno, vfno);
//
//	LOG_INFO("--------------------------------------------------------------");
//	
//}
//
//TEST(GraphFastSort_II, brock200_1){
////////////////////////////
////Compares fast ordering with previous ordering
//	LOG_INFO("GraphFastSort_II: brock200_1-------------------------");
//
//	ugraph ug("brock200_1.clq");    
//
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_fast(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vno); cout<<endl;
//
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order_fast_II(MIN_DEG_DEGEN);
//	com::stl::print_collection(vfno);
//
//	EXPECT_EQ(vno, vfno);
//
//	LOG_INFO("--------------------------------------------------------------");
//	cin.get();
//}
//
//TEST(GraphFastSort_II, basic_new_graph_sort){
//	LOG_INFO("GraphFastSort_II: basic_new_graph_sort-------------------------");
//
//	ugraph ug(106);    
//    ug.add_edge(1, 2);
//    ug.add_edge(1, 3);
//    ug.add_edge(1, 4);
//	ug.add_edge(78, 5);
//
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_fast(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vno); cout<<endl;
//
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order_fast_II(MIN_DEG_DEGEN);
//	com::stl::print_collection(vfno);
//
//	EXPECT_EQ(vno, vfno);
//	EXPECT_EQ(vno.size(), vfno.size());
//
//	LOG_INFO("--------------------------------------------------------------");
//	cin.get();
//}
//
//
//
//TEST(GraphFastSort, basic_min_width){
///////////////////
//// fast_sort after Fabio and Ivana´s visit to Madrid (20/7/17)
//
//	LOG_INFO("GraphFastSort::basic_min_width----------------------------------");
//	cout<<"-----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//
//	GraphSort<ugraph> gs(ug);
//	ug.print_data(); 
//	int edges=ug.number_of_edges();
//	vector<int> new_order=gs.new_order_fast(MIN_DEG_DEGEN,PLACE_FL);
//	gs.reorder(new_order, &cout);
//
//	//solution 
//	vector<int> sol(10);
//	sol[0]=6; sol[1]=7;  sol[2]=8;  sol[3]=9; 
//	int i=4;
//	int index=0;
//	while(i<10){
//		sol[i]=index; 
//		i++; index++;
//	}
//
//	cout<<endl; ug.print_data(); cout<<endl;
//	EXPECT_EQ(SIZE, ug.number_of_vertices());
//	EXPECT_EQ(edges, ug.number_of_edges(false));
//	EXPECT_EQ(sol, new_order);
//
//	LOG_INFO("-----------------------------------");
//}
//
//TEST(GraphFastSort, basic_new_graph_sort){
//	LOG_INFO("GraphFastSort: basic_new_graph_sort-------------------------");
//
//	ugraph ug(106);    
//    ug.add_edge(1, 2);
//    ug.add_edge(1, 3);
//    ug.add_edge(1, 4);
//	ug.add_edge(78, 5);
//
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_fast(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vno); cout<<endl;
//
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vfno);
//
//	EXPECT_EQ(vno, vfno);
//
//	LOG_INFO("--------------------------------------------------------------");
//}
//
//
//
//TEST(GraphFastSort, brock200_1){
////////////////////////////
////Compares fast ordering with previous ordering
//	LOG_INFO("GraphFastSort: brock200_1-------------------------");
//
//	ugraph ug("brock200_1.clq");    
//
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_fast(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vno); cout<<endl;
//
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vfno);
//
//	EXPECT_EQ(vno, vfno);
//
//	LOG_INFO("--------------------------------------------------------------");
//}
//
//
//
//TEST(GraphFastSort, random_gen){
////////////////////////////
////Compares fast ordering with previous ordering
//	LOG_INFO("GraphFastSort: random_gen-------------------------");
//	Result r;
//	ugraph ug;
//	RandomGen<ugraph>::create_ugraph(ug, 20000, .50);
//	ug.print_data();
//
//	LOG_INFO("STARTING_TEST");		
//	r.tic();
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_fast(MIN_DEG_DEGEN, PLACE_LF);
//	//com::stl::print_collection(vno); cout<<endl;
//	r.toc();
//	LOG_INFO("Fast-[t: "<<r.get_user_time()<<"]");
//	
//	r.tic();
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order(MIN_DEG_DEGEN, PLACE_LF);
//	//com::stl::print_collection(vno);
//	r.toc();
//	LOG_INFO("Default-[t: "<<r.get_user_time()<<"]");
//
//	EXPECT_EQ(vno, vfno);
//
//	LOG_INFO("--------------------------------------------------------------");
//	
//}
//
//TEST(GraphFuriniSort, basic_new_graph_sort){
//	LOG_INFO("GraphFuriniSort: basic_new_graph_sort-------------------------");
//
//	ugraph ug(106);    
//    ug.add_edge(1, 2);
//    ug.add_edge(1, 3);
//    ug.add_edge(1, 4);
//	ug.add_edge(78, 5);
//
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_furini(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vno); cout<<endl;
//
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vfno);
//
//	//EXPECT_EQ(vno, vfno);   /* not equal*/
// 
//	LOG_INFO("--------------------------------------------------------------");
//}
//
//TEST(GraphFuriniSort, brock200_1){
////////////////////////////
////Compares fast ordering with previous ordering
//	LOG_INFO("GraphFuriniSort: brock200_1-------------------------");
//
//	ugraph ug("brock200_1.clq");    
//
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_furini(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vno); cout<<endl;
//
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order(MIN_DEG_DEGEN, PLACE_LF);
//	com::stl::print_collection(vfno);
//
//	//EXPECT_EQ(vno, vfno);   /* not equal*/
//
//	LOG_INFO("--------------------------------------------------------------");
//	
//}
//
//TEST(GraphFuriniSort, random_gen){
////////////////////////////
////Compares fast ordering with previous ordering
//	LOG_INFO("GraphFuriniSort: random_gen-------------------------");
//	Result r;
//	ugraph ug;
//	RandomGen<ugraph>::create_ugraph(ug, 20000, .50);
//	ug.print_data();
//
//	LOG_INFO("STARTING_TEST");		
//	r.tic();
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_furini(MIN_DEG_DEGEN, PLACE_LF);
//	//com::stl::print_collection(vno); cout<<endl;
//	r.toc();
//	LOG_INFO("Furini-[t: "<<r.get_user_time()<<"]");
//	
//	r.tic();
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order_fast(MIN_DEG_DEGEN, PLACE_LF);
//	//com::stl::print_collection(vno);
//	r.toc();
//	LOG_INFO("Fast-[t: "<<r.get_user_time()<<"]");
//
//	//EXPECT_EQ(vno, vfno);   /* not equal*/
//
//	LOG_INFO("--------------------------------------------------------------");
//	
//}
//
//TEST(GraphFuriniSort, random_gen_sparse){
////////////////////////////
////Compares fast ordering with previous ordering
//	LOG_INFO("GraphFuriniSort: random_gen-------------------------");
//	Result r;
//	ugraph ug;
//	RandomGen<ugraph>::create_ugraph(ug, 20000, .001);
//	ug.print_data();
//
//	LOG_INFO("STARTING_TEST");		
//	r.tic();
//	GraphSort<ugraph> cfs(ug);
//	vint vno=cfs.new_order_furini(MIN_DEG_DEGEN, PLACE_LF);
//	//com::stl::print_collection(vno); cout<<endl;
//	r.toc();
//	LOG_INFO("Furini-[t: "<<r.get_user_time()<<"]");
//	
//	r.tic();
//	GraphSort<ugraph> cs(ug);
//	vint vfno=cs.new_order_fast(MIN_DEG_DEGEN, PLACE_LF);
//	//com::stl::print_collection(vno);
//	r.toc();
//	LOG_INFO("Fast-[t: "<<r.get_user_time()<<"]");
//
//	//EXPECT_EQ(vno, vfno);   /* not equal*/
//
//	LOG_INFO("--------------------------------------------------------------");
//	cin.get();
//}
//
//
//TEST(Graph_sort, basic_min_width){
//	
//	cout<<"-----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//
//	GraphSort<ugraph> gs(ug);
//	ug.print_data(); 
//	int edges=ug.number_of_edges();
//	vector<int> new_order=gs.new_order(MIN_DEG_DEGEN,PLACE_FL);
//	gs.reorder(new_order, &cout);
//
//	//solution 
//	vector<int> sol(10);
//	sol[0]=6; sol[1]=7;  sol[2]=8;  sol[3]=9; 
//	int i=4;
//	int index=0;
//	while(i<10){
//		sol[i]=index; 
//		i++; index++;
//	}
//
//	cout<<endl; ug.print_data(); cout<<endl;
//	EXPECT_EQ(SIZE, ug.number_of_vertices());
//	EXPECT_EQ(edges, ug.number_of_edges(false));
//	EXPECT_EQ(sol, new_order);
//
//	cout<<"-----------------------------------"<<endl;
//}
//
//TEST(Graph_sort_sparse, basic_min_width){
//	
//	cout<<"-----------------------------------"<<endl;
//	const int SIZE=10;
//	sparse_ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//
//	GraphSort<sparse_ugraph> gs(ug);
//	ug.print_data(); 
//	int edges=ug.number_of_edges();
//	vector<int> new_order=gs.new_order(MIN_DEG_DEGEN,PLACE_FL);
//	gs.reorder(new_order, &cout);
//
//	//solution 
//	vector<int> sol(10);
//	sol[0]=6; sol[1]=7;  sol[2]=8;  sol[3]=9; 
//	int i=4;
//	int index=0;
//	while(i<10){
//		sol[i]=index; 
//		i++; index++;
//	}
//
//	cout<<endl; ug.print_data(); cout<<endl;
//	EXPECT_EQ(SIZE, ug.number_of_vertices());
//	EXPECT_EQ(edges, ug.number_of_edges(false));
//	EXPECT_EQ(sol, new_order);
//
//	cout<<"-----------------------------------"<<endl;
//}
//
//TEST(Graph_sort, basic_max_width){
//	
//	cout<<"-----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//
//	GraphSort<ugraph> gs(ug);
//	ug.print_data(); 
//	int edges=ug.number_of_edges();
//	vector<int> new_order=gs.new_order(MAX_DEG_DEGEN, PLACE_FL);
//	gs.reorder(new_order, &cout);
//
//	//solution 
//	vector<int> sol(10);
//	for(int i=0; i<10; i++){
//		sol[i]=i;
//	}
//	sol[1]=2; sol[2]=1;		//consequence of degenerate sorting
//	
//	cout<<endl; ug.print_data(); cout<<endl;
//	EXPECT_EQ(SIZE, ug.number_of_vertices());
//	EXPECT_EQ(edges, ug.number_of_edges(false));
//	EXPECT_EQ(sol, new_order);
//
//	cout<<"-----------------------------------"<<endl;
//}
//
//TEST(Graph_sort, basic_subgraph){
//	
//	cout<<"-----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//	bitarray subgraph(10);
//	subgraph.set_bit(1,5);
//
//	GraphSort<ugraph> gs(ug);
//	ug.print_data(); 
//	vector<int> new_order=gs.new_subg_order(MIN_DEG_DEGEN, subgraph, PLACE_FL);
//	
//	//solution 
//	vector<int> sol(subgraph.popcn64());
//	sol[0]=4; sol[1]=5; //isolani first
//	sol[2]=1; sol[3]=2; sol[4]=3;
//	EXPECT_EQ(sol, new_order);
//
//	cout<<"-----------------------------------"<<endl;
//}
//
//
//TEST(Graph_sort, basic_kcore){
//	cout<<"-----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//
//	GraphSort<ugraph> gs(ug);
//	ug.print_data(); 
//	
//	//non decreasing kcore ordering
//	vector<int> new_order=gs.new_order(KCORE,PLACE_FL);
//	com::stl::print_collection<vector<int>>(new_order); cout<<endl;
//
//	vector<int> sol(10);
//	sol[0]=6; sol[1]=7;  sol[2]=8;  sol[3]=9; 
//	sol[4]=0; sol[5]=1;  sol[6]=2;  sol[7]=3; sol[8]=4; sol[9]=5;
//
//	EXPECT_EQ(sol, new_order);
//
//	//non increasing kcore ordering (standard for clique)
//	new_order=gs.new_order(KCORE,PLACE_LF);
//	com::stl::print_collection<vector<int>>(new_order);
//
//	sol[0]=3; sol[1]=2;  sol[2]=1;  sol[3]=0; 
//	sol[4]=9; sol[5]=8;  sol[6]=7;  sol[7]=6; sol[8]=5; sol[9]=4;
//
//	EXPECT_EQ(sol, new_order);
//
//	cout<<"-----------------------------------"<<endl;
//}
//
//TEST(Graph_sort, kcore_UB){
//	cout<<"-----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//
//	GraphSort<ugraph> gs(ug);
//	ug.print_data(); 
//	
//	//non decreasing kcore ordering 
//	vector<int> new_order=gs.new_order(gbbs::KCORE_UB,gbbs::PLACE_FL); //currently always PLACE_FL
//	com::stl::print_collection<vector<int>>(new_order); cout<<endl;
//	EXPECT_EQ(ug.number_of_vertices(), new_order.size());
//
//	cout<<"-----------------------------------"<<endl;
//}
//
//TEST(Graph_sort, kcore_UB_brock){
//	LOG_INFO("Graph_sort::kcore_UB_brock-----------------------------------");
//	ugraph ug("brock200_1.clq");		
//	ug.print_data(); 
//	GraphSort<ugraph> gs(ug);
//		
//	//non decreasing kcore ordering
//	vector<int> new_order=gs.new_order(KCORE_UB,PLACE_FL);				//currently always PLACE_FL
//	com::stl::print_collection<vector<int>>(new_order); cout<<endl;
//	EXPECT_EQ(ug.number_of_vertices(), new_order.size());
//	LOG_INFO("-----------------------------------");
//}
//
//TEST(Decode, basic) {
//	vector<int> v, e;
//	v.push_back(1);v.push_back(2);v.push_back(0);
//	Decode::reverse_in_place(v);
//	e.push_back(2);e.push_back(0);e.push_back(1);
//	EXPECT_EQ(e, v);
//}
//
//TEST(Graph_sort, Decode){
//////////////
//// decoding a sequence of orderings
//
//	cout<<"Order: Decode------------------------"<<endl;
//	ugraph ug(106);    
//    ug.add_edge(1, 2);
//    ug.add_edge(1, 3);
//    ug.add_edge(1, 4);
//	ug.add_edge(78, 5);
//	ug.print_data();
//		
//	ugraph ug1(ug);
//
////MIN WIDTH algorithm
//	cout<<endl<<"init min width----------------------"<<endl;
//	GraphSort<ugraph> o(ug1);
//	vint vres=o.new_order(gbbs::MIN_DEG_DEGEN, gbbs::PLACE_LF); cout<<endl;
//	copy(vres.begin(), vres.end(), ostream_iterator<int>(cout, " "));
//	o.reorder(vres);
//
//	Decode d;
//	d.reverse_in_place(vres);
//	d.insert_ordering(vres);
//
//	vint vlist;
//	vlist.push_back(105);
//	vlist.push_back(104);
//	vlist.push_back(103);
//	vlist.push_back(102);
//	vlist.push_back(101);
//	vlist.push_back(100);
//	
//	vint dec=d.decode_list(vlist);
//	cout<<endl;
//	com::stl::print_collection(dec);
//			
//	cout<<"--------------------------------------"<<endl;
//}
//
//TEST(GraphSort, support){
//	cout<<"--------------------------------------------------------"<<endl;
//
//	//-------------------------------------------------------------------------
//	//Ugraph
//	ugraph ug(100);
//	GraphSort<ugraph> o(ug);
//	ug.add_edge(0, 1);
//	ug.add_edge(1, 2);
//	ug.add_edge(2, 80);
//	EXPECT_EQ(3,o.sum_of_neighbor_deg(1));
//	cout<<"Test support ugraph passed"<<endl;
//	cout<<"--------------------------------------------------------"<<endl;
//	
//	
//	//-------------------------------------------------------------------------
//	//Sparse Ugraph
//	sparse_ugraph usg(100);
//	usg.add_edge(0, 1);
//	usg.add_edge(1, 2);
//	usg.add_edge(2, 80);
//	GraphSort<sparse_ugraph> os(usg);
//	EXPECT_EQ(3,os.sum_of_neighbor_deg(1));
//	cout<<"Test support sparse ugraph passed"<<endl;
//	cout<<"--------------------------------------------------------"<<endl;
//
//}
//
//TEST(GraphSort, normal_subgraph){		//*** TODO: change test to GRAPH block"
//
//cout<<"Order: normal_subgraph-------------------------"<<endl;
//	ugraph ug(106);    
//    ug.add_edge(1, 2);
//    ug.add_edge(1, 3);
//    ug.add_edge(1, 4);
//	ug.add_edge(78, 5);
//		
//	//subgraph
//	bitarray subg(106);
//	subg.set_bit(1, 5);
//
////MIN WIDTH algorithm
//	ugraph ug1(ug);
//	cout<<endl<<"init min width----------------------"<<endl;
//	GraphSort<ugraph> o(ug1);
//	vint vres=o.new_subg_order(gbbs::MIN_DEG_DEGEN, subg, gbbs::PLACE_FL); cout<<endl;
//	com::stl::print_collection(vres);
//	
//	vint vsol(subg.popcn64());
//	vsol[0]=5; vsol[1]=2; vsol[2]=3; vsol[3]=1; vsol[4]=4;					//MIN WIDTH first to last
//	EXPECT_EQ(vsol, vres);
//	EXPECT_EQ(subg.popcn64(),vres.size());
//	
//	//reverse order
//	ug1=ug;
//	vres=o.new_subg_order(gbbs::MIN_DEG_DEGEN, subg, gbbs::PLACE_LF); 
//	reverse(vsol.begin(), vsol.end());
//	EXPECT_EQ(vsol, vres);
//	
////MIN WIDTH MIN TIE STATIC algorithm
//	ug1=ug;
//	ug1.add_edge(2, 3);														//penalizes these edges since they now have more support
//	ug1.print_data();							
//	cout<<endl<<"init min width min tie static----------------------"<<endl;
//	GraphSort<ugraph> o1(ug1);
//	subg.print();
//	vres=o1.new_subg_order(gbbs::MIN_DEG_DEGEN_TIE_STATIC, subg,  gbbs::PLACE_FL); 
//	vsol[0]=5; vsol[1]=4; vsol[2]=1; vsol[3]=2; vsol[4]=3;	
//	EXPECT_EQ(vsol, vres);
//	EXPECT_EQ(subg.popcn64(),vres.size());
//
//	com::stl::print_collection(vres);
//	
//	cout<<"--------------------------------------"<<endl;
//}
//
//TEST(GraphSort, sparse_subgraph){
//cout<<"Order: sparse_subgraph-------------------------"<<endl;
//	sparse_ugraph ug(106);    
//    ug.add_edge(1, 2);
//    ug.add_edge(1, 3);
//    ug.add_edge(1, 4);
//	ug.add_edge(78, 5);
//		
//	//subgraph
//	sparse_bitarray subg(106);
//	subg.set_bit(1, 5);
//
////MIN WIDTH algorithm
//	sparse_ugraph ug1(ug);
//	cout<<endl<<"init min width----------------------"<<endl;
//	GraphSort<sparse_ugraph> o(ug1);
//	vint vres=o.new_subg_order(MIN_DEG_DEGEN, subg, PLACE_FL); cout<<endl;
//	com::stl::print_collection(vres);
//		
//	vint vsol(subg.popcn64());
//	vsol[0]=5; vsol[1]=2; vsol[2]=3; vsol[3]=1; vsol[4]=4;					//MIN WIDTH first to last
//	EXPECT_EQ(vsol, vres);
//	EXPECT_EQ(subg.popcn64(),vres.size());
//	
//	//reverse order
//	vres=o.new_subg_order(MIN_DEG_DEGEN, subg); 
//	reverse(vsol.begin(), vsol.end());
//	EXPECT_EQ(vsol, vres);
//	
////MIN WIDTH MIN TIE STATIC algorithm
//	ug1=ug;
//	ug1.add_edge(2, 3);	
//	
//	//penalizes these edges since they now have more support
//	cout<<endl<<"init min width min tie static----------------------"<<endl;
//	GraphSort<sparse_ugraph> o1(ug1);
//	vres=o1.new_subg_order(MIN_DEG_DEGEN_TIE_STATIC, subg, PLACE_FL); 
//	vsol[0]=5; vsol[1]=4; vsol[2]=1; vsol[3]=2; vsol[4]=3;	
//	EXPECT_EQ(vsol, vres);
//	EXPECT_EQ(subg.popcn64(),vres.size());
//
//	com::stl::print_collection(vres);
//	
//	cout<<"--------------------------------------"<<endl;
//}
//
//TEST(GraphSort, basic_new_graph_sort){
//	cout<<"Order: basic_new_graph_sort-------------------------"<<endl;
//
//	ugraph ug(106);    
//    ug.add_edge(1, 2);
//    ug.add_edge(1, 3);
//    ug.add_edge(1, 4);
//	ug.add_edge(78, 5);
//
//	GraphSort<ugraph> cs(ug);
//	cs.new_order(MIN_DEG_DEGEN, PLACE_LF);
//
//	cout<<"--------------------------------------"<<endl;
//}
//
//TEST(GraphSort_in_place, reorder_simple){
//        
//    int myints[] = {5,4,2,3,1,0};
//    std::vector<int> sol1 (myints, myints + (sizeof(myints) / sizeof(int)) );
//    int myints2[] = {5,3,1,2,4,0};
//    std::vector<int> sol2 (myints2, myints2 + (sizeof(myints2) / sizeof(int)) );
//    cout<<"--------------------------------------------------------"<<endl;
//	
//    //-------------------------------------------------------------------------
//    //Ugraphs
//    sparse_ugraph ug(106);    
//    GraphSort<sparse_ugraph> o(ug);
//    ug.add_edge(3, 1);
//    ug.add_edge(3, 2);
//    ug.add_edge(3, 4);
//    ug.add_edge(3, 5);
//    ug.add_edge(2, 5);
//    ug.add_edge(4, 5);
//    ug.add_edge(78, 5);
//
//    sparse_ugraph ug2(106);
//    GraphSort<sparse_ugraph> o2(ug2);
//    ug2.add_edge(3, 1);
//    ug2.add_edge(3, 2);
//    ug2.add_edge(3, 4);
//    ug2.add_edge(3, 5);
//    ug2.add_edge(2, 5);
//    ug2.add_edge(4, 5);
//    ug2.add_edge(78, 5);
//    cout<<"1:"<<endl;
//    ug.print_edges();
//    cout<<"2:"<<endl;
//    ug2.print_edges();
//    cout<<"--------------"<<endl;
//   
//	//checks reverse order
//    GraphSort<sparse_ugraph>::print(o.new_order(gbbs::NONE, gbbs::PLACE_LF));		
//    o.reorder(o.new_order(gbbs::NONE));
//    o2.reorder_in_place(o2.new_order(gbbs::NONE));
//    cout<<"1:"<<endl;
//    ug.print_edges();
//    cout<<"2:"<<endl;
//    ug2.print_edges();
//    cout<<"--------------"<<endl;
//   
//	//Comparison between both graphs edge by edge
//	//(currently operator == between graphs is not working because equality between sparse_bitarrays does not check empty bitblocks)
//    int j,j2;
//    ASSERT_TRUE(ug.number_of_vertices()==ug2.number_of_vertices());
//    for(int i=0;i<ug.number_of_vertices();i++){
//        sparse_bitarray neigh=ug.get_neighbors(i);
//		sparse_bitarray neigh2=ug2.get_neighbors(i);
//        if((neigh.init_scan(bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM) && (neigh2.init_scan(bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM)) {
//            while(true){
//                j=neigh.next_bit();
//                j2=neigh2.next_bit();
//                ASSERT_TRUE(j==j2);
//                if(j==EMPTY_ELEM)
//                        break;                
//            }
//        }
//    }
//
//    cout<<"--------------------------------------------------------"<<endl;
//}
//
//
//TEST(Graph_composite_sort, basic){
//////////////
//// decoding a sequence of orderings
//
//    cout<<"Graph_composite_sort: basic------------------------"<<endl;
//    ugraph ug(6);    
//    ug.add_edge(1, 2);
//    ug.add_edge(1, 3);
//    ug.add_edge(1, 4);
//    ug.add_edge(4, 5);
//    ug.print_data();
//
//    
//    GraphSort<ugraph> o(ug);
//    vector<pair<sort_t, place_t> > lo;
//    lo.push_back(pair<sort_t, place_t>(NONE,PLACE_LF));
//    lo.push_back(pair<sort_t, place_t>(NONE,PLACE_LF));
//
//    Decode d;
//    o.reorder_composite(lo,d,NULL);
//	vector<int> vindex(6);
//	for(int i=0; i<vindex.size(); i++){
//		vindex[i]=i;
//	}
//	vector<int> vdec=d.decode_list(vindex);		//same ordering expected since the graph remains the original one
//	EXPECT_EQ(vindex, vdec);
//	
//	
//    cout<<"--------------------------------------"<<endl;
//}
//
//
//TEST(Graph_sort,get_v_basic ){
//	
//	cout<<"Graph_sort::get_v_basic----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//	ug.print_data(); 
//	
//
//	GraphSort<ugraph> gs(ug);
/////////////////
//// GRAPH
//
//	int v=gs.get_v(gbbs::PICK_MINFL);
//	EXPECT_EQ(4,v);
//
//	v=gs.get_v(gbbs::PICK_MINLF);
//	EXPECT_EQ(9,v);
//
//	v=gs.get_v(gbbs::PICK_MAXLF);
//	EXPECT_EQ(3,v);
//
//	v=gs.get_v(gbbs::PICK_MAXFL);
//	EXPECT_EQ(0,v);
//
/////////////////
//// SUBGRAPH
//	bitarray sg(10);
//	sg.set_bit(0);
//	sg.set_bit(1);
//	sg.set_bit(3);
//	sg.set_bit(9);
//		
//	v=gs.get_v(sg, gbbs::PICK_MINFL);
//	EXPECT_EQ(9,v);
//
//	v=gs.get_v(sg, gbbs::PICK_MINLF);
//	EXPECT_EQ(9,v);
//
//	v=gs.get_v(sg, gbbs::PICK_MAXFL);
//	EXPECT_EQ(0,v);
//
//	v=gs.get_v(sg, gbbs::PICK_MAXLF);
//	EXPECT_EQ(0,v);
//	
//	cout<<"-----------------------------------"<<endl;
//}
//
//
//TEST(Graph_sort,new_order_primitives_basic){
//	cout<<"Graph_sort::new_order_basic_primitives----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//	ug.print_data(); 
//	
//	ugraph ug1(ug);
//
//	//sorting with primitives
//	GraphSort<ugraph> gs(ug1);
//	vint order=gs.new_order(gbbs::PICK_MINFL, gbbs::PLACE_LF);		//Equivalent to MIN_DEG_DEGEN
//	vint sol(SIZE);
//	sol[0]=3; sol[1]=2; sol[2]=1; sol[3]=0; sol[4]=9;
//	sol[5]=8; sol[6]=7; sol[7]=6; sol[8]=5; sol[9]=4;
//	EXPECT_EQ(sol, order);
//
//	//MIN_DEG_DEGEN, supposed to be equivalent
//	gs.reorder(order);
//	ugraph ug2(ug);
//	GraphSort<ugraph> gs2(ug2);
//	gs2.reorder(gs.new_order(gbbs::MIN_DEG_DEGEN));
//
//	EXPECT_EQ(ug2.number_of_edges(), ug1.number_of_edges());	
//	EXPECT_EQ(ug2, ug1);
//	
//	cout<<"-----------------------------------"<<endl;
//}
//
//
//TEST(Graph_sort,new_order_primitives_sg_I){
//	cout<<"Graph_sort::new_order_primitives_sg_I----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//	ug.print_data(); 
//	
//	ugraph ug1(ug);
//
//	//sorting with primitives
//	GraphSort<ugraph> gs(ug1);
//	bitarray sg(SIZE);
//
//	//will only sort G[{0,...,3}]
//	sg.set_bit(0,3);
//
//	vint order=gs.new_order(sg, gbbs::PICK_MINFL, gbbs::PLACE_LF);				
//	com::stl::print_collection(order);
//
//	vint sol(SIZE);
//	sol[0]=3; sol[1]=2; sol[2]=1; sol[3]=0; sol[4]=4;
//	sol[5]=5; sol[6]=6; sol[7]=7; sol[8]=8; sol[9]=9;
//	EXPECT_EQ(sol, order);
//
//	order=gs.new_order(sg, gbbs::PICK_MINFL, gbbs::PLACE_FL);						//equivalent to NO SORTING
//	com::stl::print_collection(order);
//
//	sol[0]=0; sol[1]=1; sol[2]=2; sol[3]=3; sol[4]=4;
//	sol[5]=5; sol[6]=6; sol[7]=7; sol[8]=8; sol[9]=9;
//	EXPECT_EQ(sol, order);
//
//	cout<<"-----------------------------------"<<endl;
//}
//
//
//TEST(Graph_sort,new_order_primitives_sg_II){
//	cout<<"Graph_sort::new_order_primitives_sg_II----------------------------------"<<endl;
//	const int SIZE=10;
//	ugraph ug(SIZE);
//	ug.add_edge(0,1);
//	ug.add_edge(1,2);
//	ug.add_edge(2,3);
//	ug.add_edge(0,3);
//	ug.print_data(); 
//	
//	ugraph ug1(ug);
//
//	//sorting with primitives
//	GraphSort<ugraph> gs(ug1);
//	bitarray sg(SIZE); bitarray sgV(SIZE);
//
//	//will only sort G[{0,...,3}]
//	sg.set_bit(0,3);
//	sgV.set_bit(0, SIZE-1);
//
//	vint order=gs.new_order(sg, sgV, gbbs::PICK_MINFL, gbbs::PLACE_LF, true);		//Equivalent to MIN_DEGEN (see sg_I test)				
//	com::stl::print_collection(order);
//
//	vint sol(SIZE);
//	sol[0]=3; sol[1]=2; sol[2]=1; sol[3]=0; sol[4]=4;
//	sol[5]=5; sol[6]=6; sol[7]=7; sol[8]=8; sol[9]=9;
//	EXPECT_EQ(sol, order);
//
//	
//	cout<<"-----------------------------------"<<endl;
//}
//
//
//
//
//
//
//
