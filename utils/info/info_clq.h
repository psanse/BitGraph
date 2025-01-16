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
//
///////////////////////

template<class W_t = int>
struct infoCLQ : public com::infoBase {
	infoCLQ() :infoBase(),
		incumbent_(0), optimum_(0), nSteps_(0), isTimeOutReached_(false),
		lbRootNoHeur(0), lbRootHeur(0),
		branchingRootSize_(0),
		idSortReal_(-1),
		isDegOrd_(false),
	/*	nLastIsetPreFilterCalls_(0), nLastIsetCalls_(0), nCurrIsetCalls_(0), nVertexCalls_(0), nUBpartCalls_(0),
		nsLastIsetPreFilterCalls_(0), nsLastIsetCalls_(0), nsCurrIsetCalls_(0), nsVertexCalls_(0), nsUBpartCalls_(0)*/
	{}

	//context
	virtual void clearPreprocInfo();
	virtual void clearSearchInfo();

	/*
	*@brief clears context
	*@param lazy - if true general info is not cleared
	*/
	void clear(bool lazy = false) override {
		clearPreprocInfo();
		clearSearchInfo();
		com::infoBase::clear(lazy);	
	}

	//I/O
	std::ostream& printSummary	(std::ostream & = std::cout) const;										//results (summary) in nice format for console			
	std::ostream& printTable	(std::ostream & = std::cout, bool is_endl = false) const override;				//results in table format for output file

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
	W_t lbRootNoHeur;						//LB_0  (without a sophisticated heur, can use a simple heur)
	W_t lbRootHeur;							//LB_0 	(sophisticated heur, e.g. AMTS)

	//UB bounds root
	std::size_t branchingRootSize_;			//size of Branching set at ROOT	

	//sorting
	int idSortReal_;						//sorting policy really used by the algorithm
	bool isDegOrd_;							//FALSE - sorting partition based (COLOR_SORT), TRUE- sorting degree based (DEGREE_SORT)
	
};

////////////////////////
// CliSAT specific

struct infoCliSAT : public infoCLQ<int> {

	//specific data for CliSAT algorithm
	uint64_t nLastIsetPreFilterCalls_ = 0;		
	uint64_t nLastIsetCalls_ =0;				
	uint64_t nCurrIsetCalls_ =0;
	uint64_t nVertexCalls_=0;
	uint64_t nUBpartCalls_=0;

	uint64_t nsLastIsetPreFilterCalls_=0;
	uint64_t nsLastIsetCalls_=0;				//succesful calls to the bound
	uint64_t nsCurrIsetCalls_=0;
	uint64_t nsVertexCalls_=0;
	uint64_t nsUBpartCalls_=0;

	//clear
	void clearSearchInfo() override;

	//I/O
	std::ostream& printTable(std::ostream & = std::cout, bool is_endl = false) const override;				//results in table format for output file

};

#endif

//////////////////////////////////
//

inline void infoCliSAT::clearSearchInfo()
{
	infoCLQ<int>::clearSearchInfo();

	//CliSAT specific
	nLastIsetCalls_ = 0; nUBpartCalls_ = 0; nCurrIsetCalls_ = 0; nVertexCalls_ = 0; nLastIsetPreFilterCalls_ = 0;
	nsLastIsetCalls_ = 0;  nsUBpartCalls_ = 0; nsCurrIsetCalls_ = 0; nsVertexCalls_ = 0; nsLastIsetPreFilterCalls_ = 0;
}


inline std::ostream& infoCliSAT::printTable(std::ostream& o, bool is_endl) const
{
	infoCLQ<int>::printTable(o, false);

	  o << nLastIsetCalls_ << "\t" << nsLastIsetCalls_ << "\t" << nLastIsetPreFilterCalls_ << "\t" << nsLastIsetPreFilterCalls_ << "\t"
		<< nCurrIsetCalls_ << "\t" << nsCurrIsetCalls_ << "\t"
		<< nVertexCalls_ << "\t" << nsVertexCalls_ << "\t" << nUBpartCalls_ << "\t" << nsUBpartCalls_ << "\t" << K_;

		if (is_endl) {
			o << std::endl;
		}


	return o;
}
