/**
 * @file tables.h
 * @brief Internal lookup tables and masks for BITSCAN bit operations.
 *
 * @details
 * This header declares the non-instantiable @c bitgraph::detail::Tables
 * holder used by the low-level bitblock algorithms. Its static data includes
 * single-bit, interval, and word masks; population-count and least/most-
 * significant-bit lookup tables; De Bruijn index tables; and optional caches
 * controlled by the BITSCAN configuration macros.
 *
 * The public @c Tables::InitAllTables() function is the initialization driver
 * for the generated mutable tables. The library invokes it during startup so
 * the lookup data is ready before normal bitset operations. Compile-time
 * options such as @c EXTENDED_LOOKUPS and @c CACHED_INDEX_OPERATIONS control
 * whether the corresponding optional tables are declared and initialized.
 *
 * The tables use the 64-bit @c BITBOARD and related types defined in
 * @c bbtypes.h. They are shared implementation state and should be accessed
 * through the BITSCAN operations rather than modified by client code.
 *
 * @author Pablo San Segundo
 * @date Last updated: 14 October 2026
 *
 * @see bbtypes.h
 * @see bitblock.h
 * @see tables.cpp
 * @warning The declarations in @c bitgraph::detail are implementation details
 *          and are not part of the supported public API.
 */

#ifndef BITGRAPH_BITSCAN_TABLES_H
#define BITGRAPH_BITSCAN_TABLES_H

#include "bbtypes.h"

namespace bitgraph {

	namespace detail {

		class Tables {

			Tables() = delete;
			~Tables() = delete;
		
		public:
			static void InitAllTables();							//Driver for all inits
		
		private:
			static void init_masks();
			static void init_popc8();
			static void init_popc();
			static void init_mlsb();
			static void init_lsb_l();							//Conditioned to EXTENDED_LOOKUPS 

			//Table
			static void init_cached_index();					//Conditioned to CACHED_INDEX_OPERATIONS 

			////////////////////////////////////////
			//data members

		public:
			//commonly used tables
			static BITBOARD	mask[64];						//masks for 64 bit block of a single bit
			static U8 mask8[8];								//masks for 8 bit block of a single bit
			static BITBOARD mask_low[65];					//1_bit to the right of index (less significant bits, excluding index) - mask_low[WORD_SIZE] = ONE
			static BITBOARD mask_high[66];					//1_bit to the left of index (more significant bits, excluding index)

			//private:
			static BITBOARD mask_mid[64/*a*/][64/*b*/];	//1-bits between intervals (a<=b)

			//0 but word masks
			static BITBOARD  mask0_1W;
			static BITBOARD  mask0_2W;
			static BITBOARD  mask0_3W;
			static BITBOARD	 mask0_4W;

			static int pc[65536];							//16 bit population count
			static int lsb[65536];							//LSB lookup table 16 bits
			static int lsba[4][65536];						//LSB lookup table 16 bits con pos indes
			static int msba[4][65536];						//MSB lookup table 16 bits con pos indes


			static int pc8[256];					//population count for 8 bits
			static int pc_sa[65536];				//populaton count for 16 bits (Shift + Add)
			static int msb[65536];					//MSB for 16 bits

#ifdef EXTENDED_LOOKUPS	
			static int lsb_l[65536][16];			//LSB for 16 bits list of position of 1-bits)
#endif

			////////////////////////
			//magic number tables

			static const int T_32[37];									//32 bits	
			static const int T_64[67];									//64 bits
			static const int indexDeBruijn64_ISOL[64];					//bit scan with b&(-b)	
			static const int indexDeBruijn64_SEP[64];					//bit scan with b^(b-1)

			//64 bit block index cache
#ifdef  CACHED_INDEX_OPERATIONS 
			static int t_wdindex[MAX_CACHED_INDEX];
			static int t_wxindex[MAX_CACHED_INDEX];
			static int t_wmodindex[MAX_CACHED_INDEX];
#endif

		};

	}//end of namespace detail

}//end of namespace bitgraph

#endif // BITGRAPH_BITSCAN_TABLES_H
