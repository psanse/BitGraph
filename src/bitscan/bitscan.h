/**
 * @file bitscan.h
 * @brief Public facade for the BITSCAN bitset and scanning APIs.
 *
 * @details
 * This header is the consumer-facing entry point for BITSCAN as distributed
 * with the BITGRAPH C++ library. It includes the dense and sparse bitset
 * hierarchies, persistent scanners, independent scan views, sentinel-based
 * experimental support, and common bitset utilities.
 *
 * The facade provides concise aliases for the primary types:
 * @c bitgraph::bitarray for dense persistent scanning,
 * @c bitgraph::sparse_bitarray for sparse persistent scanning, and the
 * corresponding @c simple_* aliases for bitsets without persistent scan
 * state. View aliases are exposed under @c bitgraph::view, with sparse view
 * aliases under @c bitgraph::view::sparse.
 *
 * Persistent scanners store their cursor in the bitset. View scanners instead
 * own an independent cursor and reference an existing bitset. Destructive
 * scanner variants clear the bits they return. The
 * @c bitgraph::watched_bitarray alias remains experimental and is not fully
 * validated.
 *
 * @author pss
 * @date Last updated: 16 September 2026
 *
 * @see bbscan.h
 * @see bbscan_sparse.h
 * @see bbscan_view.h
 * @see bbscan_view_sparse.h
 * @warning The aliases do not change the ownership or mutation semantics of
 *          the underlying types; consult the referenced class documentation.
 */


#ifndef BITGRAPH_BITSCAN_BITSCAN_H
#define BITGRAPH_BITSCAN_BITSCAN_H

#include "bbsentinel.h"      // Non-sparse hierarchy
#include "bbscan_sparse.h"   // Sparse hierarchy
#include "bbscan_view.h"     // Independent scan views (non-sparse)
#include "bbscan_view_sparse.h" // Independent scan views (sparse)
#include "bbutils.h"         // Basic bitset algorithms

namespace bitgraph {

	//non-sparse

	/**
	 * @brief Basic non-sparse bit array without persistent scanning support.
	 */
	using simple_bitarray = Bitset;

	/**
	 * @brief Non-sparse bit array optimized for persistent hot-path scanning.
	 *
	 * This is the primary bit-array type used by bitset graph encodings.
	 * Persistent scanners are exposed as nested types:
	 * `bitarray::scan`, `bitarray::scanR`, `bitarray::scanD`, and
	 * `bitarray::scanDR`.
	*/
	using bitarray = BBScan;

	/**
	 * @brief Non-sparse bit array with sentinel-based scanning support.
	 * @details currently trial version, not fully tested
	 */
	using watched_bitarray = experimental::BBSentinel;

	//sparse
	
	/**
	 * @brief Basic sparse bit array without persistent scanning support.
	 */
	using simple_sparse_bitarray = BitsetSp;

	/**
	 * @brief Sparse bit array optimized for persistent hot-path scanning.
	*/
	using sparse_bitarray = BBScanSp;
	
}

namespace bitgraph {
	namespace view {

		// Facade aliases for the independent scanning views, which are not tied to a specific bitset type.
		// scan..scanDR: non-sparse bitsets (bitgraph::bitarray); view::sparse::scan..scanDR: sparse bitsets (bitgraph::sparse_bitarray).

		using scan = bitgraph::view::Forward;
		using scanR = bitgraph::view::Reverse;
		using scanD = bitgraph::view::DestructiveForward;
		using scanDR = bitgraph::view::DestructiveReverse;

		// Same for sparse bitsets (bitgraph::sparse_bitarray)
		namespace sparse {
			using scan = bitgraph::view::ForwardSp;
			using scanR = bitgraph::view::ReverseSp;
			using scanD = bitgraph::view::DestructiveForwardSp;
			using scanDR = bitgraph::view::DestructiveReverseSp;
		}

	} // namespace view
} // namespace bitgraph


/////////////////////////
// Examples of API usage:

//bitgraph::bitarray candidates(numVertices);
//
//// Persistent hot-path scanner owned by the BBScan API
//bitgraph::bitarray::scan fastScan(candidates);
//
//// Independent-cursor view over the same public bitarray type
//bitgraph::view::scan scanView(candidates);

////////////////////


#endif // BITGRAPH_BITSCAN_BITSCAN_H
