/**
* @file graph_excep_hand.h
* @brief contains exception handling for the GRAPH lib part of the BITGRAPH lib
* @details created 08/07/2025
**/

#ifndef __GRAPH_EXCEP_HAND_H__	
#define __GRAPH_EXCEP_HAND_H__

#include <iostream>
#include <exception>


class GraphParseError : public std::invalid_argument {

public:
	explicit GraphParseError(const std::string& message)
		: std::invalid_argument("GraphParseError: " + message) {}
};


#endif