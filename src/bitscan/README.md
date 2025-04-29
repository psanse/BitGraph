BITSCAN  - DOCUMENTATION UNDER DEVELOPMENT
===================
BITSCAN is a C++ library designed to efficiently manipulate bit arrays—also called bit strings or bit sets—with a particular emphasis on high-performance bitscanning operations. The library is the outcome of over **two decades of research in combinatorial optimization** and is very efficient when working with sets. In this context, a bitset encodes a set of elements, where each element is represented by a single bit set to 1.

Main Features
-------------------------------
BITSCAN features several low-level optimizations for bitscanning—the enumeration of set bits—that have proven effective in the implementation of high-performance combinatorial optimization algorithms. Among other features, it supports both destructive and non-destructive scans, forward and reverse scans, random starting points and caching of the last scanned bit. It also includes many operations for single 64-bit numbers.

Getting started
-------------------------------
The library currently offers a variety of types for handling bitsets, the most relevant ones being:

- `bitarray`: main type for standard bitset manipulation. 
- `sparse_bitarray`: main type for large and massive sets with few elements.

BITSCAN also offers a `bblock` namespace with many useful operations for 64-bit numbers.

To include the BITSCAN library in your project, add the header `#include "bitscan/bitscan.h"`. In the following example, a bitset of 100 elements is declared, containing only the 11th element (index numbering starts at 0). 

``` plaintext
#include <iostream>
#include "bitscan/bitscan.h"

int main(){
   bitarray bb(100);
   bb.set_bit(10);
   bb.set_bit(20);	

   std::cout << bb;	//screen ouptut [10, 20 (2)]
 }
```   
Many more examples are provided in the *tests* folder.  

EXAMPLES OF BITSCANNING OPERATIONS
-------------------------------

The BITSCAN library features specific optimisation techniques for *destructive* and *non-destructive* scans. In a destructive bit scan (DBS), whenever a 1-bit is read *it is removed from the bitset* (at the end of the process the bit string is empty). In the (conventional) non-destructive case (NDBS), the bitset remains unaltered. 

Here is an example of fast bit scan loop syntax in BITSCAN:

    bitarray bba(100);
    	//...
	bba.init_scan(bbo::NON_DESTRUCTIVE);
   	int nBit = BBObject::noBit;;
  	 while( (nBit=bba.next_bit()) != BBObject::noBit ){
   	     //Do something with nBit...
  	}   
The *init_scan* member function configures cache information appropriately. Currently four types of scans are supported: DESTRUCTIVE, NON\_DESTRUCTIVE and the reverse counterparts (DESTRUCTIVE\_REVERSE and NON\_DESTRUCTIVE\_REVERSE). For each type there is an appropriate family of functions. Here is another example, this time for a sparse bitset of 100000 bits:


    sparse_bitarray bba(100000);
    	//...

    if(bba.init_scan(bbo::DESTRUCTIVE)!= -1){
	   int nBit = BBObject::nobit;
   	   while( (nBit=bbi.next_bit_del()) != BBObject::noBit ){
		//Do something with nBit...
	   }
	}
In this case the *if* clause is necessary because sparse bitsets can have empty semantics (no memory allocation), which is checked in the init_scan function. Moreover, the scanning now differs from the previous case because it deletes each population member from the set once it is found (DESTRUCTIVE type). Finally, it is worth noting that scanning operations from different starting points in the bitset are also supported.


CONFIGURATION PARAMETERS
-------------------------

The file *bbconfig.h* contains customization parameters. An important parameter is:

1. POPCOUNT_64: Disable in processor architectures which do not support hardware population count assembly instructions. 


