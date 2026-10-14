 /**
  * @file bbtypes.h
  * @brief Defines the fundamental types and constants used by BITSCAN.
  *
  * This header provides:
  *
  * - fixed-width integer aliases used by bit-parallel operations;
  * - the 64-bit BITBOARD block type;
  * - commonly used bit masks and sentinel values;
  * - constants describing the BITBOARD layout;
  * - bit and block index types;
  * - collection aliases for storing bit positions;
  * - compatibility aliases retained for the legacy BITSCAN API.
  *
  * All compile-time constants are defined as namespace-scope constexpr
  * variables. Under C++14, these constants have internal linkage and may
  * therefore be safely defined in this header.
  *
  * @note BITSCAN requires BITBOARD to be an unsigned 64-bit integer type.
  *
  * @date Last updated: 21/09/2026
  * @author Pablo San Segundo
  **/


#ifndef BITGRAPH_BITSCAN_BBTYPES_H
#define BITGRAPH_BITSCAN_BBTYPES_H

#include <cstdint>
#include <limits>
#include <vector>
#include <set>

namespace bitgraph {

	using U8 = std::uint8_t;
	static_assert(sizeof(U8) * 8 == 8,
		"U8 must be exactly 8 bits");

	using U16 = std::uint16_t;
	using U32 = std::uint32_t;
	static_assert(sizeof(U32) * 8 == 32,
		"U32 must be exactly 32 bits");

	using U64 = std::uint64_t;
	using Ul = unsigned long;							// for intrinsics
	

	using BYTE = U8;
	using BITBOARD = U64;								// alias for semantics
	static_assert(sizeof(BITBOARD) * 8 == 64,
		"BITBOARD must be exactly 64 bits");
	
	constexpr BITBOARD ALL_BITS_SET = ~BITBOARD{ 0 };
	constexpr BITBOARD ONE = ALL_BITS_SET;				// backward compatibility alias for semantics

	constexpr BITBOARD EVEN_BITS_MASK =	static_cast<BITBOARD>(0x5555555555555555ULL);
	constexpr BITBOARD EVEN = EVEN_BITS_MASK;			// backward compatibility alias for semantics
	
	constexpr BITBOARD EMPTY_BLOCK = BITBOARD{ 0 };
	constexpr BITBOARD ZERO = EMPTY_BLOCK;				// backward compatibility alias for semantics
	
	constexpr U32 EVEN_BITS_MASK_32 = static_cast<U32>(0x55555555U);
	constexpr U32 EVEN_32 = EVEN_BITS_MASK_32;			// backward compatibility alias for semantics
	
	constexpr U8 EVEN_BITS_MASK_8 =	static_cast<U8>(0x55U);
	constexpr U8 EVEN_8 = EVEN_BITS_MASK_8;				// backward compatibility alias for semantics
		
	/**
	 * @brief Sentinel representing an empty or invalid table entry.
	 */
	constexpr int EMPTY_ELEM = -1;

	constexpr int WORD_SIZE = std::numeric_limits<BITBOARD>::digits;	
	static_assert(WORD_SIZE == 64, "BITSCAN requires a 64-bit BITBOARD");
	
	/** @brief Largest valid zero-based bit position in a BITBOARD. */
	constexpr int WORD_SIZE_MINUS_ONE = WORD_SIZE - 1;

	/** @brief Limit for mask operations on a single BITBOARD. */
	constexpr int MASK_LIM = WORD_SIZE + 1;
		
	/////////////////////
	// Aliases for bit and block indices

	using block_t = BITBOARD;

	using bit_index_t = int;
	using block_index_t = int;

	using bit_indices = std::vector<bit_index_t>;
	using bit_index_set = std::set<bit_index_t>;

	// aliases for backward compatibility
	using bitpos_list = std::vector<bit_index_t>;
	using bitpos_set = std::set<bit_index_t>;


} // end namespace bitgraph

#endif // BITGRAPH_BITSCAN_BBTYPES_H
