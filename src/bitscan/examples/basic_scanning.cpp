
/**
 * @file basic_scanning.cpp
 * @brief Demonstrates forward and reverse persistent scanning of a bitarray.
 *
 * This example presents the two interfaces available for persistent scanning:
 *
 * - The scanner-wrapper interface, which exposes next_bit() for both forward
 *   and reverse traversal.
 * - The direct interface, which requires next_bit() for forward traversal and
 *   prev_bit() for reverse traversal.
 *
 * Persistent scanning uses the cursor stored in the bitarray and is intended
 * for performance-critical bit-parallel algorithms.
 *
 * @warning Only one persistent scan may be active on a given bitarray at a
 *          time. Initializing another persistent scan on the same bitarray
 *          invalidates the scan already in progress.
 */

#include <iostream>

#include "bitscan/bitscan.h"

int main()
{
    using bitgraph::BBObject;
    using bitgraph::bitarray;

    // Create a 130-bit array with all bits initially cleared.
    bitarray bits(130);

    bits.set_bit(2);
    bits.set_bit(10);
    bits.set_bit(63);
    bits.set_bit(64);
    bits.set_bit(100);
    bits.set_bit(129);

	// Wrapper interface for persistent scanning. This is the most convenient interface for most use cases.

    // Forward scan: increasing bit positions.
    std::cout << "Forward scan: ";

    bitarray::scan forward(bits);

    for (int bit = forward.next_bit();
        bit != BBObject::noBit;
        bit = forward.next_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';

    // Reverse scan: decreasing bit positions.
    std::cout << "Reverse scan: ";

    bitarray::scanR reverse(bits);

    for (int bit = reverse.next_bit();
        bit != BBObject::noBit;
        bit = reverse.next_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';


    // Direct persistent scanning requires: 
    // 
    // 1) the traversal function that matches
    // the initialized direction: next_bit() for forward scans and prev_bit()
    // for reverse scans. Only one persistent scan may be active on a given
    // bitarray at a time.
    //
	// 2) explicit initialization of the scan direction with init_scan().

  
    // This is the lowest-level hot-path interface.	
    std::cout << "Direct persistent forward scan: ";

    bits.init_scan(BBObject::NON_DESTRUCTIVE);

    for (int bit = bits.next_bit();
        bit != BBObject::noBit;
        bit = bits.next_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';
       
    // This is the lowest-level hot-path interface.  
    std::cout << "Direct persistent reverse scan: ";

    bits.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);

    for (int bit = bits.prev_bit();
        bit != BBObject::noBit;
        bit = bits.prev_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';


    return 0;
}