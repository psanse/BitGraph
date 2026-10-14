/**
 * @file ordering_decoder.cpp
 * @brief Implementation of graph vertex-ordering composition and decoding.
 *
 * @details
 * This translation unit implements the non-inline operations of
 * @c bitgraph::OrderingDecoder. Each stored ordering maps indices in a
 * reordered graph to indices in the preceding graph. Decoding therefore
 * applies the stored mappings in reverse insertion order to recover the
 * corresponding indices in the original graph.
 *
 * The implementation also computes inverse permutations and applies decoding
 * to individual vertices or complete vertex-ordering collections. In-place
 * collection decoding uses a transformation over the existing container;
 * the value-returning overload leaves its input unchanged.
 *
 * @author Pablo San Segundo
 * @date Created: 29 November 2013
 * @date Last updated: 26 September 2026
 *
 * @see ordering_decoder.h
 * @warning Ordering vectors are expected to contain valid zero-based
 *          permutations. Debug builds validate the relevant index bounds with
 *          assertions.
 */

#include "ordering_decoder.h"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

using namespace bitgraph;

void OrderingDecoder::invert_ordering(
	ordering_type& ordering)
{
	ordering_type inverse =
		inverse_ordering(ordering);

	ordering.swap(inverse);
}

auto OrderingDecoder::inverse_ordering(
	const ordering_type& ordering) -> ordering_type
{
	assert(
		ordering.size() <=
		static_cast<std::size_t>(
			std::numeric_limits<vertex_t>::max()));

	ordering_type inverse(ordering.size());

	for (std::size_t new_index = 0;
		new_index < ordering.size();
		++new_index)
	{
		const vertex_t old_index = ordering[new_index];

		assert(old_index >= 0);
		assert(
			static_cast<std::size_t>(old_index) <
			ordering.size());

		inverse[static_cast<std::size_t>(old_index)] =
			static_cast<vertex_t>(new_index);
	}

	return inverse;
}


vertex_t OrderingDecoder::decode(
	vertex_t vertex) const noexcept
{
	/*
	 * Apply the stored orderings in reverse insertion order to recover the
	 * corresponding vertex in the original graph.
	 */
	for (auto iterator = orderings_.rbegin();
		iterator != orderings_.rend();
		++iterator) {
		assert(vertex >= 0);
		assert(
			static_cast<std::size_t>(vertex) <
			iterator->size());

		vertex = (*iterator)[
			static_cast<std::size_t>(vertex)];
	}

	return vertex;
}

void OrderingDecoder::decode_vertices_in_place(
	ordering_type& ordering) const noexcept
{
	std::transform(
		ordering.cbegin(),
		ordering.cend(),
		ordering.begin(),
		[this](vertex_t vertex) noexcept {
			return decode(vertex);
		});
}

int OrderingDecoder::decode_in_place(
	ordering_type& ordering) const noexcept 
{
	if (ordering.empty()) {
		return -1;
	}

	decode_vertices_in_place(ordering);

	return 0;
}

auto OrderingDecoder::decode(
	const ordering_type& ordering) const -> ordering_type
{
	ordering_type decoded(ordering.size());

	std::transform(
		ordering.cbegin(),
		ordering.cend(),
		decoded.begin(),
		[this](vertex_t vertex) {
			return decode(vertex);
		});

	return decoded;
}


