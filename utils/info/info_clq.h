/*
 *info_clq.h: interface for result logs of clique and clique-based algorithms
 *@date 12/12/2024
 *@last_update 16/01/2025
 *@dev pss
 */

 //TODO - move to COPT lib (remove from ::com namespace)

#ifndef __INFO_CLIQUE_H__
#define __INFO_CLIQUE_H__

#include "info_base.h"
#include <iostream>
#include <vector>

//////////////////////
//
//	infoCLQ struct
// 
//  @brief info on results for MCP / MWCP algorithms
//
///////////////////////

template<class W_t = int>
struct infoCLQ : public com::infoBase 
{
	/*
	*@brief clears context
	*@param lazy - if true general info is not cleared
	*/
	void clear(bool lazy = false) override;
		
protected:
	void clearPreprocInfo		();
virtual void clearSearchInfo	();	

///////////
//I/O

	std::ostream& printSummary	(std::ostream & = std::cout) const;										//results (summary) in nice format for console			
	std::ostream& printReport	(std::ostream & = std::cout, bool is_endl = false) const override;		//results in table format for output file

/////////////////////
//data members

	//////////////////////
	//  Preproc info  
	//////////////////////

	//LB bounds - root
	W_t lbRootNoHeur = 0;						//LB at root node (without a sophisticated heur, can use a simple heur)
	W_t lbRootHeur = 0;							//LB at root node (sophisticated heur, e.g. AMTS)

	//UB bounds root
	std::size_t branchingRootSize_;				//size of Branching set at ROOT	

	//sorting
	int idSortReal_ = -1;						//sorting policy really used by the algorithm
	int isDegOrd_ = -1;							//FALSE - sorting partition based (COLOR_SORT), TRUE- sorting degree based (DEGREE_SORT)

	/////////////////////
	//search (main params)
	////////////////////

	W_t incumbent_ = 0;							//best solution value found during search (not in preproc)
	W_t optimum_ = 0;							//best solution value (omega) - will be the same as incumbent is time out limit is not reached
	uint64_t nSteps_ =0;						//recursive calls to the algorithm		
	bool  isTimeOutReached_= false;				//flag time out	
	std::vector<int> sol_;						//solution (vertices)
	
};

//////////////////////
//
//	infoCLQ struct
// 
//  @brief CliSAT algorithm specific
//
///////////////////////

struct infoCliSAT : public infoCLQ<int> {

	//clear
	void clearSearchInfo		() override;

	//I/O
	std::ostream& printReport	(std::ostream & = std::cout, bool is_endl = false) const override;

/////////////////////
//data members

	uint64_t nLastIsetPreFilterCalls_ = 0;
	uint64_t nLastIsetCalls_ = 0;
	uint64_t nCurrIsetCalls_ = 0;
	uint64_t nVertexCalls_ = 0;
	uint64_t nUBpartCalls_ = 0;

	uint64_t nsLastIsetPreFilterCalls_ = 0;
	uint64_t nsLastIsetCalls_ = 0;				//succesful calls to the bound
	uint64_t nsCurrIsetCalls_ = 0;
	uint64_t nsVertexCalls_ = 0;
	uint64_t nsUBpartCalls_ = 0;

};

#endif

