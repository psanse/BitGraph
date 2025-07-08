/**
* @file bbexcep_hand.h
* @brief contains exception handling for the bitscan lib part of the bitgraph lib
* @details created 07/07/2025, last_update 08/07/2025
**/

#ifndef __BBEXCEP_HAND_H__	
#define __BBEXCEP_HAND_H__

#include <iostream>
#include <exception>


class BitScanError : public std::runtime_error {

public:
	explicit BitScanError(const std::string& message)
		: std::runtime_error("BitScanError: " + message) {}
};


#endif