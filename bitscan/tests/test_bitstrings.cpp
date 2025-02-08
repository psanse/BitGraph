#include <algorithm>
#include <iterator>
#include <iostream>
#include <set>

#include "bitscan/bitscan.h"				
#include "gtest/gtest.h"

using namespace std;

class BitScanningTest: public ::testing::Test{
protected:
	BitScanningTest():bbn(301), bbi(301), bbs(301){}
	virtual void SetUp(){
	  for(int i = 0; i <= 300; i += 50){
		  bbn.set_bit(i);
		  bbi.set_bit(i);
		  bbs.set_bit(i);
		  sol.insert(i); 
	  }
	}

//////////////////////
//data members
	BitSet bbn;
	BBIntrin bbi;
	BBSentinel bbs;
	set<int> sol;
};



TEST(Bitstrings, setters_and_getters) {
	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_TRUE(bb.is_bit(20));
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_FALSE(bb.is_bit(63));

	//assignment
	BitSet bb1(34);
	bb1.set_bit(22);
	bb1.set_bit(23);
	bb=bb1;

	EXPECT_TRUE(bb.is_bit(22));
	EXPECT_TRUE(bb.is_bit(23));
	EXPECT_EQ(1,bb.number_of_bitblocks());

	//copy constructor
	BitSet bb2(bb);
	EXPECT_TRUE(bb2.is_bit(22));
	EXPECT_TRUE(bb2.is_bit(23));
	EXPECT_EQ(1,bb2.number_of_bitblocks());

}

TEST(Bitstrings, boolean_disjoint){
	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BitSet bb1(130);
	bb1.set_bit(11);
	bb1.set_bit(21);
	bb1.set_bit(65);

	//is_disjoint
	EXPECT_TRUE(bb.is_disjoint(bb1));
	
	bb1.set_bit(64);
	EXPECT_FALSE(bb.is_disjoint(bb1));

	BitSet bb2(130);
	bb2.set_bit(11);				//in common in bb1 and bb2 but not bb
	bb2.set_bit(22);
	bb2.set_bit(66);
	EXPECT_TRUE(bb.is_disjoint(bb1, bb2));

	bb.set_bit(11);
	EXPECT_FALSE(bb.is_disjoint(bb1, bb2));
}

TEST(Bitstrings, set_bit_range){
	BitSet bb(130);
	bb.set_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(0));
	EXPECT_TRUE(bb.is_bit(64));
	
	BitSet bb1(130);
	bb1.set_bit(0, 0);
	EXPECT_TRUE(bb1.is_bit(0));
	
	bb1.set_bit(64, 64);
	EXPECT_TRUE(bb1.is_bit(64));
	EXPECT_TRUE(bb1.is_bit(0));
	
	bb1.set_bit(55, 56);
	EXPECT_TRUE(bb1.popcn64(4, 129));
}

TEST(Bitstrings, erase_bit_range){
	BitSet bb(130);
	bb.set_bit(0, 129);

	bb.erase_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(65));
	EXPECT_FALSE(bb.is_bit(64));
	

	bb.erase_bit(115, 116);
	EXPECT_TRUE(bb.is_bit(114));
	EXPECT_FALSE(bb.is_bit(115));

}


TEST(Bitstrings, algorithms) {
//simple test for algorithms in bbalg.h

	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	////get_first_k_bits
	//BITBOARD b8=bb.get_bitstring()[0];
	//EXPECT_EQ(bblock::popc64_lup(get_first_k_bits(b8, 1)), 1);
	//EXPECT_EQ(bblock::popc64_lup(get_first_k_bits(b8, 2)), 2);
	//EXPECT_EQ(bblock::popc64_lup(get_first_k_bits(b8, 3)), 0);
}

TEST(Bitstrings, population_count){
	
	BBIntrin bbi(130);
	bbi.set_bit(10);
	bbi.set_bit(20);
	bbi.set_bit(64);

	EXPECT_EQ(3, bbi.popcn64());
	EXPECT_EQ(2, bbi.popcn64(11));
	EXPECT_EQ(1, bbi.popcn64(21));
	EXPECT_EQ(0, bbi.popcn64(65));
	EXPECT_EQ(1, bbi.popcn64(64));
}

TEST(Bitstrings, to_vector){
	BitSet bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	vector<int> sol;
	sol.push_back(10);  sol.push_back(20); sol.push_back(64);

	vector<int> vint;
	bb.to_vector(vint);
	EXPECT_EQ(sol, vint);

	//simple sparse bitstring
	BitBoardS bbs(130);
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(64);

	vint.clear();
	bbs.to_vector(vint);
	EXPECT_EQ(sol, vint);

///////////////
// old vector interface (4/7/16)
	int v[130]; std::size_t size=0;
	for(int i=0; i<130; i++){
		v[i]=EMPTY_ELEM;
	}
		
	bb.to_C_array(v, size);
	EXPECT_EQ(3, size);
	copy(v, v+size, vint.begin());
	EXPECT_EQ(sol, vint);

	//BBIntrin redefinition
	BBIntrin bbi(130);
	bbi.set_bit(10);
	bbi.set_bit(20);
	bbi.set_bit(64);
	bbi.to_C_array(v,size, false);
	EXPECT_EQ(3, size);
	copy(v, v+size, vint.begin());
	EXPECT_EQ(3, vint.size());
	EXPECT_EQ(sol, vint);
		
}

