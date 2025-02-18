/**
* @file test_sparse.cpp
* @brief Unit tests for sparse classes
* @details created  ?, last_update 12/02/2025
* @author pss
* 
* TODO - refactor (15/02/2025), separate possible sparse scanning tests from the rest
* 
**/

#include "bitscan/bbscan_sparse.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

class BitSetSpClassTest : public ::testing::Test {
protected:
	BitSetSpClassTest() :bbsp(301) {}
	virtual void SetUp() {
		for (int i = 0; i <= 300; i += 50) {
			bbsp.set_bit(i);
			pSet.push_back(i);
		}
	}

	//////////////////////
	//data members
	BitSetSp bbsp;	
	vector<int> pSet;
};

TEST(Sparse, construction_basic){

	BitSetSp bbsp;
	bbsp.init(1000);

	bbsp.set_bit(500);
	bbsp.set_bit(700);
	bbsp.set_bit(900);
	
	//////////////////////
	int retVal=bbsp.set_bit(1100);							//outside popsize range, not added
	EXPECT_EQ(-1, retVal);
	//////////////////////

	EXPECT_EQ(3, bbsp.popcn64());							//bbsp = {500, 700, 900}

	bbsp.init(100000);
	bbsp.set_bit(10);
	EXPECT_TRUE(bbsp.is_bit(10));

}

TEST(Sparse, construction_from_vector) {

	vector<int> lv = { 100, 200, 300, 400, 500 };
	BitSetSp bbsp(1000, lv);

	EXPECT_TRUE(bbsp.is_bit(100));
	EXPECT_TRUE(bbsp.is_bit(200));
	EXPECT_TRUE(bbsp.is_bit(300));
	EXPECT_TRUE(bbsp.is_bit(400));
	EXPECT_TRUE(bbsp.is_bit(500));
	EXPECT_EQ(5, bbsp.size());
	EXPECT_EQ(5, bbsp.number_of_blocks());			//sparse number of blocks, one per element in this case 

}

TEST(Sparse, DISABLED_construction_semantics) {

	//TODO
}


TEST_F(BitSetSpClassTest, reset) {

	//size
	bbsp.reset(10);
	EXPECT_EQ(0, bbsp.size());							//number of 1-bis
	EXPECT_EQ(0, bbsp.capacity());						//number of non_zero bitblocks

	//size and bits
	vint lv = { 10, 20, 100 };
	bbsp.reset(200, lv);
	EXPECT_EQ(3, bbsp.size());							//number of 1-bis
	EXPECT_EQ(2, bbsp.capacity());						//number of non_zero bitblocks
	EXPECT_TRUE(bbsp.is_bit(10));						//number of 1-bis		
	EXPECT_TRUE(bbsp.is_bit(20));						//number of 1-bis		
	EXPECT_TRUE(bbsp.is_bit(100));						//number of 1-bis		

}

TEST(Sparse, basics) {

	BitSetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(20));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_FALSE(bbsp.is_bit(63));
	EXPECT_EQ(3,bbsp.popcn64());

	bbsp.erase_bit(10);
	EXPECT_FALSE(bbsp.is_bit(10));
	EXPECT_EQ(2,bbsp.popcn64());

	//find functions for blocks (not that the actual bit in WDIV(nBit) is not important, it is the bitblock index that has to exist)
	EXPECT_EQ(0, bbsp.find_block_pos(WDIV(10)).second);
	EXPECT_TRUE(bbsp.find_block_pos(WDIV(10)).first);
	EXPECT_EQ(1, bbsp.find_block_pos(WDIV(64)).second);
	EXPECT_TRUE(bbsp.find_block_pos(WDIV(64)).second);
	
	//test for a block which does not exist
	EXPECT_FALSE(bbsp.find_block_pos(WDIV(129)).first);
	EXPECT_EQ(EMPTY_ELEM, bbsp.find_block_pos(WDIV(129)).second);

	EXPECT_EQ(0x01, bbsp.find_block(WDIV(64)));
}

