/**
* @file bitstring.cpp
* @brief Example of how to link to the BITSCAN lib.
* @details created 26/02/2025
**/

using namespace std;

#include <iostream>
#include <BitGraph/bitscan/bitscan.h>		//the one and only header file for the BITSCAN 1.0 lib

int main() {

	//small bitset (130 elements)
	bitarray bb(130);
	bb.set_bit(0);
	bb.set_bit(129);
	bb.print();
		
	cout << "****************************" << endl;

	//massive bitset (10M elements)
	sparse_bitarray sb(10000000);
	sb.set_bit(0);
	sb.set_bit(900000);
	sb.print();

	
	cout << "****************************" << endl;


	//non-destructive forward bitscanning (manual)
	bb.init_scan(bbo::NON_DESTRUCTIVE);
	int nBit = bbo::noBit;
	while ((nBit = bb.next_bit()) != bbo::noBit) {
		cout << "bit scanned:" << nBit << endl;
	}

	cout << "****************************" << endl;

	//non-destructive forward bitscanning (specialized scanning objects)
	bbo::Scan<bitarray> sc(bb);
	while ( (nBit = sc.next_bit()) != bbo::noBit) {
		cout << "bit scanned:" << nBit << endl;
	}

	cout << "****************************" << endl;

	//non-destructive forward bitscanning for sparse bitsets
	bbo::Scan<sparse_bitarray> scSparse(sb);
	while ((nBit = scSparse.next_bit()) != bbo::noBit) {
		cout << "sparse bit scanned:" << nBit << endl;
	}

	return 0;
}