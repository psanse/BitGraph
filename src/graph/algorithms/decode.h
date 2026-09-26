/**
 * @file ordering_decoder.h
 * @brief Defines utilities for composing and decoding graph vertex orderings.
 *
 * Provides OrderingDecoder, which stores a sequence of vertex orderings and
 * maps vertices or collections of vertices back through their composition.
 *
 * @author Pablo San Segundo
 * @date 29/11/13
 * @last_update 17/12/24
 * @date Last updated: 26/09/2026
 */

#ifndef BITGRAPH_GRAPH_ALGORITHMS_ORDERING_DECODER_H
#define BITGRAPH_GRAPH_ALGORITHMS_ORDERING_DECODER_H

#include "graph/graph_types.h"
#include "utils/logger.h"
#include <vector>
#include <cassert>
#include <cstddef>
#include <utility>

namespace bitgraph {


	/**
	 * @brief Decodes vertices through a composition of graph orderings.
	 *
	 * Each stored ordering maps vertex indices in a reordered graph to their
	 * corresponding indices in the preceding graph. Decoding applies the stored
	 * orderings in reverse order to recover indices in the original graph.
	 */
	class OrderingDecoder {
	public:

		using ordering_type = vertex_ordering;
		using container_type = std::vector<ordering_type>;
		using size_type = typename container_type::size_type;
		
		OrderingDecoder() = default;
		~OrderingDecoder() = default;

		OrderingDecoder(const OrderingDecoder&) = default;
		OrderingDecoder& operator=(const OrderingDecoder&) = default;
		OrderingDecoder(OrderingDecoder&&) noexcept = default;
		OrderingDecoder& operator=(OrderingDecoder&&) noexcept = default;

		/**
		* @brief Removes all stored orderings.
		*/
		void clear() {
			orderings_.clear();
		}

		/**
		 * @brief Adds an ordering to the composition.
		 *
		 * Passing the ordering by value allows lvalues to be copied and rvalues
		 * to be moved into the internal collection.
		 *
		 * @param ordering Ordering mapping new indices to preceding indices.
		 */
		void add_ordering(ordering_type ordering) {
			orderings_.push_back(std::move(ordering));
		}


		/**
		 * @brief Determines whether no orderings are stored.
		 */
		bool empty() const noexcept
		{
			return orderings_.empty();
		}

	   /**
	    * @brief Backward-compatible alias for empty().
	    */
		bool is_empty() const noexcept {
			return orderings_.empty();
		}

		/**
		 * @brief Returns the number of stored orderings.
		 */
		size_type size() const noexcept
		{
			return orderings_.size();
		}

		/**
		 * @brief Returns the first stored ordering.
		 *
		 * @return Const reference to the first ordering in the composition.
		 *
		 * @pre The decoder must contain at least one ordering; that is,
		 *      `empty() == false`.
		 *
		 * @note The precondition is checked with an assertion in debug builds.
		 *       Calling this function on an empty decoder in a release build results
		 *       in undefined behavior.
		 *
		 * @warning The returned reference may be invalidated when add_ordering()
		 *          causes the internal container to reallocate, or when clear() is
		 *          called.
		 *
		 * @see empty()
		 */
		/*	const ordering_type& first_ordering() const noexcept
		{
			assert(!orderings_.empty());
			return orderings_.front();
		}*/

		/**
		 * @brief Returns the first stored ordering, or an empty ordering if none exists.
		 *
		 * @return Const reference to the first stored ordering. If the decoder
		 *         contains no orderings, returns a reference to a shared immutable
		 *         empty ordering.
		 *
		 * @note An empty returned ordering does not distinguish between an empty
		 *       decoder and a stored first ordering that is itself empty. Use empty()
		 *       when this distinction is required.
		 * 
		 * @note This function is provided for convenience and backward compatibility.
		 *
		 * @warning The reference to a stored ordering may be invalidated when an
		 *          ordering is added and the internal container reallocates, or when
		 *          clear() is called. The reference to the shared empty ordering
		 *          remains valid for the lifetime of the program.
		  */
		const ordering_type& first_ordering() const
		{
			static const ordering_type empty;
			return orderings_.empty()
				? empty
				: orderings_.front();
		}