TEST(Sparse, member_masks) {

	BitSetSp lhs;
	lhs.init(200);
	lhs.set_bit(63);
	lhs.set_bit(126);
	lhs.set_bit(189);	
	
	BitSetSp rhs;
	rhs.init(200);
	rhs.set_bit(63);
	rhs.set_bit(127);
	
	lhs.AND_EQ(1,rhs);
	EXPECT_TRUE(lhs.popcn64()==1);
	EXPECT_TRUE(lhs.is_bit(63));

	lhs.OR_EQ(1,rhs);
	EXPECT_TRUE(lhs.is_bit(127));
	EXPECT_EQ(2, lhs.popcn64());
} 

TEST(Sparse_intrinsic, basics_2) {

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	
	(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(20));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_FALSE(bbsp.is_bit(63));
	EXPECT_EQ(3,bbsp.popcn64());

	bbsp.erase_bit(10);
	EXPECT_FALSE(bbsp.is_bit(10));
	EXPECT_EQ(2,bbsp.popcn64());


	//assignment
	BBScanSp bbsp1(34);
	bbsp1.set_bit(22);
	bbsp1.set_bit(23);
	bbsp=bbsp1;

	EXPECT_TRUE(bbsp1.is_bit(22));
	EXPECT_TRUE(bbsp1.is_bit(23));
	EXPECT_EQ(1,bbsp1.number_of_blocks());

	//copy constructor
	BBScanSp bbsp2(bbsp);
	EXPECT_TRUE(bbsp2.is_bit(22));
	EXPECT_TRUE(bbsp2.is_bit(23));
	EXPECT_EQ(1,bbsp2.number_of_blocks());

}

TEST(Sparse_intrinsic, set_clear_bit_in_intervals) {

	BBScanSp bba(1000000);
	bba.set_bit(1200,1230);

	EXPECT_EQ(31, bba.popcn64());		
	EXPECT_EQ(1200, bba.lsbn64());
	EXPECT_EQ(1230, bba.msbn64());

	bba.clear_bit(1201, 1230);
	bba.shrink_to_fit();				//effectively deallocates space
	EXPECT_EQ(1, bba.popcn64());
	EXPECT_TRUE(bba.is_bit(1200));
}

TEST(Sparse, population_count){

	BitSetSp bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_EQ(3, bb.popcn64());
	EXPECT_EQ(2, bb.popcn64(11));
	EXPECT_EQ(1, bb.popcn64(21));
	EXPECT_EQ(0, bb.popcn64(65));
	EXPECT_EQ(1, bb.popcn64(64));

	BBScanSp bbs(130);
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(64);

	EXPECT_EQ(3, bbs.popcn64());
	EXPECT_EQ(2, bbs.popcn64(11));
	EXPECT_EQ(1, bbs.popcn64(21));
	EXPECT_EQ(0, bbs.popcn64(65));
	EXPECT_EQ(1, bbs.popcn64(64));

	bbs.set_bit(129);						//the last possible bit in the population
	EXPECT_EQ(1, bbs.popcn64(129));
	EXPECT_EQ(0, bbs.popcn64(130));			//at the moment there is no population check

	//empty graphs
	BitSetSp bbs_empty(130);
	EXPECT_EQ(0, bbs_empty.popcn64(5));	

	BBScanSp bbs_empty1(130);
	EXPECT_EQ(0, bbs_empty1.popcn64(5));	
}

