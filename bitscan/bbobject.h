/**  
 * @file bbobject.h
 * @brief Interface for the BITSCAN library hierarchy of classes
 * @created ?
 * @last_update 01/02/2025
 * @author: pss
 *
 * Permission to use, modify and distribute this software is
 * granted provided that this copyright notice appears in all 
 * copies, in source code or in binaries. For precise terms 
 * see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any 
 * kind, express or implied, and with no claim as to its
 * suitability for any purpose.
 *
 **/

#ifndef  __BB_OBJECT_H__
#define  __BB_OBJECT_H__

#include <iostream>
#include "bbtypes.h"
#include "tables.h"

class BBObject{
public:
	static const int noBit = -1;

	//types of bit scans
	enum scan_types	{NON_DESTRUCTIVE, NON_DESTRUCTIVE_REVERSE, DESTRUCTIVE, DESTRUCTIVE_REVERSE};				

	friend std::ostream& operator<< (std::ostream& o , const BBObject& bb)		{ bb.print(o); return o;}

	//Data structure for bitscanning optimization
	struct scan_t {																			
		
		int bbi_;								//bitboard index 	
		int pos_;								//bit index [0...63] 

		scan_t() :bbi_(noBit), pos_(MASK_LIM)		{}
				
		void set_block(int block)					{ bbi_ = block; }
		void set_pos(int bit)						{ pos_ = bit; }
		void set_bit(int bit) {
			int bbh = WDIV(bit);
			bbi_ = bbh;
			pos_ = bit - WMUL(bbh);					/* equiv. to WMOD(bit)*/
		}
	};



//////////////
//construction / destruction
	BBObject() = default;

	virtual ~BBObject() = default;
	
//////////////////	
//bit scanning
	virtual int init_scan			(scan_types)								 { return 0; }

///////////////////
//I/O
	virtual std::ostream& print(	std::ostream& o = std::cout, 
									bool show_pc = true,
									bool endl = true					) const = 0;
};

#endif