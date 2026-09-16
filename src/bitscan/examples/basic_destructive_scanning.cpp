/**
 * @file destructive_scanning.cpp
 * @brief Demonstrates forward and reverse destructive scanning of a bitarray.
 *
 * A destructive scan clears each set bit from the bitarray as it is returned.
 * This example presents the two persistent destructive-scanning interfaces:
 *
 * - The scanner-wrapper interface, which exposes next_bit() for both forward
 *   and reverse traversal.
 * - The direct interface, which requires next_bit_del() for forward traversal
 *   and prev_bit_del() for reverse traversal.
 *
 * @warning Destructive scanning modifies the scanned bitarray.
 * @warning Only one persistent scan may be active on a given bitarray at a
 *          time.
 */

#include <iostream>

#include "bitscan/bitscan.h"

int main()
{
    using bitgraph::BBObject;
    using bitgraph::bitarray;

    // Create a 130-bit array with all bits initially cleared.
    bitarray original(130);

    original.set_bit(2);
    original.set_bit(10);
    original.set_bit(63);
    original.set_bit(64);
    original.set_bit(100);
    original.set_bit(129);

    /*
     * Scanner-wrapper interface
     *
     * This is the recommended persistent destructive-scanning interface for
     * most uses. Both forward and reverse scanners expose next_bit().
     */

     // Destructive forward scan in increasing bit-position order.
    {
        bitarray bits = original;

        std::cout << "Destructive forward scan: ";

        bitarray::scanD forward(bits);

        for (int bit = forward.next_bit();
            bit != BBObject::noBit;
            bit = forward.next_bit()) {
            std::cout << bit << ' ';
        }

        std::cout << '\n';

        // All originally set bits have now been cleared from bits.
    }

    // Destructive reverse scan in decreasing bit-position order.
    {
        bitarray bits = original;

        std::cout << "Destructive reverse scan: ";

        bitarray::scanDR reverse(bits);

        for (int bit = reverse.next_bit();
            bit != BBObject::noBit;
            bit = reverse.next_bit()) {
            std::cout << bit << ' ';
        }

        std::cout << '\n';

        // All originally set bits have now been cleared from bits.
    }

    /*
     * Direct persistent destructive-scanning interface
     *
     * This is the lowest-level hot-path interface. The traversal function
     * must match the direction selected during initialization:
     *
     *   - next_bit_del() for forward destructive scanning;
     *   - prev_bit_del() for reverse destructive scanning.
     * 
	 * Also the scanner must be initialized with the appropriate direction.
     */

     // Direct destructive forward scan.
    {
        bitarray bits = original;

        std::cout << "Direct destructive forward scan: ";

        bits.init_scan(BBObject::DESTRUCTIVE);

        for (int bit = bits.next_bit_del();
            bit != BBObject::noBit;
            bit = bits.next_bit_del()) {
            std::cout << bit << ' ';
        }

        std::cout << '\n';
    }

    // Direct destructive reverse scan.
    {
        bitarray bits = original;

        std::cout << "Direct destructive reverse scan: ";

        bits.init_scan(BBObject::DESTRUCTIVE_REVERSE);

        for (int bit = bits.prev_bit_del();
            bit != BBObject::noBit;
            bit = bits.prev_bit_del()) {
            std::cout << bit << ' ';
        }

        std::cout << '\n';
    }

    return 0;
}