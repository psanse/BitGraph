/*
 *info_clq.h: interface for result logs of clique and clique-based algorithms
 *@date 12/12/2024
 *@last_update 16/01/2025
 *@dev pss
 */

 //TODO - move infoCliSAT to COPT lib (remove from ::com namespace)

#ifndef __INFO_CLIQUE_H__
#define __INFO_CLIQUE_H__

#include "info_base.h"
#include <iostream>
#include <vector>

namespace com{
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
		/**
		* @brief clears context
		* @param lazy - if true general info is not cleared
		**/
		void clear(bool lazy = false) override;

	protected:
		void clearPreprocInfo();
		virtual void clearSearchInfo();

		///////////
		//I/O
	public:
		/**
		*
		* @brief streams main search info: sol value, times and number of steps
		* @param o: output stream
		* @returns output stream
		*
		**/
		std::ostream& printSummary(std::ostream & = std::cout) const;										//results (summary) in nice format for console			

		/**
		*
		* @brief streams all data
		* @param o: output stream
		* @param is_endl: if true, adds a new line at the end
		* @returns output stream
		*
		**/
		std::ostream& printReport(std::ostream & = std::cout, bool is_endl = false) const override;		//results in table format for output file

		/////////////////////
		//data members

		//////////////////////
		//  Preproc info  
		//////////////////////

		//LB bounds - root
		W_t lbRootNoHeur_ = 0;						//LB at root node (without a sophisticated heur, can use a simple heur)
		W_t lbRootHeur_ = 0;						//LB at root node (sophisticated heur, e.g. AMTS)
				
		//UB bounds root
		W_t ubRoot_ = 0;							//UB at root node (size of the graph) 
		std::size_t branchingFactorRoot_ = 0;		//size of Branching set at ROOT	

		//sorting
		int algSortReal_ = -1;						//sorting policy actually used by the algorithm (infoBase::idSort_ is the configuration parameter)
		bool isAlgSortRealDeg_ = false;				//flag to indicate if any varaint of  DEG-SORT has been used.  CHECK, looksidSO redundant with idsortReal (16/01/2025)

		/////////////////////
		// Search info (main params)
		////////////////////

		W_t lb_ = 0;								//incumbent solution value found during search (initially - max(lbRootHeur_, lbRootNoHeur_), optimum if isTimeOut_=false)
		W_t ub_ = 0;								//upper bound found during search (initially - ubRoot_)
		uint64_t nSteps_ = 0;						//recursive calls to the algorithm		
		bool  isTimeOut_ = false;					//flag time out	
		std::vector<int> sol_;						//solution found (set of vertices) - TODO - change to a set of maximum cliques?

	};

	//////////////////////
	//
	//	infoCliSAT struct
	// 
	//  @brief CliSAT algorithm specific
	//
	///////////////////////

	struct infoCliSAT : public infoCLQ<int> {

		/**
		* @brief clears search data.
		* @comment infoCLQ::clear() uses the Template Method Pattern
		**/
		void clearSearchInfo() override;

		/**
		*
		* @brief streams all data
		* @param o: output stream
		* @param is_endl: if true, adds a new line at the end
		* @returns output stream
		*
		**/
		std::ostream& printReport(std::ostream & = std::cout, bool is_endl = false) const override;

		/////////////////////
		//data members

		//search info CliSAT specific
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

}//namespace com	

#endif