TEST(Sparse, set_bits) {
	
	BitSetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	EXPECT_TRUE(bbsp.is_bit(126));

	//range
	bbsp.set_bit(3, 27);
	cout<<bbsp<<endl;
	EXPECT_TRUE(bbsp.is_bit(3));
	EXPECT_TRUE(bbsp.is_bit(27));
	EXPECT_EQ(32, bbsp.popcn64());
	

	bbsp.set_bit(1002,1002);
	EXPECT_TRUE(bbsp.is_bit(1002));
	EXPECT_EQ(33, bbsp.popcn64());


	//range
	bbsp.set_bit(29, 125);
	EXPECT_FALSE(bbsp.is_bit(28));
	EXPECT_TRUE(bbsp.is_bit(29));
	EXPECT_TRUE(bbsp.is_bit(125));

	bbsp.set_bit(1001,1100);
	EXPECT_FALSE(bbsp.is_bit(999));
	EXPECT_TRUE(bbsp.is_bit(1001));
	EXPECT_TRUE(bbsp.is_bit(1100));
	EXPECT_FALSE(bbsp.is_bit(1101));
	cout<<bbsp<<endl;


//init member functions
	BBScanSp sb(150);
	sb.init_bit(30,40);
	EXPECT_TRUE(sb.is_bit(30));
	EXPECT_TRUE(sb.is_bit(40));
	EXPECT_EQ(11, sb.popcn64());

	sb.init_bit(55);
	EXPECT_TRUE(sb.is_bit(55));
	EXPECT_EQ(1, sb.popcn64());

}

TEST(Sparse, boolean_properties){

	BBScanSp bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BBScanSp bb1(130);
	bb1.set_bit(11);
	bb1.set_bit(21);
	bb1.set_bit(65);

	//is_disjoint
	EXPECT_TRUE(bb.is_disjoint(bb1));
	
	bb1.set_bit(64);
	EXPECT_FALSE(bb.is_disjoint(bb1));

	//is disjoint with ranges
	EXPECT_TRUE(bb.is_disjoint(0,0,bb1));
	EXPECT_FALSE(bb.is_disjoint(0,1,bb1));
	EXPECT_FALSE(bb.is_disjoint(1,2,bb1));
	EXPECT_FALSE(bb.is_disjoint(1,7,bb1));
	
	//does not produce an out of range error
	bb1.erase_bit(64);
	EXPECT_TRUE(bb.is_disjoint(1,7,bb1));			
	EXPECT_TRUE(bb.is_disjoint(3,7,bb1));


}

TEST(Sparse, set_blocks) {
	
	BitSetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	BitSetSp bbsp1(10000);

	bbsp1.set_bit(5);
	bbsp1.set_bit(8);
	bbsp1.set_bit(129);
	bbsp1.set_bit(5000);

	//range
	bbsp.set_block(2,2, bbsp1);
	EXPECT_TRUE(bbsp.is_bit(129));
	EXPECT_FALSE(bbsp.is_bit(5000));


	//range
	bbsp.set_block(2,INDEX_1TO0(5000), bbsp1);
	EXPECT_TRUE(bbsp.is_bit(5000));

	bbsp.erase_block(0, bbsp1);
	EXPECT_EQ(7, bbsp.popcn64());

	//range
	bbsp.set_block(2,bbsp1);
	EXPECT_TRUE(bbsp.is_bit(129));
	EXPECT_TRUE(bbsp.is_bit(5000));
}

