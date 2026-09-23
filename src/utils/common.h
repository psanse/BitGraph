/**
 * @file common.h
 * @brief common utilities for the framework
 * @date ?
 * @last_update 17/01/25
 * @dev: main pss, others ff
 **/


#ifndef BITGRAPH_UTILS_COMMON_H
#define	BITGRAPH_UTILS_COMMON_H


 //comment in release mode - disable checks
 //current impact on stack_t in common_types.h

#ifndef NDEBUG
//#define DEBUG_STACKS		//checks stack sizes (important to debug SAT engine in DEBUG mode) TODO - legacy code, remove (06/10/2025)
#endif 

#include "logger.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <regex>
#include <unordered_set>				//for all_unique

#include "common_types.h"				//common types additional utilities, to be included e
#include "random_utils.h"
#include "prec_timer.h"

namespace bitgraph {
		
	
				

}//end namespace bitgraph


#endif // BITGRAPH_UTILS_COMMON_H