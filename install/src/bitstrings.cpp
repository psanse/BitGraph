/**
* @file bitstring.cpp
* @brief Example of BITSCAN, part of the BITGRAPH lib.
* @details created 26/02/2025, last update 04/07/2025
**/

#include <iostream>
#include <BitGraph/bitscan/bitscan.h>		//the one and only header file for the BITSCAN 1.0 lib

using namespace std;
//using namespace bitgraph;					//alternatively, to avoid typing "bitgraph::" before each type


int main() {

	//small bitset (130 elements)
	bitgraph::bitarray bb(130);
	bb.set_bit(0);
	bb.set_bit(129);
	bb.print();
		
	cout << "****************************" << endl;

	//massive bitset (10M elements)
	bitgraph::sparse_bitarray sb(10000000);
	sb.set_bit(0);
	sb.set_bit(900000);
	sb.print();

	
	cout << "****************************" << endl;


	//non-destructive forward bitscanning (manual)
	bb.init_scan(bitgraph::bbo::NON_DESTRUCTIVE);
	int nBit = bitgraph::bbo::noBit;
	while ((nBit = bb.next_bit()) != bitgraph::bbo::noBit) {
		cout << "bit scanned:" << nBit << endl;
	}

	cout << "****************************" << endl;

	//non-destructive forward bitscanning (specialized scanning objects)
	bitgraph::bbo::Scan<bitgraph::bitarray> sc(bb);
	while ( (nBit = sc.next_bit()) != bitgraph::bbo::noBit) {
		cout << "bit scanned:" << nBit << endl;
	}

	cout << "****************************" << endl;

	//non-destructive forward bitscanning for sparse bitsets
	bitgraph::bbo::Scan<bitgraph::sparse_bitarray> scSparse(sb);
	while ((nBit = scSparse.next_bit()) != bitgraph::bbo::noBit) {
		cout << "sparse bit scanned:" << nBit << endl;
	}

	return 0;
}