TEST(Sparse, erase_bits) {
	
	BitSetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);


	bbsp.erase_bit(126);
	EXPECT_FALSE(bbsp.is_bit(126));

	//range
	bbsp.erase_bit(2, 1000);
	EXPECT_FALSE(bbsp.is_bit(2));
	EXPECT_FALSE(bbsp.is_bit(1000));
	EXPECT_EQ(3, bbsp.popcn64());
	EXPECT_EQ(4, bbsp.number_of_blocks());		//all bitblocks are still there (with 0 value)
	

	//range
	bbsp.erase_bit(1,1);
	EXPECT_FALSE(bbsp.is_bit(1));
	EXPECT_EQ(2, bbsp.popcn64());

	//erase in a sequential loop
	int nBit=0;
	BBScanSp::vPB_it it=bbsp.begin();
	while(true){
		it=bbsp.erase_bit(nBit++,it);
		if(nBit>1000) break;
	}
	EXPECT_TRUE(bbsp.is_bit(9000));
	EXPECT_EQ(1,bbsp.popcn64());
	
	//range trying to erase blocks in higher positions than any existing
	bbsp.reset(10000);
	bbsp.set_bit(55, 58);		
	bbsp.erase_bit(77, 100);		//no bits in index>=2 so nothing is done
	EXPECT_FALSE(bbsp.is_empty());
	
	//last bit in range does not correspond to a block but there are blocks with higher index
	bbsp.reset(10000);
	bbsp.set_bit(1, 5);	
	bbsp.set_bit(200,205);	
	bbsp.erase_bit(67, 69);	
	bbsp.print();
	EXPECT_EQ(11, bbsp.popcn64());
	EXPECT_TRUE(bbsp.is_bit(200));

	//first bit in range does not correspond to a block
	bbsp.reset(10000);
	bbsp.set_bit(70, 75);	
	bbsp.erase_bit(63, 70);	
	EXPECT_EQ(5,bbsp.popcn64());
	EXPECT_FALSE(bbsp.is_bit(70));
	EXPECT_TRUE(bbsp.is_bit(71));
}

TEST(Sparse, clear_bits) {

	BitSetSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	
	//clear bits removing bitblocks in range
	bbsp.clear_bit(2, 1000);
	EXPECT_FALSE(bbsp.is_bit(2));
	EXPECT_FALSE(bbsp.is_bit(1000));
	EXPECT_EQ(3, bbsp.popcn64());
	EXPECT_EQ(3,bbsp.number_of_blocks());		//first and 9000 bitblocks are there + 1000 bitblock set to 0

	bbsp.clear_bit(2, 1064);
	EXPECT_EQ(2,bbsp.number_of_blocks());		//removes 1000 bitblock

	//range
	bbsp.clear_bit(1,1);
	EXPECT_FALSE(bbsp.is_bit(1));
	EXPECT_EQ(2, bbsp.popcn64());
	EXPECT_EQ(2,bbsp.number_of_blocks());


//end slices
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	bbsp.clear_bit(1001,EMPTY_ELEM);
	EXPECT_TRUE(bbsp.is_bit(1000));
	EXPECT_EQ(3,bbsp.number_of_blocks());

	bbsp.clear_bit(EMPTY_ELEM, 1);
	EXPECT_TRUE(bbsp.is_bit(2));
	EXPECT_EQ(3,bbsp.number_of_blocks());

	bbsp.clear_bit(EMPTY_ELEM, EMPTY_ELEM);
	EXPECT_TRUE(bbsp.is_empty());

}

TEST(Sparse_non_instrinsic, scanning){
	BitSetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	//direct loop
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbsp.next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		v.push_back(nBit);
	}
		
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);
	EXPECT_EQ(bbsp.number_of_blocks(), 2);				//number of blocks 
	bbsp.erase_bit(64);
	EXPECT_EQ(bbsp.number_of_blocks(), 2);				//one of the blocks is empty, so still 2
	EXPECT_FALSE(bbsp.is_bit(64));

	//reverse loop
	bbsp.set_bit(64);
	v.clear();

	nBit=EMPTY_ELEM;
	while(true){
		nBit=bbsp.prev_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		v.push_back(nBit);
	}

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
}


