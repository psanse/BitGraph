/**
 * @file bitset_core_api.cpp
 * @brief Demonstrates the core bitarray API.
 *
 * This example covers storage information, bit queries, single-bit and range
 * modification, logical assignment operations, intersection testing, and
 * bitarray reinitialization.
 *
 * Bit ranges are closed: both firstBit and lastBit are included.
 */

#include <iostream>

#include "bitscan/bitscan.h"

int main()
{
	// bitarray is the recommended public type. The same core API is also
	// available through the lower-level simple_bitarray type.
	using bitset_type = bitgraph::bitarray;

	std::cout << std::boolalpha;

	// Create a bitarray with capacity for at least 130 positions.
	bitset_type bits(130);

	// Storage and population queries.
	std::cout << "Number of blocks: "
		<< bits.num_blocks() << '\n';
	std::cout << "Physical bit capacity: "
		<< bits.bit_capacity() << '\n';
	std::cout << "Initially empty: "
		<< bits.is_empty() << '\n';
	std::cout << "Initial population count: "
		<< bits.count() << '\n';

	/*
	 * Set individual positions and the closed range [20,25].
	 *
	 * Modification functions return *this, allowing operations to be chained.
	 */
	bits.set(3)
		.set(10)
		.set_range(20, 25);

	std::cout << "\nAfter setting bits:\n";
	std::cout << "Bit 3 is set: " << bits.test(3) << '\n';
	std::cout << "Bit 4 is set: " << bits.test(4) << '\n';
	std::cout << "Population count: " << bits.count() << '\n';

	// Clear one position and the closed range [22,24].
	bits.clear(10)
		.clear_range(22, 24);

	std::cout << "\nAfter clearing bits:\n";
	std::cout << "Bit 10 is set: " << bits.test(10) << '\n';
	std::cout << "Bit 22 is set: " << bits.test(22) << '\n';
	std::cout << "Population count: " << bits.count() << '\n';

	// bits now contains {3,20,21,25}.
	bitset_type rhs(130);

	rhs.set(3)
		.set(25)
		.set(64);

	std::cout << "\nThe bitarrays intersect: "
		<< bits.intersects(rhs) << '\n';

	// Intersection: {3,20,21,25} intersect {3,25,64} = {3,25}.
	bitset_type intersection = bits;
	intersection. and_eq (rhs);

	std::cout << "Intersection population: "
		<< intersection.count() << '\n';

	// Union: {3,20,21,25} union {3,25,64} = {3,20,21,25,64}.
	bitset_type union_set = bits;
	union_set. or_eq (rhs);

	std::cout << "Union population: "
		<< union_set.count() << '\n';

	// Set difference: {3,20,21,25} minus {3,25,64} = {20,21}.
	bitset_type difference = bits;
	difference.andnot_eq(rhs);

	std::cout << "Difference population: "
		<< difference.count() << '\n';

	/*
	 * Reinitialize the bitarray with capacity for at least 70 positions.
	 *
	 * The previous contents are discarded and all bits are cleared.
	 */
	bits.reset(70);

	std::cout << "\nAfter reset:\n";
	std::cout << "Number of blocks: "
		<< bits.num_blocks() << '\n';
	std::cout << "Physical bit capacity: "
		<< bits.bit_capacity() << '\n';
	std::cout << "Empty: "
		<< bits.is_empty() << '\n';
	std::cout << "Population count: "
		<< bits.count() << '\n';

	return 0;
}