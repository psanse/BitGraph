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

class BBObject{
public:
	static const int NOBIT = -1;

	//types of bit scans
	enum scan_types	{NON_DESTRUCTIVE, NON_DESTRUCTIVE_REVERSE, DESTRUCTIVE, DESTRUCTIVE_REVERSE};				

	friend std::ostream& operator<< (std::ostream& o , const BBObject& bb)		{ bb.print(o); return o;}

	//Data structure for bitscanning optimization
	struct scan_t {																			
		
		int bbi;								//bitboard index 	
		int pos;								//bit position for bitscan

		scan_t() :bbi(EMPTY_ELEM), pos(MASK_LIM)	{}
		void set_block(int block)					{ bbi = block; }	
		void set_bit(int bit)						{ pos = bit; }
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