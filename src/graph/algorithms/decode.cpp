/**
 * @file decode.cpp
 * @brief Implements graph vertex-ordering decoding utilities.
 *
 * Contains the non-inline definitions of the OrderingDecoder class, including
 * permutation inversion and the decoding of individual vertices and vertex
 * collections through a composition of stored orderings.
 *
 * @author Pablo San Segundo
 * @date created: 29/11/2013
 * @date last updated: 26/09/2026
 */

#include "graph/algorithms/decode.h"
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


