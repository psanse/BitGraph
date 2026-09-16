/**
 * @file independent_views.cpp
 * @brief Demonstrates independent scan views over the same bitarray.
 *
 * Each scan view owns its cursor instead of using the persistent cursor stored
 * in the bitarray. Multiple non-destructive views can therefore traverse the
 * same bitarray independently and simultaneously.
 *
 * The example demonstrates:
 *
 * - convenience aliases bound to bitarray, such as view::Forward;
 * - generic aliases with an explicit bitset type, such as
 *   view::ForwardView<bitarray>;
 * - simultaneous forward and reverse traversal of the same bitarray.
 *
 * The view type determines the traversal direction. All views expose the same
 * next_bit() operation.
 *
 * @note The referenced bitarray must remain valid throughout the lifetime of
 *       every view.
 *
 * @warning Destructive views modify the referenced bitarray and must not
 *          overlap with another view accessing the same bitarray.
 */

#include <iostream>

#include "bitscan/bitscan.h"

int main()
{
    using bitgraph::bitarray;

    // Create a 130-bit array with all bits initially cleared.
    bitarray bits(130);

    bits.set_bit(2);
    bits.set_bit(10);
    bits.set_bit(63);
    bits.set_bit(64);
    bits.set_bit(100);
    bits.set_bit(129);

    /*
     * Create forward and reverse views over the same bitarray.
     *
     * Each view owns an independent cursor, so advancing one view does not change
     * the position of the other. Both views expose next_bit(); the view type
     * determines the traversal direction.
     */

    bitgraph::view::scan forward(bits);
    bitgraph::view::scanR reverse(bits);

    forward.init_scan();
    reverse.init_scan();

    std::cout << "Interleaved independent scans:\n";

    int forwardBit = forward.next_bit();
    int reverseBit = reverse.next_bit();

    while (forwardBit != bitarray::noBit ||
        reverseBit != bitarray::noBit) {

        if (forwardBit != bitarray::noBit) {
            std::cout << "  forward: " << forwardBit << '\n';
            forwardBit = forward.next_bit();
        }

        if (reverseBit != bitarray::noBit) {
            std::cout << "  reverse: " << reverseBit << '\n';
            reverseBit = reverse.next_bit();
        }
    }

    /*
    * Reinitialize both views and perform complete scans. Reinitializing a view
    * resets only its own cursor.
    */

    std::cout << "\nComplete forward view: ";

    forward.init_scan();

    for (int bit = forward.next_bit();
        bit != bitarray::noBit;
        bit = forward.next_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';

    std::cout << "Complete reverse view: ";

    reverse.init_scan();

    for (int bit = reverse.next_bit();
        bit != bitarray::noBit;
        bit = reverse.next_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';


    /*
     * Generic scan-view aliases
     *
     * The generic aliases accept the bitset type explicitly. This is useful for
     * algorithms that select the bitset implementation through a template
     * parameter.
     */

    std::cout << "\nGeneric forward view: ";

    bitgraph::view::ForwardView<bitarray> genericForward(bits);
    genericForward.init_scan();

    for (int bit = genericForward.next_bit();
        bit != bitarray::noBit;
        bit = genericForward.next_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';

    std::cout << "Generic reverse view: ";

    bitgraph::view::ReverseView<bitarray> genericReverse(bits);
    genericReverse.init_scan();

    for (int bit = genericReverse.next_bit();
        bit != bitarray::noBit;
        bit = genericReverse.next_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';

    return 0;
}