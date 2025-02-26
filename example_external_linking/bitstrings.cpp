/**
* @file bitstring.cpp
* @brief Example of how to link to the BITSCAN lib.
* @details created 26/02/2025
**/

#include <iostream>
#include <bitscan/bitscan.h>		//the one and only header file for the BITSCAN 1.0 lib

int main() {

	//normal bitset 
	bitarray bb(130);
	bb.set_bit(0);
	bb.set_bit(129);
	bb.print();
		
	
	cout << "****************************" << endl;

	//massive bitset 10M bits
	sparse_bitarray sb(10000000);
	sb.set_bit(0);
	sb.set_bit(900000);
	sb.print();

	
	cout << "****************************" << endl;

	

	return 0;
}