TEST_F(BitScanningTest, non_destructive){
	std::set<int> res;
			
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbn.next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);

	res.clear();
	bbi.init_scan(BBObject::NON_DESTRUCTIVE);
	while(true){
		nBit=bbi.next_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);
/////////////////////
	res.clear();
	
	if(bbs.init_scan(BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){	//it is necessary to check if the bitstring is empty
		while(true){
			nBit=bbs.next_bit();
			if(nBit==EMPTY_ELEM) break;
			res.insert(nBit);	
		}
		EXPECT_TRUE(res==sol);
	}
}

TEST_F(BitScanningTest, non_destructive_with_starting_point){
	std::set<int> res;
			
	int nBit=50;
	while(true){
		nBit=bbn.next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_EQ(5, res.size() );
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));

	res.clear();
	bbi.init_scan_from(50,BBObject::NON_DESTRUCTIVE);
	while(true){
		nBit=bbi.next_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_EQ(5, res.size() );
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));
/////////////////////
	res.clear();
	
	bbs.init_scan_from(50, BBObject::NON_DESTRUCTIVE);
		while(true){
			nBit=bbs.next_bit();
			if(nBit==EMPTY_ELEM) break;
			res.insert(nBit);	
		}

	EXPECT_EQ(5, res.size() );
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));
	
}

TEST_F(BitScanningTest, reverse_non_destructive){
	std::set<int> res;
			
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbn.prev_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);

	res.clear();
	bbi.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbi.prev_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);
/////////////////////
	res.clear();
	bbs.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbs.prev_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	// CTG: eliminar salida a pantalla de los test
	//for(set<int>::iterator it=res.begin(); it!=res.end(); ++it){
	//	cout<<*it<<" ";
	//}
	//cout<<"----------------------------------------------"<<endl;

	EXPECT_TRUE(res==sol);
}

TEST_F(BitScanningTest, reverse_non_destructive_with_starting_point){
	std::set<int> res;
			
	int nBit=50;
	while(true){
		nBit=bbn.prev_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_EQ(1, res.size() );
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));

	res.clear();
	bbi.init_scan_from(50,BBObject::NON_DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbi.prev_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_EQ(1, res.size() );
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));
/////////////////////
	res.clear();
	
	bbs.init_scan_from(50, BBObject::NON_DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbs.prev_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}

	EXPECT_EQ(1, res.size() );
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));
	
}

TEST_F(BitScanningTest, destructive){
	std::set<int> res;
	
	BitSet bbn1(bbn);
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbn1.next_bit_if_del(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);
	bbn1.erase_bit(nBit);
	}
	
	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbn1.popcn64());

	//intrinsic
	res.clear();
	BBIntrin bbi1(bbi);
	bbi1.init_scan(BBObject::DESTRUCTIVE);
	while(true){
		nBit=bbi1.next_bit_del();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbi1.popcn64());

	//sentinels
	res.clear();
	BBSentinel bbs1(bbs);
	if(bbs1.init_scan(BBObject::DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=bbs1.next_bit_del();
			if(nBit==EMPTY_ELEM) break;
			res.insert(nBit);	
		}
	}

	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbs1.popcn64());
}


TEST_F(BitScanningTest, reverse_destructive){
	std::set<int> res;
	
	BitSet bbn1(bbn);
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbn1.prev_bit(nBit);		
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);
	bbn1.erase_bit(nBit);
	}
	
	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbn1.popcn64());

	//intrinsic
	res.clear();
	BBIntrin bbi1(bbi);
	bbi1.init_scan(BBObject::DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbi1.prev_bit_del();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbi1.popcn64());

	//sentinels
	res.clear();
	BBSentinel bbs1(bbs);
	if(bbs1.init_scan(BBObject::DESTRUCTIVE_REVERSE) != EMPTY_ELEM){
		while(true){
			nBit = bbs1.prev_bit_del();
			if(nBit == EMPTY_ELEM) break;
			res.insert(nBit);	
		}
	}
	EXPECT_TRUE(res == sol);
	EXPECT_EQ(0,bbs1.popcn64());
}

//TEST_F (BitScanningTest, algorithms){
//	EXPECT_TRUE(similar(bbn, bbi, 0));
//	EXPECT_TRUE(subsumes(bbn, bbi));
//	bbn.erase_bit(bbn.lsbn64());
//	EXPECT_FALSE(subsumes(bbn, bbi));
//	EXPECT_TRUE(subsumes(bbi, bbn));
//}


