/**
* @file bbexcep_hand.h
* @brief contains exception handling for the bitscan lib part of the bitgraph lib
* 
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