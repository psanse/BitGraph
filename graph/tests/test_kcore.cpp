/**
* @file test_kcore.cpp 
* @brief Unit tests for class Kcore which manages the k-core number of undirected graphs
* @created ?
* @last_update 13/01/24
* @author pss
* 
* TODO - CHECK TEST(KCoreSparse, DISABLED_kcore_decomp_subgraph)  - NOT WORKING IN RELEASE MODE (03/03/25)
**/


#include <iostream>
#include <fstream>
#include <string>
#include "gtest/gtest.h"
#include "graph/algorithms/graph_gen.h"
#include "graph/algorithms/kcore.h"
#include "utils/common.h"
#include "utils/logger.h"
#include "utils/result.h"

using namespace std;

//aliases
using vint = std::vector<int>;

class KcoreWTest : public ::testing::Test {
protected:
	void SetUp() override {
		ug.reset(NV);
		ug.add_edge(0, 1);
		ug.add_edge(0, 2);
		ug.add_edge(0, 3);
		ug.name("star");
	}
	void TearDown() override {}

	//undirected graph instance	
	const int NV = 4;
	ugraph ug;												//undirected graph with integer weights
};

TEST_F(KcoreWTest, constructor) {

	KCore<ugraph> kc(ug);

	EXPECT_EQ(4, kc.get_graph().number_of_vertices());
	EXPECT_TRUE(kc.get_subgraph().is_empty());


}

TEST_F(KcoreWTest, set_subgraph) {

	//bitset that induces a subgraph in G
	decltype(ug)::_bbt bbset(4);
	bbset.set_bit(0);
	bbset.set_bit(2);
	
	//KCore with subgraph
	KCore<ugraph> kc(ug, bbset);
	auto psg = kc.get_subgraph();

	EXPECT_TRUE	(kc.get_subgraph().is_bit(0));
	EXPECT_TRUE	(kc.get_subgraph().is_bit(2));
	EXPECT_FALSE(kc.get_subgraph().is_bit(1));
	EXPECT_FALSE(kc.get_subgraph().is_bit(3));

}

TEST_F(KcoreWTest, set_subgraph_from_vector) {

	//bitset that induces a subgraph in G
	vint vset = { 0, 2, 3 };

	//KCore with subgraph
	KCore<ugraph> kc(ug, vset);
	auto psg = kc.get_subgraph();

	EXPECT_TRUE(kc.get_subgraph().is_bit(0));
	EXPECT_TRUE(kc.get_subgraph().is_bit(2));
	EXPECT_FALSE(kc.get_subgraph().is_bit(1));
	
}

TEST_F(KcoreWTest, kcore_decomp_full_graph) {

	KCore<ugraph> kc(ug);

	/////////////////
	kc.find_kcore();
	/////////////////

	EXPECT_EQ(1, kc.max_core_number());						//1-core is the maximum core number in a star graph

	EXPECT_EQ(1, kc.coreness(0));
	EXPECT_EQ(1, kc.coreness(1));
	EXPECT_EQ(1, kc.coreness(2));
	EXPECT_EQ(1, kc.coreness(3));

	vint kcore_num = kc.coreness_numbers();					//coreness of all vertices
	EXPECT_EQ(1, kcore_num[0]);
	EXPECT_EQ(1, kcore_num[1]);
	EXPECT_EQ(1, kcore_num[2]);
	EXPECT_EQ(1, kcore_num[3]);

	//size of k-cores
	EXPECT_EQ(0, kc.core_size(0));							//number of vertices in the 0-core (isolani in disconnected graphs)
	EXPECT_EQ(4, kc.core_size(1));							//number of vertices in the 1-core (and not in the 2-core)
	EXPECT_EQ(0, kc.core_size(2));							//number of vertices in the 2-core (and not in the 3-core)

	//set of vertices in k-cores
	vint kcore_set = kc.core_set(1);						//vertices in the 1-core
	EXPECT_EQ(4, kcore_set.size());

	kcore_set = kc.core_set(0);								//vertices in the 0-core: all vertices
	EXPECT_EQ(4, kcore_set.size());

	//k-core sorting (new-to-old format)
	vint kcore_ord = kc.kcore_ordering();					//arrangement of vertices in non-decreasing kcore order

	vint kcore_ord_exp = { 1, 2, 3, 0 };					//0 is the last because it was placed in the last bin	
	EXPECT_EQ(kcore_ord_exp, kcore_ord);

	//I/O
	//kc.print_kcore(false, cout);							//prints the coreness of each vertex
	//kc.print_kcore(true, cout);								//prints the coreness and degree of each vertex
}