		/**
		 * @brief Returns read-only access to all stored orderings.
		 */
		const container_type& orderings() const noexcept
		{
			return orderings_;
		}


		/**
		* @brief Replaces an ordering with its inverse permutation.
		*
		* If @p ordering maps new indices to old indices, the resulting ordering
		* maps old indices to new indices.
		*
		* @param ordering Ordering to invert.
		*
		* @throws std::invalid_argument If @p ordering is not a valid permutation.
		* @throws std::length_error If its size cannot be represented by vertex_t.
		*/
		static void invert_ordering(
			ordering_type& ordering
		);

		/**
		 * @brief Computes the inverse permutation of a vertex ordering.
		 *
		 * If @p ordering maps new vertex indices to old vertex indices, the returned
		 * ordering maps old vertex indices back to new vertex indices.
		 *
		 * @param ordering Vertex ordering to invert.
		 * @return Inverse permutation of @p ordering.
		 *
		 * @pre @p ordering is a valid permutation of
		 *      `[0, ordering.size())`.
		 * @pre `ordering.size()` is representable by vertex_t.
		 *
		 * @note This operation does not reverse the sequence of elements; it computes
		 *       the inverse mapping of the permutation.
		 */
		static ordering_type inverse_ordering(
			const ordering_type& ordering
		);
		
		/**
		* @brief Backward-compatible alias for invert_ordering().
		*/
		static void reverse_in_place(ordering_type& ordering)
		{
			invert_ordering(ordering);
		}

		/**
		* @brief Backward-compatible alias for inverse_ordering().
		*/
		static ordering_type reverse(const ordering_type& ordering)
		{
			return inverse_ordering(ordering);
		}
		
		/**
		 * @brief Decodes one vertex through the stored ordering composition.
		 *
		 * The stored orderings are applied in reverse insertion order to map a vertex
		 * from the most recently reordered graph back to the original graph.
		 *
		 * @param vertex Vertex index to decode.
		 * @return Corresponding vertex index in the original graph. If no orderings
		 *         are stored, returns @p vertex unchanged.
		 *
		 * @pre At every decoding step, @p vertex is a valid index for the
		 *      corresponding ordering.
		 *
		 * @note The precondition is checked with assertions in debug builds.
		 *       Violating it in a release build results in undefined behavior.
		 */
		vertex_t decode(vertex_t vertex) const noexcept;


		/**
		 * @brief Decodes a collection of vertices.
		 *
		 * @param vertices Vertices in the most recently reordered graph.
		 * @return Corresponding vertices in the original graph.
		 *
		 * If no orderings are stored, a copy of @p vertices is returned.
		 *
		 * @throws std::out_of_range If an intermediate vertex index is invalid.
		 */
		ordering_type decode(
			const ordering_type& vertices) const;

		/**
		* @brief Decodes a collection of vertices in place.
		*
		* An empty collection is accepted and remains unchanged.
		*
		* @param vertices Vertices to decode.
		*
		* @throws std::out_of_range If an intermediate vertex index is invalid.
		*/
		void decode_vertices_in_place(
			ordering_type& vertices) const noexcept;


		/**
		 * @brief Backward-compatible in-place decoding operation.
		 *
		 * @param vertices Vertices to decode.
		 * @return `-1` if @p vertices is empty; otherwise `0`.
		 *
		 * @deprecated Use decode_vertices_in_place(), which treats an empty
		 *             collection as a valid no-op.
		 *
		 * @throws std::out_of_range If an intermediate vertex index is invalid.
		 */
		int decode_in_place(ordering_type& vertices) const noexcept;
		
	private:				

		std::vector<ordering_type> orderings_;					
	};
		
	// backward compatibility alias
	
	using Decode = OrderingDecoder;


}//end of namespace bitgraph



#endif // BITGRAPH_GRAPH_ALGORITHMS_ORDERING_DECODER_H