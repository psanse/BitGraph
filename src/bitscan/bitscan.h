/**
 * @file bitscan.h
 * @brief facade aliases for consumer code of the BITSCAN lib, a C++ library for bitset optimization.
 * @details The BITSCAN library is currently released as part of the BITGRAPH C++ library. 
 * @details
 *  - date 2014
 *  - last modified 16/09/2026
 * @author pss
 **/

#include "bbsentinel.h"      // Non-sparse hierarchy
#include "bbscan_sparse.h"   // Sparse hierarchy
#include "bbscan_view.h"     // Independent scan views
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
	using watched_bitarray = _impl::BBSentinel;

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
		// currently only of non-sparse bitsets (bitgraph::bitarray), but to be extended to sparse bitsets in the future.

		using scan = bitgraph::view::Forward;
		using scanR = bitgraph::view::Reverse;
		using scanD = bitgraph::view::Destructive;
		using scanDR = bitgraph::view::DestructiveReverse;	

	} // namespace view
} // namespace bitgraph


/////////////////////////
// Examples of usage:

//bitgraph::bitarray candidates(numVertices);
//
//// Persistent hot-path scanner owned by the BBScan API
//bitgraph::bitarray::scan fastScan(candidates);
//
//// Independent-cursor view over the same public bitarray type
//bitgraph::view::scan scanView(candidates);

////////////////////
