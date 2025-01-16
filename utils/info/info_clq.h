/*
 *info_clq.h: interface for result logs of clique and clique-based algorithms
 *@date 12/12/2024
 *@last_update 13/23/2024
 *@dev pss
 */

 //TODO - move to clique algorithms asap (not in com:: namespace)

#ifndef __INFO_CLIQUE_H__
#define __INFO_CLIQUE_H__

#include "info_base.h"
#include <iostream>
#include <vector>

//////////////////////
//
//	infoCLQ
// 
//  @brief info on results for MCP / MWCP algorithms
//		   (some are CliSAT specific)
//
///////////////////////

template<class W_t = int>
struct infoCLQ : public com::infoBase {
	infoCLQ() :infoBase(),
		incumbent_(0), optimum_(0), nSteps_(0), isTimeOutReached_(false),
		LB_0_no_AMTS_(0), LB_0_AMTS_(0),
		branchingRootSize_(0),
		idSortReal_(-1),
		isDegOrd_(false),
		nLastIsetPreFilterCalls_(0), nLastIsetCalls_(0), nCurrIsetCalls_(0), nVertexCalls_(0), nUBpartCalls_(0),
		nsLastIsetPreFilterCalls_(0), nsLastIsetCalls_(0), nsCurrIsetCalls_(0), nsVertexCalls_(0), nsUBpartCalls_(0)
	{}

	//context
	void clearPreprocInfo() override;
	void clearSearchInfo() override;

	/*
	*@brief clears context
	*@param lazy - if true, only search and preprocessing info is cleared
	*/
	void clear(bool lazy = true) {
		clearPreprocInfo();
		clearSearchInfo();
		clearTimers();
		if (!lazy) {
			com::infoBase::clear();						//clears general info
		}
	}

	//I/O
	std::ostream& printResults(std::ostream & = std::cout) const;							//results (summary) in nice format for console			
	std::ostream& printTable(std::ostream & = std::cout) const override;					//results in table format for output file

/////////////////////
//data members

	/////////////////////
	//search
	////////////////////
	W_t incumbent_;							//best solution value found during search (not in preproc)
	W_t optimum_;							//best solution value (omega) - will be the same as incumbent is time out limit is not reached
	uint64_t nSteps_;						//recursive calls to the algorithm		
	bool  isTimeOutReached_;				//flag time out	
	std::vector<int> sol_;					//solution (vertices)

	//////////////////////
	//  Preproc info  
	//////////////////////

	//LB bounds - root
	W_t LB_0_no_AMTS_;						//LB_0 without AMTS
	W_t LB_0_AMTS_;							//LB_0 with AMTS

	//UB bounds root
	std::size_t branchingRootSize_;			//size of Branching set at ROOT	

	//sorting
	int idSortReal_;						//sorting policy really used by the algorithm
	bool isDegOrd_;							//FALSE - sorting partition based (COLOR_SORT), TRUE- sorting degree based (DEGREE_SORT)

	//////////////////////
	//	Additional search info 
	// (specific of certain algorithms - CliSAT)
	//////////////////////
	uint64_t nLastIsetPreFilterCalls_;		//calls to the bound
	uint64_t nLastIsetCalls_;				//calls to the bound
	uint64_t nCurrIsetCalls_;
	uint64_t nVertexCalls_;
	uint64_t nUBpartCalls_;

	uint64_t nsLastIsetPreFilterCalls_;
	uint64_t nsLastIsetCalls_;				//succesful calls to the bound
	uint64_t nsCurrIsetCalls_;
	uint64_t nsVertexCalls_;
	uint64_t nsUBpartCalls_;

};




#endif