TEST(BitBoardTests, BitSetTo0) {
	BitSet bb1(25);
	BitSet bb2(bb1);

	bb1.set_bit();			//sets to ONE the whole bitblock
	bb2.set_bit(0,24);

	vector<int> v1(25,0);
	
	for(int i=0;i<25;i++){
		v1[i]=i;
	}

	EXPECT_LT(v1, to_vector(bb1));
	EXPECT_LT(25, bb1.popcn64());
	EXPECT_EQ(to_vector(bb2), v1);
	EXPECT_EQ(25, bb2.popcn64());

}

TEST(BITBOARDTest, GenRandom){
///////////
// deterministic test, unless the seed is changed 
// date: 2/6/14

	//srand(time(NULL));
	vector<double> vd;
	const int NUM_TRIES=100;
	const double DENSITY=.7;
	const double TOLERANCE=.05;

	for(int i=0; i<NUM_TRIES; i++){
		BITBOARD bb=gen_random_bitboard(DENSITY);  //random bits
		vd.push_back(bblock::popc64(bb)/(double)WORD_SIZE);
	}

	//average value
	double sum_of_elems=0;
	for(vector<double>::iterator j=vd.begin();j!=vd.end();j++){
		sum_of_elems += *j;
	}
	double av_of_elems=sum_of_elems/NUM_TRIES;
	EXPECT_TRUE(abs(av_of_elems-DENSITY)<TOLERANCE);
}


class BasicFunctionsTest : public ::testing::Test {
protected:
	virtual void SetUp() {
		int aux[] = {4,8,15,16,23,42};
		val.assign (aux,aux+6);
		bbn.init(45,val);
		bbi.init(45,val);
		bbs.init(45,val);
	}

	vector<int> val;
	BitSet bbn;
	BBIntrin bbi;
	BBSentinel bbs;
};

TEST_F(BasicFunctionsTest, miscellanous){
	EXPECT_EQ(bbn.popcn64(),bbi.popcn64());
	bbs.init_sentinels(true);
	EXPECT_EQ(bbn.popcn64(),bbs.popcn64());
	EXPECT_EQ(to_vector(bbn), to_vector(bbi));
	EXPECT_EQ(to_vector(bbn), to_vector(bbs));
}


TEST(Bitstrings,is_singleton){
///////////////////////
// Determines if there is 0 or 1 bit in a range,
// both included
 
	BitSet bb(130);
	bb.set_bit(62); bb.set_bit(63); bb.set_bit(64);
	EXPECT_EQ(0,bb.is_singleton(10, 20));
	EXPECT_EQ(1,bb.is_singleton(60, 62));
	EXPECT_EQ(-1,bb.is_singleton(62, 64));
	EXPECT_NE(0,bb.is_singleton(64, 65));
	EXPECT_EQ(-1, bb.is_singleton(63, 64));
	EXPECT_EQ(1, bb.is_singleton(64, 65));

	bb.erase_bit(64);
	EXPECT_EQ(0, bb.is_singleton(64, 65));

}

TEST(BitScanning, init_scan_from_specific){
//added specifically because some doubts were creeping during CSP implementation
//

	BBIntrin bbi(100);
	bbi.set_bit(10);
	bbi.set_bit(50);
	bbi.set_bit(64);
	bbi.init_scan_from(10, bbo::NON_DESTRUCTIVE);

	BBIntrin bbres(100);
	while(true){
		int v=bbi.next_bit();
		if(v==EMPTY_ELEM) break;
		bbres.set_bit(v);
	}

	EXPECT_FALSE(bbres.is_bit(10));
	EXPECT_TRUE(bbres.is_bit(50));
	EXPECT_TRUE(bbres.is_bit(64));
	EXPECT_EQ(2,bbres.popcn64());

	//scan from the beginning
	bbres.erase_bit();
	bbi.set_bit(0);
	bbi.init_scan_from(EMPTY_ELEM, bbo::NON_DESTRUCTIVE);  /* note bi.init_scan_from(0, ..) is not the same */
	while(true){
		int v=bbi.next_bit();
		if(v==EMPTY_ELEM) break;
		bbres.set_bit(v);
	}

	EXPECT_EQ(4,bbres.popcn64());
	EXPECT_TRUE(bbres.is_bit(0));
	EXPECT_TRUE(bbres.is_bit(10));

	//incorrect scan from the beginning
	bbres.erase_bit();
	bbi.init_scan_from(0, bbo::NON_DESTRUCTIVE);  /* note bi.init_scan_from(0, ..) is not the same */
	while(true){
		int v=bbi.next_bit();
		if(v==EMPTY_ELEM) break;
		bbres.set_bit(v);
	}

	EXPECT_FALSE(bbres.is_bit(0));
}