TEST_F(KcoreWTest, minimum_width) {

	KCore<ugraph> kc(ug);

	//////////////////
	kc.find_kcore();
	//////////////////
	
	auto min_width	= kc.minimum_width	(false);			//1 - minimum width of the graph 
	auto width		= kc.minimum_width	(true);				//3 - a width but not minimum
	
	EXPECT_EQ(min_width, kc.max_core_number());				//minimum width = k in the max k-core 
	EXPECT_NE(width, kc.max_core_number());

	//I/O
	//kc.print_kcore(false, cout);							//prints the coreness of each vertex
	//kc.print_kcore(true, cout);							//prints the coreness and degree of each vertex
}


TEST_F(KcoreWTest, kcore_decomp_static) {

	KCore<ugraph> kc(ug);

	//////////////////
	kc.find_kcore();
	int max_kcore_quad = KCore<ugraph>::find_kcore(ug);
	//////////////////

	EXPECT_EQ(max_kcore_quad, kc.max_core_number());			//minimum width = k in the max k-core 
	

	//I/O
	//kc.print_kcore(false, cout);							//prints the coreness of each vertex
	//kc.print_kcore(true, cout);							//prints the coreness and degree of each vertex
}

TEST(KCore, kcore_decomp_static_brock) {

	ugraph ug(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_1.clq");

	//core number in O(|V|*|V|)
	int max_core_quad = KCore<ugraph>::find_kcore(ug);								
	///////////////////////////////////////////////////

	//core number in O(|E|)
	KCore<ugraph> kc(ug);									
	kc.find_kcore();
	

	///////////////////////////////////////////////////
	EXPECT_EQ(max_core_quad, kc.max_core_number());			 
	///////////////////////////////////////////////////

	//I/O
	//kc.print_kcore(true, cout);								

}

TEST(KCoreSparse, kcore_decomp_full_graph){
	
	//star graph with 11 vertices and a clique {1, 2, 7}
	sparse_ugraph sug(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "star.clq");		
	
	KCore<sparse_ugraph> kc(sug);

	EXPECT_EQ	(11, kc.get_graph().number_of_vertices());
	EXPECT_TRUE	( kc.get_subgraph().is_empty());

	////////////////////
	kc.find_kcore();
	///////////////////

	//1-core = V \ {0, 1, 6}
	EXPECT_EQ(8, kc.core_size(1));

	//2-core = {0, 1, 6}
	vint core2	= kc.core_set(2);
	EXPECT_EQ(1, count(core2.begin(), core2.end(), 0));
	EXPECT_EQ(1, count(core2.begin(), core2.end(), 1));
	EXPECT_EQ(1, count(core2.begin(), core2.end(), 6));
	EXPECT_EQ(0, count(core2.begin(), core2.end(), 7));
	
}

TEST(KCoreSparse, DISABLED_kcore_decomp_subgraph) {

	//star graph with 11 vertices and a clique {0, 1, 6}
	sparse_ugraph sug(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "star.clq");

	//bitset that induces a 3-clique in G
	decltype(sug)::_bbt bbset(sug.number_of_vertices());
	bbset.set_bit(0);
	bbset.set_bit(1);
	bbset.set_bit(6);

	KCore<sparse_ugraph> kc(sug, bbset);

	EXPECT_EQ(11, kc.get_graph().number_of_vertices());
	EXPECT_TRUE(kc.get_subgraph().is_bit(0));
	EXPECT_TRUE(kc.get_subgraph().is_bit(1));
	EXPECT_TRUE(kc.get_subgraph().is_bit(6));
	EXPECT_EQ(3, kc.get_subgraph().size());

	//////////////////////
	kc.find_kcore(true);											//on G[{0, 1, 6}]			
	/////////////////////

	//1-core = V \ {0, 1, 6}
	EXPECT_EQ(0, kc.core_size(1));


	//2-core = {0, 1, 6}
	vint core2 = kc.core_set(2);
	EXPECT_EQ(3, kc.core_size(2));
	EXPECT_EQ(1, count(core2.begin(), core2.end(), 0));
	EXPECT_EQ(1, count(core2.begin(), core2.end(), 1));
	EXPECT_EQ(1, count(core2.begin(), core2.end(), 6));
	EXPECT_EQ(0, count(core2.begin(), core2.end(), 7));

	//I/O
	//kc.print_kcore(false, cout);								//prints the coreness of each vertex
	//kc.print_kcore(true, cout);								//prints the coreness and degree of each vertex

}

/////////////////
//
// Examples with synthetic graphs
// 
// TODO - CHECK
//
/////////////////

TEST(KCoreUB, kcore_example){

	ugraph ug(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "brock200_1.clq");
		
	//test with real kcore number
	KCore<ugraph> kc(ug);	
	kc.find_kcore();
	int kcn=kc.max_core_number();
	int UB_corr=kc.find_kcore_UB(kcn);
	int kcUBn=kc.max_core_number();
	EXPECT_EQ(UB_corr, kcUBn);
	EXPECT_EQ(kcUBn, kc.minimum_width(true));   //checks width (real degrees)
	

	//test with UB on kcore number
	KCore<ugraph> kc1(ug);
	kc1.find_kcore();
	kcn = kc1.max_core_number();
	UB_corr=kc1.find_kcore_UB(kcn);
	kcUBn=kc1.max_core_number();

	EXPECT_EQ(UB_corr, kcUBn);	
	EXPECT_EQ(kcUBn, kc.minimum_width(true)); //checks width (real degrees)

	
	//test with UB on kcore number
	KCore<ugraph> kc2(ug);
	kc2.find_kcore();
	kcn=kc2.max_core_number();
	UB_corr=kc2.find_kcore_UB(kcn);
	kcUBn=kc2.max_core_number();

	EXPECT_EQ(UB_corr, kcUBn);	
	EXPECT_EQ(kcUBn, kc.minimum_width(true)); //checks width (real degrees)
}


TEST(KCoreUB, kcore_example_II){

	ugraph ug(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "r10_0.2_23.txt");
	
	//test with real kcore number
	KCore<ugraph> kc(ug);
	kc.find_kcore();
	int kcn=kc.max_core_number();
	int UB_corr=kc.find_kcore_UB(kcn);
	int kcUBn=kc.max_core_number();

	EXPECT_EQ(UB_corr, kcUBn);
	EXPECT_EQ(kcUBn, kc.minimum_width(true)); //checks width (real degrees)
}

///////////////
//
// DISABLED TESTS
//
////////////////


TEST(KCoreUB, DISABLED_kcore_example_I) {

	ugraph ug(PATH_GRAPH_TESTS_CMAKE_SRC_CODE "ia-southernwomen.edges");

	//test with real kcore number
	KCore<ugraph> kc(ug);
	kc.find_kcore();
	int kcn = kc.max_core_number();
	int UB_corr = kc.find_kcore_UB(kcn);
	int kcUBn = kc.max_core_number();

	EXPECT_EQ(UB_corr, kcUBn);
	EXPECT_EQ(kcUBn, kc.minimum_width(true)); //checks width (real degrees)

}

TEST(KCoreUB, DISABLED_random){
	random_attr_t rd(10, 100, .05, .20, 50, 10, .05);
	const BOOL store_graph=true;
	char PATH[250]="C:\\Users\\pablo\\Desktop\\random_tests\\";
	stringstream sstr("");
	string name("");
	
	for(int tam=rd.nLB ; tam<=rd.nUB; tam+=rd.incN){
		for(double den=rd.pLB ; den<=rd.pUB; den+=rd.incP){
			for(int rep=0; rep<rd.nRep; rep++){
				ugraph ug;
				RandomGen<ugraph>::create_graph(ug, tam,den);

				//write the graph
				name.clear();
				name="r";
				sstr.str(""); sstr.clear();
				sstr<<tam<<"_"<<den<<"_"<<rep;
				name+=sstr.str();
				ug.name(name+".txt");		//change name appropiately
				
				//name stamp
				sstr.str(""); sstr.clear();	
				sstr << ug.name();
				
				//store instance if desired
				//if(store_graph){
				//	if(comclq::graph_to_dimacs(ug, PATH+ug.name())==-1){
				//		LOG_WARNING("random_test: could not store generated instance");
				//	}
				//}
			
				KCore<ugraph> kc(ug);
				kc.find_kcore();
				int kcn=kc.max_core_number();

				KCore<ugraph> kcUB(ug);
				int UB_corr= kcUB.find_kcore_UB(kcn);
				int kcUBn=kcUB.max_core_number();
				EXPECT_EQ(UB_corr, kcUBn);
				EXPECT_EQ(kcUBn, kcUB.minimum_width(true));		//checks width (real degrees)
				
				
				if(kcn!=kcUBn){
					sstr<<":"<<"kcore: "<<kc.max_core_number()<<" kcore_UB: "<<kcUB.max_core_number();
					LOGG_INFO(sstr.str());
				}
	
			}
		}
	}
}

/////////////////////
//
// COMMENTED OUT TESTS - DEPRECATED
//
// TODO: Update tests
/////////////////////

//TEST(DISABLE_KCoreOrdering, kcore_example){
/////////////////
//// Testing new fast orderings (30/10/17)
//
//	LOG_INFO("KCoreOrdering::kcore_example()-----------------");
//	
//	string filename(TEST_PATH_BIG_GRAPHS);
//	//filename+=("brock200_1.clq");
//	filename+="cond-mat-2005.clq";
//	//filename+="soc-Slashdot0811_u.edges.clq";
//
//	ugraph ug(filename);
//	ug.print_data();
//	const int NV=ug.number_of_vertices()-1;
//	KCore<ugraph> kc(ug);	
//	kc.kcore();
//	vint lkc=kc.get_kcore_numbers();
//
//	Result r;
//	r.tic();
//
//	//sorting according to non-decreasing coreness
//	GraphSort<ugraph> gs(ug);
//	vint ord=kc.get_kcore_ordering();
//	reverse(ord.begin(),ord.end());
//	Decode::reverse_in_place(ord);	
//		
//	ugraph ugn(1);
//	Decode d;
//	gs.reorder_edge_based(ord,ugn,d,nullptr);								/*slightly faster than reordering in place (much faster for big graphs) */
//	//gs.reorder(ord,gn,d,nullptr);									
//	//gs.reorder_edge_based(ord,d,nullptr);
//	//gs.reorder_edge_based(ord,nullptr);								
//	
//	r.toc();
//	LOGG_INFO("kc:[" , r.get_user_time() , "]");
//
//	ugn.print_data();
//	EXPECT_EQ(ugn.number_of_edges(), ug.number_of_edges());
//
//
//	//test
//	LOGG_INFO("deg(first):" , ug.degree(0), " deg(last):" , ug.degree(NV-1));
//	EXPECT_GT(ug.degree(0), ug.degree(NV-1));
//
//	//test-coreness
//	KCore<ugraph> kc_ord(ugn);
//	kc_ord.kcore();
//	LOGG_INFO("core(first):" , kc.coreness(0) , " deg(last):" , kc.coreness(NV-1));
//	EXPECT_GT(kc.coreness(0), kc.coreness(NV-1));
//
//	//test-corenesses of the two graphs
//	vint lkc_ord=kc_ord.get_kcore_numbers();
//	//sort(lkc.begin(), lkc.end(), greater_equal<int>());			/* is also possible */
//	vint new_lkc(lkc.size());
//	for(int i=0; i<lkc.size(); i++){
//		new_lkc[ord[i]]=lkc[i];
//	}
//	EXPECT_EQ(new_lkc, lkc_ord);
//	//com::stl::print_collection<vint>(lkc,cout,true);
//		
//	cin.get();
//	LOG_INFO("-----------------------------------------------");
//}


//TEST(KCore, kcore_example){
//	ugraph ug("brock200_1.clq");
//			
//	KCore<ugraph> kc(ug);
//	kc.kcore();
//	EXPECT_EQ(134, kc.get_kcore_number());
//}
//
//TEST(KCore, kcore_example_I){
//	ugraph ug("ia-southernwomen.edges");
//			
//	KCore<ugraph> kc(ug);
//	kc.kcore();
//	EXPECT_EQ(6, kc.get_kcore_number());
//}
//
//TEST(KCore, kcore_clq){
//	ugraph ug("brock200_1.clq");
//		
//	KCore<ugraph> kc(ug);
//	kc.kcore();
//	com::stl::print_collection(kc.find_heur_clique());
//	com::stl::print_collection(kc.find_heur_clique(10));
//}
//
//
//TEST(KCore, kcore_example_sparse){
//	sparse_ugraph ug("brock200_1.clq");
//
//	KCore<sparse_ugraph> kc(ug);
//	kc.kcore();
//	EXPECT_EQ(134, kc.get_kcore_number());
//
//	sparse_bitarray bbs(ug.number_of_vertices());
//	bbs.set_bit(1,2);
//	EXPECT_TRUE(ug.is_edge(1,2));
//	KCore<sparse_ugraph> kc1(ug, &bbs);
//	kc1.kcore();
//	EXPECT_EQ(1, kc1.get_kcore_number());
//	kc1.print_kcore();
//	cout<<"-----------------------------------------"<<endl;
//}
//
//TEST(KCore, change_subgraph){
//	sparse_ugraph ug("brock200_1.clq");
//
//	KCore<sparse_ugraph> kc(ug);
//	kc.kcore();
//	EXPECT_EQ(134, kc.get_kcore_number());
//
//	//change subgraph to analyse
//	sparse_bitarray bbs(ug.number_of_vertices());
//	bbs.set_bit(1,2);
//	EXPECT_TRUE(ug.is_edge(1,2));
//	kc.set_subgraph(&bbs);
//	kc.kcore();
//	EXPECT_EQ(1, kc.get_kcore_number());
//	kc.print_kcore();
//
//	//change to full graph again
//	bbs.set_bit(0, ug.number_of_vertices()-1);
//	kc.set_subgraph(&bbs);
//	kc.kcore();
//	EXPECT_EQ(134, kc.get_kcore_number());
//	
//	cout<<"-----------------------------------------"<<endl;
//}
//
//TEST(KCore, kcore_decomposition){
//	ugraph ug("star.clq");			//0, 1, 6 form a 3-clique
//	
//	KCore<ugraph> kc(ug);
//	kc.kcore();
//	vector<int> v= kc.get_kcore_numbers(2);
//	
//	EXPECT_EQ(1, count(v.begin(), v.end(), 0));
//	EXPECT_EQ(1, count(v.begin(), v.end(), 1));
//	EXPECT_EQ(1, count(v.begin(), v.end(), 6));
//	EXPECT_EQ(0, count(v.begin(), v.end(), 7));
//
//	////////////
//	v.clear();
//	ugraph ug1(100);							//an empty graph
//	ug1.add_edge(0,1);
//	ug1.add_edge(0,2);
//	ug1.add_edge(0,3);
//	ug1.add_edge(0,4);
//	ug1.add_edge(0,5);
//
//	KCore<ugraph> kc1(ug1);
//	kc1.kcore();
//	
//	v= kc1.get_kcore_numbers(1);
//	for(int i=0; i<6; i++)
//		EXPECT_EQ(i, v[i]);
//}
//
//TEST(KCore, filter_simple){
//	ugraph ug("star.clq");			//0, 1, 6 form a 3-clique
//	map<int,int> filter;
//	
//	KCore<ugraph> kc(ug);
//	kc.kcore();
//	
//	//standard use for max clique (vertices will be ordered in this way)
//	kc.make_kcore_filter(filter);	
//	EXPECT_EQ(3,filter[2]);
//	EXPECT_EQ(1,filter.size());
//
//	//direct order: assume vertices are order by increasing kcore number
//	kc.make_kcore_filter(filter,false);
//	EXPECT_EQ(8,filter[1]);
//	EXPECT_EQ(1,filter.size());
//			
//	//I/O
//	/*cout<<kc<<endl;
//	for(map_it it=filter.begin(); it!=filter.end(); ++it){
//		cout<<"["<<it->first<<","<<it->second<<"]"<<endl;
//	}*/
//	
//}
//
//TEST(KCore, filter_non_kcore_vakues){
//	ugraph ug("auxfilerandomTEST.txt");			//kcores are (v:k(v)) 4:0, 0:1, rest have k(v)=4
//	map<int,int> filter;
//	
//	KCore<ugraph> kc(ug);
//	kc.kcore();
//	
//	//standard use for max clique (vertices will be ordered in this way)
//	kc.make_kcore_filter(filter, true);	
//	EXPECT_EQ(9,filter[2]);
//	EXPECT_EQ(8,filter[3]);				//3 is not a core order
//	EXPECT_EQ(8,filter[4]);
//}
//
//

//
//TEST(Ugraph_sparse, find_clique){
//	
//	const int NV_INF=100, NV_SUP=1000, INC_SIZE=100, REP_MAX=10;
//    const double DEN_INF=.1,DEN_SUP=.9, INC_DENSITY=.1;
//
//    for(int N=NV_INF; N<NV_SUP; N+=INC_SIZE)  {
//        for(double P=DEN_INF; P<DEN_SUP;P+=INC_DENSITY){
//            for(int rep=0;rep<REP_MAX;rep++){
//				 cout<<"--------------------------------------------------------"<<endl;
//				sparse_ugraph ug;
//				SparseRandomGen<>().create_ugraph(ug,N,P);
//				KCore<sparse_ugraph> kc(ug);
//				kc.kcore();
//				
//				vector<int> v1=kc.find_heur_clique();
//				vector<int> v2=kc.find_heur_clique_opt();
//				vector<int> v3=kc.find_heur_clique_sparse();
//				com::stl::print_collection(v1);
//				com::stl::print_collection(v2);
//				com::stl::print_collection(v3);
//
//				//determine clique for opt
//				for(int i=0; i<v2.size()-1; i++){
//					for(int j=i+1; j<v2.size(); j++){
//						if(!ug.is_edge(v2[i],v2[j])){
//							cerr<<"incorrect clique"<<endl;
//						}
//						ASSERT_TRUE(ug.is_edge(v2[i],v2[j]));
//					}
//				}
//
//				//determine clique for opt
//				for(int i=0; i<v3.size()-1; i++){
//					for(int j=i+1; j<v3.size(); j++){
//						if(!ug.is_edge(v3[i],v3[j])){
//							cerr<<"incorrect clique"<<endl;
//						}
//						ASSERT_TRUE(ug.is_edge(v3[i],v3[j]));
//					}
//				}
//																
//				cout<<"[N:"<<N<<" p:"<<P<<" r:"<<rep<<"]"<<endl;
//				ASSERT_EQ(v1.size(),v2.size());
//				//ASSERT_EQ(v1.size(),v3.size());
//			}
//		}
//	}
//}

////
////TEST(KCore, find_clique_I){
////	sparse_ugraph ug("brock200_1.clq");
////		
////	KCore<sparse_ugraph> kc(ug);
////	kc.kcore();
////	vector<int> v1=kc.find_clique_lb();
////	vector<int> v2=kc.find_clique_lb_opt();
////	vector<int> v3=kc.find_clique_lb_opt_B();
////	EXPECT_EQ(v1.size(), v2.size());
////	EXPECT_EQ(v1.size(), v3.size());
////	printCol(v1);
////	printCol(v2);
////	printCol(v3);
////}
////
////TEST(KCore, find_clique_II){
////	sparse_ugraph ug("failed_lb.clq");
////		
////	KCore<sparse_ugraph> kc(ug);
////	kc.kcore();
////	vector<int> v1=kc.find_clique_lb();
////	vector<int> v2=kc.find_clique_lb_opt();
////	vector<int> v3=kc.find_clique_lb_opt_B();
////
////	//determine clique for opt
////	for(int i=0; i<v2.size()-1; i++){
////		for(int j=i+1; j<v2.size(); j++){
////			ASSERT_TRUE(ug.is_edge(v2[i],v2[j]));
////		}
////	}
////
////	//determine clique for opt_B
////	for(int i=0; i<v3.size()-1; i++){
////		for(int j=i+1; j<v3.size(); j++){
////			ASSERT_TRUE(ug.is_edge(v3[i],v3[j]));
////		}
////	}
////
////
////}