TEST(Sparse_intrinsic, non_destructive_scanning){

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	//non destructive
	int nBit;
	if(bbsp.init_scan(BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.next_bit();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}
		
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);
	EXPECT_EQ(bbsp.number_of_blocks(), 2);				//number of blocks 
	bbsp.erase_bit(64);
	EXPECT_EQ(bbsp.number_of_blocks(), 2);				//one of the blocks is empty, so still 2
	EXPECT_FALSE(bbsp.is_bit(64));
	EXPECT_EQ(2, bbsp.popcn64());

	//non destructive reverse
	bbsp.set_bit(64);
	v.clear();

	if(bbsp.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.prev_bit();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
}

TEST(Sparse_intrinsic, non_destructive_scanning_with_starting_point){

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	int nBit;
	EXPECT_EQ(2,bbsp.number_of_blocks());				//number of blocks 

	//non destructive
	if(bbsp.init_scan_from(20,BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.next_bit();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}
		

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(1, v.size());
	
	
	//non destructive reverse
	v.clear();
	if(bbsp.init_scan_from(20,BBObject::NON_DESTRUCTIVE_REVERSE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.prev_bit();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}
		
	
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(1, v.size());
}

TEST(Sparse_intrinsic, destructive_scanning){

	BBScanSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	//non destructive
	int nBit;
	if(bbsp.init_scan(BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.next_bit_del();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}
		
	EXPECT_TRUE(bbsp.is_empty());
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);

	//non destructive reverse
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	v.clear();
	if(bbsp.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.prev_bit_del();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}

	EXPECT_TRUE(bbsp.is_empty());

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
}

TEST(Sparse, operators){

	BitSetSp bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	BitSetSp bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(128);
	
	//OR
	BitSetSp resOR(130);
	OR(bbsp1, bbsp, resOR);
	EXPECT_EQ(6, resOR.popcn64());

	//=
	bbsp=resOR;
	EXPECT_EQ(6, bbsp.popcn64());
	EXPECT_TRUE(bbsp==resOR);

	//AND
	BitSetSp bbsp2(130,true);
	bbsp2.set_bit(54);
	BitSetSp resAND(130);
	AND(bbsp1,bbsp2,resAND);
	EXPECT_TRUE(resAND.is_bit(54));
	EXPECT_EQ(1, resAND.popcn64());
}

TEST(Sparse, insertion) {

	BitSetSp bbsp(130);					
	bbsp.set_bit(64);
	bbsp.set_bit(65);
	bbsp.set_bit(12);					//obligatory

	EXPECT_TRUE(bbsp.is_bit(12));
}

TEST(Sparse, integration) {

	BitSetSp bbs(10000);
	bbs.set_bit(10);
	bbs.set_bit(1000);
	bbs.set_bit(70);
	bbs.set_bit(150);
	
	BitSetSp bbs1(10000);
	bbs1.set_bit(1000);

	BitSetSp bbs2(10000);
	AND(bbs1, bbs, bbs2);
	EXPECT_EQ(1, bbs2.popcn64());
	EXPECT_TRUE(bbs2.is_bit(1000));
	EXPECT_TRUE(bbs2==bbs1);

	BitSetSp bbs3(10000);
	OR(bbs1,bbs, bbs3);
	EXPECT_EQ(4, bbs3.popcn64());
	EXPECT_TRUE(bbs3.is_bit(1000));
	EXPECT_TRUE(bbs3.is_bit(10));
	EXPECT_TRUE(bbs3.is_bit(70));
	EXPECT_TRUE(bbs3.is_bit(150));

	bbs3.erase_bit(1000);
	EXPECT_EQ(3, bbs3.popcn64());			//even though one bit block is empty
	EXPECT_EQ(4, bbs3.number_of_blocks());		
}

TEST(Sparse, next_bit_del_pos) {

	BBScanSp bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	//bitscannning loop
	bbsp.init_scan(BBObject::DESTRUCTIVE);
	int posBB=EMPTY_ELEM;
	while(true){
		int	nBit=bbsp.next_bit_del_pos(posBB);
		if(nBit==EMPTY_ELEM) 
				break;
		cout<<"nBit: "<<nBit<<" pos: "<<posBB<<endl;
		
	}
	
}

TEST(Sparse, erase_block_pos) {

	BBScanSp bbsp(10000);
	BBScanSp bberase(10000);
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	
	bberase.set_bit(0);
	bberase.set_bit(1);
	bberase.set_bit(2);
	bberase.set_bit(126);
	bberase.set_bit(127);
	bberase.set_bit(1000);
	bberase.set_bit(9000);

	bbsp.erase_block_pos(1,bberase);
	EXPECT_FALSE(bbsp.is_bit(126));

	bbsp.erase_block_pos(2,bberase);
	EXPECT_FALSE(bbsp.is_bit(1000));


	bbsp.erase_block_pos(3,bberase);
	EXPECT_FALSE(bbsp.is_bit(9000));
}

TEST(Sparse, copy_up_to_some_bit) {

	BBScanSp bbsp(10000);
	BBScanSp bbcopy(10000);
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	bbsp.set_bit(9999);
	
	bbcopy.init_bit(126, bbsp);
	EXPECT_EQ(4, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(126));

	bbcopy.init_bit(1000, bbsp);
	EXPECT_EQ(6, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(1000));

	bbcopy.init_bit(9000, bbsp);
	EXPECT_EQ(7, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(9000));

	bbcopy.init_bit(9999, bbsp);
	EXPECT_TRUE(bbcopy==bbsp);
}

TEST(Sparse, copy_in_closed_range){

	BBScanSp bbsp(10000);
	BBScanSp bbcopy(10000);
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	bbsp.set_bit(9999);

	bbcopy.init_bit(126, 1000, bbsp);
	bbcopy.print();
	EXPECT_EQ(3, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(126));
	EXPECT_TRUE(bbcopy.is_bit(127));
	EXPECT_TRUE(bbcopy.is_bit(1000));

	bbcopy.reset(10000);
	bbcopy.init_bit(127, 129, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(127));

	bbcopy.reset(10000);
	bbcopy.init_bit(0, 10000, bbsp);
	EXPECT_TRUE( bbcopy==bbsp);

	bbcopy.reset(10000);
	bbcopy.init_bit(9000, 9999, bbsp);
	EXPECT_EQ(2, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(9000));
	EXPECT_TRUE(bbcopy.is_bit(9999));

	bbcopy.reset(10000);
	bbcopy.init_bit(0, 0, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(0));

	bbcopy.reset(10000);
	bbcopy.init_bit(9999, 9999, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(9999));
}

TEST(Sparse, copy_in_closed_range_special_cases){

	BBScanSp bbsp(120);
	BBScanSp bbcopy(120);
	bbsp.set_bit(64);

	bbcopy.init_bit(0, 51, bbsp);
	EXPECT_EQ(0, bbcopy.popcn64());

	bbcopy.reset(120);
	bbcopy.init_bit(64, 65, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(64));

	bbcopy.reset(120);
	bbcopy.init_bit(63, 64, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(64));
}

TEST(Sparse, keep_operations) {

	BBScanSp bbsp(10000);
	BBScanSp bbkeep(10000);
	BBScanSp bbkeep2(10000);
	BBScanSp bbkeep3(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	
	bbkeep.set_bit(0);
	bbkeep.set_bit(1);
	bbkeep.set_bit(2);
	bbkeep.set_bit(126);
	bbkeep.set_bit(127);
	bbkeep.set_bit(1000);
	bbkeep.set_bit(9000);

	bbkeep2.set_bit(0);
	bbkeep2.set_bit(1);
	bbkeep2.set_bit(126);
	bbkeep2.set_bit(1000);

	bbkeep3.set_bit(0);

	//tests
	bbsp &= bbkeep;
	EXPECT_TRUE(bbsp==bbkeep);

	bbsp.AND_EQ(1, bbkeep2);
	bbsp.print();
	EXPECT_TRUE(bbsp.is_bit(126));
	EXPECT_TRUE(bbsp.is_bit(0));
	EXPECT_FALSE(bbsp.is_bit(127));		//last bit of block 1 deleted
	EXPECT_EQ(5, bbsp.popcn64());

	
	bbsp &= bbkeep3;
	EXPECT_TRUE(bbsp.is_bit(0));
	EXPECT_EQ(1, bbsp.popcn64());

}


